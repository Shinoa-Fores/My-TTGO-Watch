#ifndef _bcrates_APP_H
    #define _bcrates_APP_H

    #ifdef NATIVE_64BIT
        #include <string>
        using namespace std;
        #define String string
    #endif

    void bcrates_app_setup();

    bool bcrates_wifictl_event_cb(EventBits_t event, void *arg);
    bool fetch_bc_rates(String apiKey, String pair1, String pair2);

    void build_main_page();
    void build_settings();

#endif // _bcrates_APP_H