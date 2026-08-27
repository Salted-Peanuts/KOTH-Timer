## KOTH Timer Documentation

This folder contains the main hardware and build documentation for the KOTH Timer.

The current recommended firmware release is **v0.3.1**.

v0.3.1 has been tested and is now the recommended version for new builds.

## PCBWay Project Link

This tested PCB revision is available as a PCBWay shared project:

[Order / view the KOTH Timer PCB v0.2 project on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

This link is useful for builders who want to order the tested PCB without manually uploading the Gerber files.

## Build Guide

Start here:

```text
docs/build-guide.md
```

The build guide covers:

* Required parts
* Required tools
* Wiring overview
* Arduino Nano ESP32 pinout
* Power wiring
* Button wiring
* Display wiring
* Battery indicator wiring
* Firmware upload
* Wi-Fi/admin page connection
* Event testing
* Troubleshooting


## BOM

The bill of materials is available in two formats:

```text
docs/BOM.xlsx
docs/BOM.csv
```

Use the Excel file if you want the formatted version.

Use the CSV file if you want something easier to view directly on GitHub.

## Schematic

The current wiring schematic is available as a PDF:

```text
docs/Wiring_schematic.pdf
```

Use the schematic together with the build guide and firmware pinout before soldering.


## Firmware Version

The current recommended firmware is:

```text
v0.3.1
```

The older v0.2 firmware is still available for reference, but new builders should use v0.3.


## Wi-Fi/Admin Interface

By default, the timer creates an open Wi-Fi network:

```text
SSID: KOTH-Timer
Password: none / open network
```

After connecting, open:

```text
http://10.10.10.1
```

Connection test page:

```text
http://10.10.10.1/ping
```

The timer does not provide internet access. This is normal.

Some phones may warn that the Wi-Fi network has no internet. Choose the option to stay connected or use the network anyway.


## Optional Wi-Fi Password

The Wi-Fi network is open by default for quick event setup.

To add a password, edit the firmware.

Find:

```cpp
static const char* AP_PASS = "";
```

Change it to a password with at least 8 characters:

```cpp
static const char* AP_PASS = "kothtimer";
```

Then upload the firmware again.


## Notes

This hardware is based on the tested prototype build.

Check all wiring carefully before powering the device.

Some parts may need different resistor values or pin assignments depending on the exact components used.

Use a multimeter before connecting the Arduino and displays to the battery power system.
