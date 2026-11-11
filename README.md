# KOTH-Timer

A DIY Arduino Nano ESP32 King of the Hill timer for Nerf and other objective-based games.

This started as a timer I built for local Nerf events. The project now has a tested PCB, four physical displays, two large team buttons, battery monitoring and a local referee webpage.

## Current version

**Firmware:** `v0.4.0`

```text
Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino
```

**PCB:** `v0.2-tested`

```text
PCB/v0.2-tested/
```

PCB v0.2 is still the current tested board. A revised PCB is being worked on to fold the newer v0.4 hardware changes into the board itself.

### Before using v0.4.0

The battery divider changed from **100 kΩ / 100 kΩ** to **10 kΩ / 10 kΩ**.

If you are upgrading an older timer or assembling PCB v0.2 for v0.4.0, fit:

```text
R1 = 10 kΩ
R2 = 10 kΩ
```

The buzzer is **optional**. v0.4.0 works normally with no buzzer connected.

See [v0.4.0 hardware upgrade notes](docs/hardware-upgrade-v0.4.0.md) if you are upgrading an existing build.

## How the game works

Each team has its own countdown timer.

- Hold only Team A's button: Team A counts down.
- Hold only Team B's button: Team B counts down.
- Hold both buttons: contested, no scoring.
- Hold neither button: no scoring.
- First team to reach zero wins.

The timer can be controlled from the referee webpage. In v0.4.0, a match can also be started from the physical buttons by holding both team buttons for 5 seconds.

## v0.4.0 changes

- 5-second physical match start
- optional active buzzer on D13
- start, scoring, victory and low-battery sounds when a buzzer is fitted
- new winner animation
- 10 kΩ / 10 kΩ battery divider and revised calibration
- revised 5-segment battery gauge
- low-battery `LO:LO` warning below 3.40 V
- automatic match freeze and save on critical battery
- paused match recovery after a battery swap/reboot
- cleaned-up referee webpage behaviour
- captive portal removed; connect manually to the admin page

Full notes: [v0.4.0 release notes](docs/release-notes-v0.4.0.md)

## Referee webpage

The timer creates its own Wi-Fi network:

```text
SSID: KOTH-Timer
Password: none
```

Then open:

```text
http://10.10.10.1/
```

Connection test:

```text
http://10.10.10.1/ping
```

The timer does not provide internet access. If your phone or laptop warns about that, stay connected to the KOTH-Timer network.

Automatic browser opening was removed from v0.4.0 because it was inconsistent across devices.

## Optional buzzer

v0.4.0 supports a 3.3–5 V active buzzer:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

With a buzzer fitted, the physical start sequence is:

```text
BEEP ... BEEP ... BEEEEEEP
                    ^
                    GO
```

The game goes live at the start of the long beep.

No buzzer is required for normal operation.

## Hardware

Current build:

- Arduino Nano ESP32
- single 18650 cell
- 5 V boost converter
- fuse and power switch
- 2 illuminated arcade buttons
- 4 TM1637 4-digit displays
- 5-segment battery indicator
- battery-check button
- 2 × 10 kΩ battery-divider resistors
- 220 Ω LED resistors
- optional active buzzer
- prototype wiring or PCB v0.2

Current BOM:

```text
docs/BOM.csv
docs/BOM.xlsx
```

## PCB status

| Revision | Status |
|---|---|
| v0.1 | First manufactured board. Known TM1637 display issues. Reference only. |
| v0.2 | Manufactured, assembled and tested. Current recommended PCB. |
| Next revision | In development. Will include the v0.4-era hardware changes. |

### Using PCB v0.2 with v0.4.0

PCB v0.2 is still usable.

For v0.4.0:

- use 10 kΩ at R1 and R2 instead of the original 100 kΩ values
- optionally connect the active buzzer to D13, 3.3 V and GND

The original v0.2 KiCad and Gerber files are left unchanged because they represent the board that was actually manufactured and tested.

## PCBWay

PCB v0.2 is available as a shared PCBWay project:

[View KOTH Timer PCB v0.2 on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

That page shows the original v0.2 hardware. If you are running firmware v0.4.0, use the current BOM and the upgrade notes in this repo.

PCB manufacturing for this project has been supported by **PCBWay**.

[PCBWay](https://pcbway.com/g/EGJ27l)

## Docs

- [Build guide](docs/build-guide.md)
- [v0.4.0 hardware changes](docs/hardware-upgrade-v0.4.0.md)
- [v0.4.0 release notes](docs/release-notes-v0.4.0.md)
- [Documentation index](docs/README.md)

## Photos

### Finished prototype

![KOTH-Timer finished prototype](Images/Prototype_ref_1.jpg)

### Inside wiring

![KOTH-Timer internal wiring](Images/PCB2.jpg)

### PCB

![KOTH-Timer custom PCB](Images/PCB3.jpg)

## Firmware

The firmware is written for the Arduino Nano ESP32 using Arduino IDE.

Keep the sketch in a folder with the same name:

```text
Firmware/
  KOTH_Timer_v0_4_0/
    KOTH_Timer_v0_4_0.ino
```

External libraries:

- WebSockets
- TM1637Display

Wi-Fi, WebServer and Preferences come from the Arduino Nano ESP32 board package.

## Licence

MIT Licence.
