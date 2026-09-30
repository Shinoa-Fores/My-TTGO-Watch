/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 *
 *   Patches for ESP32 + webserver use:
 *   - Capture buffer via MALLOC (PSRAM when available)
 *   - No LVGL image-cache resize during capture
 *   - Default save as BMP (low peak RAM vs LodePNG)
 *   - Yield during SPIFFS write so async_tcp can feed the task WDT
 *   - volatile screenshot_requested for deferred work from HTTP handlers
 ****************************************************************************/

#include "config.h"
#include <endian.h>
#include <string.h>
#include "screenshot.h"
#include "utils/alloc.h"
#include "utils/filepath_convert.h"
#include "gui/png_decoder/lv_png.h"
#include "hardware/motor.h"
#include "hardware/blectl.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_task_wdt.h"
#endif

/*
 * PNG needs a large LodePNG scratch area. BMP only needs the frame buffer
 * plus a small row buffer. Keep webserver up; yield during write instead.
 */
#ifndef SCREENSHOT_USE_PNG
    #define SCREENSHOT_USE_PNG 0
#endif

/* Set from HTTP / UI handlers; processed in main loop only */
volatile bool screenshot_requested = false;

static raw_img_grey_t *raw_grey = NULL;
static raw_img_rgb_t  *raw_rgb  = NULL;

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

#if !defined(NATIVE_64BIT)
static void screenshot_log_mem( const char *tag ) {
    log_i( "screenshot %s: free heap=%u, free PSRAM=%u",
           tag,
           (unsigned)ESP.getFreeHeap(),
#if defined(BOARD_HAS_PSRAM)
           (unsigned)ESP.getFreePsram()
#else
           0u
#endif
    );
}
#else
static void screenshot_log_mem( const char *tag ) {
    log_i( "screenshot %s", tag );
}
#endif

static void screenshot_free_buffers( void ) {
    if ( raw_grey ) {
        free( raw_grey );
        raw_grey = NULL;
    }
    if ( raw_rgb ) {
        free( raw_rgb );
        raw_rgb = NULL;
    }
}

#if !SCREENSHOT_USE_PNG
/**
 * 24-bit BGR BMP, bottom-up rows.
 * Yields every few rows so AsyncTCP / other WDT tasks can run.
 */
static bool screenshot_save_bmp( const char *filename, const raw_img_rgb_t *img,
                                 unsigned w, unsigned h ) {
    FILE *f = fopen( filename, "wb" );
    if ( !f ) {
        log_e( "screenshot: fopen failed for %s", filename );
        return false;
    }

    const uint32_t row_stride  = ( w * 3u + 3u ) & ~3u;
    const uint32_t pixel_bytes = row_stride * h;
    const uint32_t file_size   = 54u + pixel_bytes;

    uint8_t hdr[54];
    memset( hdr, 0, sizeof( hdr ) );
    hdr[0]  = 'B';
    hdr[1]  = 'M';
    hdr[2]  = (uint8_t)( file_size );
    hdr[3]  = (uint8_t)( file_size >> 8 );
    hdr[4]  = (uint8_t)( file_size >> 16 );
    hdr[5]  = (uint8_t)( file_size >> 24 );
    hdr[10] = 54;
    hdr[14] = 40;
    hdr[18] = (uint8_t)( w );
    hdr[19] = (uint8_t)( w >> 8 );
    hdr[20] = (uint8_t)( w >> 16 );
    hdr[21] = (uint8_t)( w >> 24 );
    hdr[22] = (uint8_t)( h );
    hdr[23] = (uint8_t)( h >> 8 );
    hdr[24] = (uint8_t)( h >> 16 );
    hdr[25] = (uint8_t)( h >> 24 );
    hdr[26] = 1;
    hdr[28] = 24;

    if ( fwrite( hdr, 1, 54, f ) != 54 ) {
        log_e( "screenshot: BMP header write failed" );
        fclose( f );
        return false;
    }

    uint8_t *row = (uint8_t *)MALLOC( row_stride );
    if ( !row ) {
        log_e( "screenshot: row buffer malloc failed" );
        fclose( f );
        return false;
    }
    memset( row, 0, row_stride );

    for ( int y = (int)h - 1; y >= 0; y-- ) {
        const rgb_t *src = &img->data[ (unsigned)y * w ];
        for ( unsigned x = 0; x < w; x++ ) {
            row[ x * 3 + 0 ] = src[x].b;
            row[ x * 3 + 1 ] = src[x].g;
            row[ x * 3 + 2 ] = src[x].r;
        }
        if ( fwrite( row, 1, row_stride, f ) != row_stride ) {
            log_e( "screenshot: BMP row write failed at y=%d", y );
            free( row );
            fclose( f );
            return false;
        }

#if !defined(NATIVE_64BIT)
        /* Keep async_tcp and other tasks alive during SPIFFS write */
        if ( ( y & 3 ) == 0 ) {
            esp_task_wdt_reset();
            vTaskDelay( 1 );
        }
#endif
    }

    free( row );
    fclose( f );
    return true;
}
#endif /* !SCREENSHOT_USE_PNG */

void screenshot_setup( void ) {
    raw_grey = NULL;
    raw_rgb  = NULL;
    screenshot_requested = false;
}

void screenshot_take( void ) {
    lv_disp_drv_t driver;
    lv_disp_t *system_disp;

    /* Do not call lv_img_cache_set_size() here — it spikes allocations. */

    screenshot_log_mem( "before allocate" );

#if defined( MONOCHROME ) || defined( MONOCHROME_4BIT ) || defined( MONOCHROME_EINK )
    if ( !raw_grey ) {
        raw_grey = (raw_img_grey_t *)MALLOC( sizeof( raw_img_grey_t ) );
        if ( raw_grey == NULL ) {
            log_e( "screenshot grey malloc failed" );
            screenshot_log_mem( "alloc failed" );
            return;
        }
    }
#else
    if ( !raw_rgb ) {
        raw_rgb = (raw_img_rgb_t *)MALLOC( sizeof( raw_img_rgb_t ) );
        if ( raw_rgb == NULL ) {
            log_e( "screenshot rgb malloc failed" );
            screenshot_log_mem( "alloc failed" );
            return;
        }
    }
#endif

    screenshot_log_mem( "after allocate" );
    log_i( "take screenshot" );

    system_disp = lv_disp_get_default();
    if ( !system_disp ) {
        log_e( "screenshot: no default display" );
        screenshot_free_buffers();
        return;
    }

    driver.flush_cb = system_disp->driver.flush_cb;
    system_disp->driver.flush_cb = screenshot_disp_flush;
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now( system_disp );
    system_disp->driver.flush_cb = driver.flush_cb;

    screenshot_log_mem( "after capture" );
}

void screenshot_save( void ) {
    if ( raw_grey == NULL && raw_rgb == NULL ) {
        log_e( "no screenshot memory allocated" );
        return;
    }

    screenshot_log_mem( "before save" );

    char filename[256] = "";
    filepath_convert( filename, sizeof( filename ), SCREENSHOT_FILE_NAME );
    remove( filename );

    if ( raw_grey ) {
        log_i( "save 8bit grey screenshot" );
        lv_8grey_as_png( filename, (const uint8_t *)raw_grey, RES_X_MAX, RES_Y_MAX );
        free( raw_grey );
        raw_grey = NULL;
    }

    if ( raw_rgb ) {
#if SCREENSHOT_USE_PNG
        log_i( "save rgb screenshot as PNG" );
        lv_rgb_as_png( filename, (const uint8_t *)raw_rgb, RES_X_MAX, RES_Y_MAX );
#else
        log_i( "save rgb screenshot as BMP (low peak RAM)" );
        /*
         * Path may still be SCREENSHOT_FILE_NAME (e.g. /spiffs/screen.png).
         * Content is BMP. Rename on the host or change the define in screenshot.h.
         */
        if ( !screenshot_save_bmp( filename, raw_rgb, RES_X_MAX, RES_Y_MAX ) ) {
            log_e( "screenshot BMP save failed" );
        }
#endif
        free( raw_rgb );
        raw_rgb = NULL;
    }

    screenshot_log_mem( "after save" );
    motor_vibe( 10, true );
}

void screenshot_process_request( void ) {
    bool ble_was_on = false;

#if !defined(NATIVE_64BIT)
    ble_was_on = blectl_get_event( BLECTL_ON );
    if ( ble_was_on ) {
        log_i( "screenshot: pausing BLE" );
        blectl_off();
        /* Let NimBLE / controller settle before SPIFFS work */
        vTaskDelay( pdMS_TO_TICKS( 150 ) );
    }
#endif

    screenshot_take();
    screenshot_save();

#if !defined(NATIVE_64BIT)
    if ( ble_was_on ) {
        vTaskDelay( pdMS_TO_TICKS( 50 ) );
        log_i( "screenshot: resuming BLE" );
        blectl_on();
    }
#endif
}

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ) {
    uint32_t x, y;
    lv_color_t *color = color_p;

    if ( raw_rgb == NULL && raw_grey == NULL ) {
        log_e( "no screenshot memory allocated" );
        lv_disp_flush_ready( disp_drv );
        return;
    }

    const lv_coord_t hor = lv_disp_get_hor_res( NULL );

    for ( y = area->y1; y <= (uint32_t)area->y2; y++ ) {
        for ( x = area->x1; x <= (uint32_t)area->x2; x++ ) {
            uint8_t r, g, b;
            switch ( LV_COLOR_DEPTH ) {
                case 8:
                    r = LV_COLOR_GET_R( *color ) << 5;
                    g = LV_COLOR_GET_G( *color ) << 5;
                    b = LV_COLOR_GET_B( *color ) << 6;
                    break;
                case 16:
                    r = LV_COLOR_GET_R( *color ) << 3;
                    g = LV_COLOR_GET_G( *color ) << 2;
                    b = LV_COLOR_GET_B( *color ) << 3;
                    break;
                case 32:
                    r = LV_COLOR_GET_R( *color );
                    g = LV_COLOR_GET_G( *color );
                    b = LV_COLOR_GET_B( *color );
                    break;
                default:
                    r = g = b = 0;
                    break;
            }
            if ( raw_grey ) {
                raw_grey->data[ ( y * hor ) + x ].grey = lv_color_brightness( *color );
            }
            if ( raw_rgb ) {
                const uint32_t idx = y * hor + x;
                raw_rgb->data[ idx ].r = r;
                raw_rgb->data[ idx ].g = g;
                raw_rgb->data[ idx ].b = b;
            }
            color++;
        }
    }
    lv_disp_flush_ready( disp_drv );
}
