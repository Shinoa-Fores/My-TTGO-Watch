<hr/>

# My-TTGO-Watch

A GUI named hedge for smartwatch like devices based on ESP32. Currently support for T-Watch2020 (V1,V2,V3), T-Watch2021. This fork has a BASIC interpreter as found in FantasyFactory's fork of this repo.

## Features

* BLE communication
* Time synchronization via BLE
* Notification via BLE
* Step counting
* Wake-up on wrist rotation
* Quick actions:

  * WiFi
  * Bluetooth
  * GPS
  * Luminosity
  * Sound volume

* Multiple watch faces:

  * Embedded (digital)
  * [Community based watchfaces](https://sharandac.github.io/My-TTGO-Watchfaces/)

* Multiple 'apps':

  * Music (control the playback of the music on your phone)
  * Navigation (displays navigation instructions coming from the companion app)
  * Map (displays a map)
  * Notification (displays the last notification received)
  * Stopwatch (with all the necessary functions such as play, pause, stop)
  * Alarm
  * Step counter (displays the number of steps and daily objective)
  * Weather
  * Calendar
  * IR remote
  * ...

* Companion apps: Gadgetbridge

## Install

Clone this repository.

Install PlatformIO core:
```bash
pip install -U platformio
```

Upload data file:
```bash
pio run -t uploadfs
```

Build and upload:
```bash
pio run -t upload
```

Please check out
  https://github.com/Shinoa-Fores/My-TTGO-Watch/blob/709ed0c5863435aa966c1d6f44552ddc0909a57c/src/hardware/wifictl.cpp#L256-L261
to setup your wifi when wps or input via display is not possible.

# Known issues

* the webserver crashes the ESP32 really often
* the battery indicator is not accurate, rather a problem with the power management unit ( axp202 )

# How to use

Cf. [Usage](USAGE.md)

# Forks that are recommended

[Pickelhaupt](https://github.com/Pickelhaupt/EUC-Dash-ESP32)<br>
[FantasyFactory](https://github.com/FantasyFactory/My-TTGO-Watch)<br>
[NorthernDIY](https://github.com/NorthernDIY/My-TTGO-Watch)<br>
[linuxthor](https://github.com/linuxthor/Hackers-TTGO-Watch)<br>
[d03n3rfr1tz3](https://github.com/d03n3rfr1tz3/TTGO.T-Watch.2020)<br>
[lunokjod](https://github.com/lunokjod/watch)<br>

# For the programmers

Cf. [contribution guide](CONTRIBUTING.md)

# Interface

## TTGO T-Watch 2020

![screenshot](images/screen1.png)
![screenshot](images/screen2.png)
![screenshot](images/screen3.png)
![screenshot](images/screen4.png)
![screenshot](images/screen5.png)
![screenshot](images/screen6.png)
![screenshot](images/screen7.png)
![screenshot](images/screen8.png)
![screenshot](images/screen9.png)
![screenshot](images/screen10.png)
![screenshot](images/screen11.png)
![screenshot](images/screen12.png)

## TTGO T-Watch 2021

![screenshot](images/twatch2021_img1.png)
![screenshot](images/twatch2021_img2.png)
![screenshot](images/twatch2021_img3.png)

# Contributors

Special thanks to the following people for their help:

[5tormChild](https://github.com/5tormChild)<br>
[bwagstaff](https://github.com/bwagstaff)<br>
[chrismcna](https://github.com/chrismcna)<br>
[datacute](https://github.com/datacute)<br>
[fliuzzi02](https://github.com/fliuzzi02)<br>
[guyou](https://github.com/guyou)<br>
[jakub-vesely](https://github.com/jakub-vesely)<br>
[joshvito](https://github.com/joshvito)<br>
[JoanMCD](https://github.com/JoanMCD)<br>
[NorthernDIY](https://github.com/NorthernDIY)<br>
[Neuroplant](https://github.com/Neuroplant)<br>
[paulstueber](https://github.com/paulstueber)<br>
[pavelmachek](https://github.com/pavelmachek)<br>
[rnisthal](https://github.com/rnisthal)<br>
[ssspeq](https://github.com/ssspeq)<br>

and the following projects:

[ArduinoJson](https://github.com/bblanchon/ArduinoJson)<br>
[AsyncTCP](https://github.com/me-no-dev/AsyncTCP)<br>
[ESP32SSDP](https://github.com/luc-github/ESP32SSDP)<br>
[ESP32-targz](https://github.com/tobozo/ESP32-targz)<br>
[ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)<br>
[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)<br>
[LVGL](https://github.com/lvgl)<br>
[NimBLE-Arduino]()h2zero/NimBLE-Arduino<br>
[pubsubclient](https://github.com/knolleary/pubsubclient)<br>
[TinyGPSPlus](mikalhart/TinyGPSPlus)<br>
[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)<br>
[TTGO_TWatch_Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)<br>

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.

