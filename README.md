<hr/>

# My-TTGO-Watch

A GUI named bushcat for smartwatch like devices based on ESP32. Currently support for T-Watch2020 (V3) ONLY!

## Features

* BLE communication
* Time synchronization via BLE
* Notification via BLE
* Step counting
* Wake-up on wrist rotation
* Quick actions:

  * WiFi
  * Bluetooth
  * IR
  * Luminosity
  * Sound volume

* Multiple watch faces:

  * Embedded (digital)
  * [My collection of various watchfaces.](https://github.com/Shinoa-Fores/My-TTGO-Watchfaces/)

* Multiple 'apps':

  * Music (control the playback of the music on your phone)
  * Notification (displays the last notification received)
  * Stopwatch (with all the necessary functions such as play, pause, stop)
  * Alarm
  * Step counter (displays the number of steps and daily objective)
  * Weather
  * Calendar
  * IR remote
  * BTC/XMR price in USD
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

Development and debug monitor terminal using:
```bash
pio device monitor -f esp32_exception_decoder
```

Please check out
  https://github.com/Shinoa-Fores/My-TTGO-Watch/blob/709ed0c5863435aa966c1d6f44552ddc0909a57c/src/hardware/wifictl.cpp#L256-L261
to setup your wifi when wps or input via display is not possible. The most reliable method is to set the endpoint to you mobile phone's hotspot in `wificfg.json` and upload to SPIFFS.

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



