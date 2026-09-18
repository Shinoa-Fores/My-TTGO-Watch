#include "config.h"

#include "quickglui/quickglui.h"
#include "bc_rates.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "hardware/wifictl.h"

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(bc_rates_64px);
LV_FONT_DECLARE(Ubuntu_48px);

static SynchronizedApplication bcratesApp;
static JsonConfig config("fx-rates.json");

static String apiKey, mainPair, secondPair; // Unused but kept for config file compatibility
static String mainPairValue, secondPairValue, updatedAt;
static Label lblCurrency1, lblCurrency2, lblUpdatedAt;
static Label lblTitle1, lblTitle2;

static Style big;

// Track the last successful update time (in seconds)
static uint32_t last_update_time = 0;

/*
 * setup routine for application
 */
void bcrates_app_setup() {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif

    bcratesApp.init("BTC price", &bc_rates_64px, true, 1, 1);
    
    // Build and configure application
    build_main_page();
    build_settings();

    // Executed when user click "refresh" button or when a WiFi connection is established
    bcratesApp.synchronizeActionHandler([](SyncRequestSource source) {
        auto result = fetch_bc_rates(apiKey, mainPair, secondPair);
        lblUpdatedAt.text(updatedAt);
        if (result)
        {
            bcratesApp.icon().widgetText(mainPairValue);
            lblCurrency1.text(mainPairValue).alignInParentCenter(0, -30);
            lblCurrency2.text(secondPairValue).alignOutsideBottomMid(lblCurrency1);
            bcratesApp.icon().showIndicator(ICON_INDICATOR_OK);
            
            // Record the time of successful update (resets the 6-hour timer)
            last_update_time = millis() / 1000;
        } else {
            // In case of fail
            bcratesApp.icon().showIndicator(ICON_INDICATOR_FAIL);
        }
    });
    
    // We want to start syncronization every time when WiFi connection is established.
    wifictl_register_cb(WIFICTL_CONNECT | WIFICTL_OFF, bcrates_wifictl_event_cb, "bcrates app widget");
}

bool bcrates_wifictl_event_cb(EventBits_t event, void *arg) {
    switch(event) {
        case WIFICTL_CONNECT:
            bcratesApp.icon().hideIndicator();
            if ( config.getBoolean("autosync", false ) ) {
                uint32_t current_time = millis() / 1000;
                // 6 hours = 21600 seconds. 
                // If it's the first run (last_update_time == 0) or 6 hours have passed, allow sync.
                if (last_update_time == 0 || (current_time - last_update_time) > 21600) {
                    bcratesApp.startSynchronization(SyncRequestSource::ConnectionEvent);
                } else {
                    log_i("bcrates: Skipping auto-sync, last update was %lu seconds ago", current_time - last_update_time);
                }
            }
            break;

        case WIFICTL_OFF:
            bcratesApp.icon().hideIndicator();
            break;
    }
    return true;
}

void build_main_page()
{
    big = Style::Create(ws_get_mainbar_style(), true);
    big.textFont(&Ubuntu_48px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = bcratesApp.mainPage();

    // BTC price (top)
    lblCurrency1 = Label(&screen);
    lblCurrency1.text("---")
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(big, true)
        .alignInParentCenter(0, -30);

    // XMR price (bottom)
    lblCurrency2 = Label(&screen);
    lblCurrency2.text("---")
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(big, true)
        .alignOutsideBottomMid(lblCurrency1, 0, 10);

    // Updated at
    lblUpdatedAt = Label(&screen);
    lblUpdatedAt.text("loading...")
        .alignText(LV_LABEL_ALIGN_LEFT)
        .alignInParentTopLeft(5, 5);
}

void build_settings()
{
    // Create full options list and attach items to variables
    // apiKey, pair1, and pair2 are kept only for compatibility with old settings files – they are ignored now
    config.addString("apikey", 80).assign(&apiKey);   // was 32
    config.addString("pair1", 12, "BTC").assign(&mainPair);
    config.addString("pair2", 12).assign(&secondPair);
    config.addBoolean("autosync", false);
    config.addBoolean("widget", false);

    // Switch desktop widget state based on the current settings when changed
    config.onLoadSaveHandler([](JsonConfig& cfg) {
        bool widgetEnabled = cfg.getBoolean("widget");
        if (widgetEnabled)
            bcratesApp.icon().registerDesktopWidget("bc", &bc_rates_64px);
        else
            bcratesApp.icon().unregisterDesktopWidget();
    });

    bcratesApp.useConfig(config, true); // true - auto create settings page widgets
}

bool fetch_bc_rates(String apiKey, String pair1, String pair2) {
    log_i("===== fetch_bc_rates (BTC + XMR) START =====");
    log_i("Free heap: %d", ESP.getFreeHeap());

    if (WiFi.status() != WL_CONNECTED) {
        updatedAt = "No WiFi";
        return false;
    }

    // Request both BTC and XMR in one call
    char url[280];
    if (apiKey.length() == 64) {
        snprintf(url, sizeof(url),
                 "https://min-api.cryptocompare.com/data/pricemulti?fsyms=BTC,XMR&tsyms=USD&api_key=%s",
                 apiKey.c_str());
    } else {
        snprintf(url, sizeof(url),
                 "https://min-api.cryptocompare.com/data/pricemulti?fsyms=BTC,XMR&tsyms=USD");
    }

    log_i("URL: %s", url);

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(10);

    HTTPClient https;
    https.setTimeout(10000);
    https.setUserAgent("TTGO-Watch/1.0");
    https.setReuse(false);

    if (!https.begin(client, url)) {
        updatedAt = "begin() failed";
        return false;
    }

    int httpCode = https.GET();
    log_i("HTTP code: %d", httpCode);

    if (httpCode != HTTP_CODE_OK) {
        updatedAt = String("HTTP ") + String(httpCode);
        https.end();
        return false;
    }

    String payload = https.getString();
    https.end();

    log_i("Payload: %s", payload.c_str());

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        updatedAt = String(err.c_str());
        return false;
    }

    // BTC
    if (doc["BTC"]["USD"].isNull()) {
        updatedAt = "No BTC price";
        return false;
    }
    float btc = doc["BTC"]["USD"].as<float>();
    mainPairValue = String(btc, 2);          // Top line

    // XMR
    if (!doc["XMR"]["USD"].isNull()) {
        float xmr = doc["XMR"]["USD"].as<float>();
        secondPairValue = String(xmr, 2);    // Bottom line
    } else {
        secondPairValue = "---";
    }

    updatedAt = "[BTC/XMR prices:]";
    log_i("BTC: %s   XMR: %s", mainPairValue.c_str(), secondPairValue.c_str());
    return true;
}
