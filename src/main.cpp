#include "lvgl.h"
#include "gui/gui.h"
#include "gui/screenshot.h"

#include "hardware/hardware.h"
#include "hardware/powermgm.h"
#include "utils/ftpserver/ftpserver.h"
#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"

#include "app/calc/calc_app.h"
#include "app/FindPhone/FindPhone.h"
#include "app/kodi_remote/kodi_remote_app.h"
#include "app/mail/mail_app.h"
#include "app/sshclient/sshclient_app.h"
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

#if defined( NATIVE_64BIT )
    /**
     * for non arduino
     */                 
    void setup( void );
    void loop( void );

    int main( void ) {
        setup();
        while( 1 ) { loop(); };
        return( 0 );
    }
#endif // NATIVE_64BIT


void setup() {  
    /**
     * hardware setup
     */
    hardware_setup();
    Serial.println("hardware_post_setup() done");
    /**
     * gui setup
     */
    gui_setup();
    Serial.println("gui_setup() done");
    /**
     * apps here
     */
    weather_app_setup();
    stopwatch_app_setup();
    tracker_app_setup();
    alarm_clock_setup();
    activity_app_setup();
    calendar_app_setup();
    astro_app_setup();
    mail_app_setup();
    IRController_setup();
    bcrates_app_setup();
    FindPhone_setup();
    wifimon_app_setup();
    calc_app_setup();
    kodi_remote_app_setup();
    sshclient_app_setup();
    Serial.println("apps setup done");
    
    /**
     * post hardware setup
     */
    hardware_post_setup();
    Serial.println("hardware_post_setup() done");
       
    // ========== WELCOME NOTIFICATION ON THE WATCH ==========
    bluetooth_message_queue_msg(
        "{\"t\":\"notify\","
        "\"id\":1,"
        "\"src\":\"[System]\","
        "\"title\":\"Welcome User!\","
        "\"body\":\"GM, enjoy your day! :)\"}"
    );
}

void loop(){
    powermgm_loop();
    ftpserver_handle();

    if (screenshot_requested) {
        screenshot_requested = false;

        screenshot_process_request();
    }    
}
