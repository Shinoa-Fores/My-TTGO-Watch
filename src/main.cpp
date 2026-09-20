#include "lvgl.h"
#include "gui/gui.h"
#include "gui/screenshot.h"

#include "hardware/hardware.h"
#include "hardware/powermgm.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"   // ← needed for the notification

#include "app/calc/calc_app.h"
#include "app/FindPhone/FindPhone.h"
#include "app/gps_status/gps_status.h"
#include "app/kodi_remote/kodi_remote_app.h"
#include "app/osmand/osmand_app.h"
#include "app/powermeter/powermeter_app.h"
#include "app/osmmap/osmmap_app.h"
#include "app/mail/mail_app.h"
#include "app/stopwatch/stopwatch_app.h"
#include "app/astro/astro_app.h"
#include "app/wifimon/wifimon_app.h"
#include "app/calendar/calendar.h"
#include "app/weather/weather.h"
#include "app/activity/activity.h"
#include "app/tracker/tracker_app.h"
#include "app/bc_rates/bc_rates.h"
#include "app/IRController/IRController.h"
#include "app/alarm_clock/alarm_clock.h"
#include "app/compass/compass_app.h"

//HACKING
#include "app/sshclient/sshclient_app.h"
#include "app/bluebox/bluebox_app.h"
#include "app/silverbox/silverbox_app.h"
#include "app/netscan/netscan_app.h"
#include "app/ping/ping_app.h"
#include "app/subnet/subnet_app.h"
#include "app/iplookup/iplookup_app.h"
#include "app/my_basic/my_basic_app.h"

#if defined( NATIVE_64BIT )
    void setup( void );
    void loop( void );

    int main( void ) {
        setup();
        while( 1 ) { loop(); };
        return( 0 );
    }
#endif // NATIVE_64BIT

extern volatile bool screenshot_requested;

void setup() {
    /**
     * hardware setup
     */
    hardware_setup();
    /**
     * gui setup
     */
    gui_setup();
    /**
     * apps here
     */        
    osmmap_app_setup();
    weather_app_setup();
    compass_app_setup();
    stopwatch_app_setup();
    tracker_app_setup();
    alarm_clock_setup();
    activity_app_setup();
    calendar_app_setup();
    astro_app_setup();
    mail_app_setup();
    gps_status_setup();
    IRController_setup();
    kodi_remote_app_setup();
    osmand_app_setup();
    bcrates_app_setup();
    powermeter_app_setup();
    FindPhone_setup();;
    calc_app_setup();
    netscan_app_setup();
    subnet_app_setup();
    ping_app_setup();
    iplookup_app_setup();    
    wifimon_app_setup();
    sshclient_app_setup();
    my_basic_app_setup();
#if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
    bluebox_app_setup();
    silverbox_app_setup();
#endif      
    
    /**
     * post hardware setup
     */
    hardware_post_setup();

    // ========== WELCOME NOTIFICATION ON THE WATCH ==========
    // This appears in the watch's own notification system
    bluetooth_message_queue_msg(
        "{\"t\":\"notify\","
        "\"id\":1,"
        "\"src\":\"[System]\","
        "\"title\":\"Welcome Shinoa!\","
        "\"body\":\"GM, enjoy your day! :)\"}"
    );
}

void loop(){
    powermgm_loop();

    if (screenshot_requested) {
        screenshot_requested = false;
        screenshot_take();
        screenshot_save();
    }    
}
