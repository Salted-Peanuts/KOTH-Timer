# KOTH-Timer

A DIY Arduino Nano ESP32 King of the Hill timer built for Nerf and other objective-based games.

I originally built this for local Nerf events, then kept refining it as people asked for build information. The current version uses two large team buttons, four TM1637 displays, battery monitoring, and a local referee webpage. It runs entirely on the ESP32, so it does not need an internet connection during a game.

## Current stable build

| Item | Current version |
|---|---|
| Firmware | **v0.4.0** |
| Firmware file | `Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino` |
| PCB | **v0.2-tested** |
| Admin page | `http://10.10.10.1/` |

PCB v0.2 is still the current tested board. A revised PCB is being worked on to include the newer v0.4 hardware changes directly.

### v0.4.0 hardware note

Firmware v0.4.0 was calibrated and tested with a **10 kΩ / 10 kΩ battery divider**.

If you are upgrading an older timer or assembling PCB v0.2 for v0.4.0, use:

```text
R1 = 10 kΩ
R2 = 10 kΩ
```

Older documentation and the original PCB v0.2 design files show 100 kΩ at R1/R2.

The buzzer added in v0.4.0 is optional. The timer works normally with no buzzer connected.

See [v0.4.0 hardware changes](docs/hardware-upgrade-v0.4.0.md) for the full upgrade notes.

## Features

- Two-team King of the Hill timer
- Four physical TM1637 displays
- Large illuminated team buttons
- Local phone-friendly referee/admin webpage
- Configurable match duration and team colours
- Pause, resume, reset and referee time correction
- Physical 5-second match start using both team buttons
- Battery voltage and 5-segment battery indicator
- Critical low-battery freeze and saved-match recovery
- Winner animation on the physical displays
- Optional buzzer feedback for scoring, start, victory and low battery
- No internet connection required during gameplay

## Game behaviour

Each team has its own countdown timer.

- Hold only Team A's button: Team A counts down.
- Hold only Team B's button: Team B counts down.
- Hold both buttons: the point is contested and neither team scores.
- Hold neither button: no scoring.
- The first team to reach zero wins.

A match can be started from the referee webpage, or by holding both team buttons for 5 seconds while the game is stopped.

## Referee/admin webpage

The timer creates its own Wi-Fi access point:

```text
SSID: KOTH-Timer
Password: none
```

After connecting, open:

```text
http://10.10.10.1/
```

Connection test:

```text
http://10.10.10.1/ping
```

Some phones and laptops will warn that the network has no internet access. That is expected; stay connected to the KOTH-Timer network.

Automatic captive-portal opening was removed in v0.4.0 because it was inconsistent across devices.

## Hardware

The current build uses:

| Part | Notes |
|---|---|
| Arduino Nano ESP32 | Main controller |
| 4 × TM1637 4-digit displays | Two displays per team |
| 2 × illuminated arcade buttons | Team controls and LED feedback |
| 1 × 18650 Li-ion cell | Main battery |
| 5 V step-up converter | Powers the 5 V rail |
| 5-segment LED bar graph | Physical battery indicator |
| 2 × 10 kΩ resistors | Battery divider for A0 |
| 220 Ω resistors | LED current limiting |
| Active buzzer | Optional |

The buzzer used in my reference build is the [Keyestudio Active Buzzer Alarm Module (KS0018)](https://www.keyestudio.com/products/keyestudio-active-buzzer-alarm-module-for-arduino).

It is wired:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

The buzzer is not required for the timer to operate.

Current BOM:

- [BOM.csv](docs/BOM.csv)
- [BOM.xlsx](docs/BOM.xlsx)

## PCB revisions

| Revision | Status |
|---|---|
| **v0.1** | First manufactured board. Kept for reference; known TM1637 display reliability issues. |
| **v0.2** | Manufactured, assembled and tested. Current recommended PCB. |
| **Next revision** | In development. Intended to include the v0.4 resistor and buzzer changes directly. |

### Using PCB v0.2 with firmware v0.4.0

PCB v0.2 remains usable with v0.4.0.

- Fit **10 kΩ** at R1 and R2 instead of the original 100 kΩ values.
- Add the active buzzer to D13 / 3.3 V / GND only if you want audio feedback.

The original v0.2 KiCad and Gerber files are intentionally left unchanged because they represent the board that was actually manufactured and tested.

## Documentation

- [Build guide](docs/build-guide.md)
- [v0.4.0 hardware changes](docs/hardware-upgrade-v0.4.0.md)
- [v0.4.0 release notes](docs/release-notes-v0.4.0.md)
- [Documentation index](docs/README.md)
- [Changelog](CHANGELOG.md)

## Photos

### Finished prototype

![KOTH-Timer finished prototype](Images/Prototype_ref_1.jpg)

### Inside wiring

![KOTH-Timer internal wiring](Images/PCB2.jpg)

### PCB

![KOTH-Timer custom PCB](Images/PCB3.jpg)

## Firmware

The firmware is written for the Arduino Nano ESP32 using Arduino IDE.

Keep the sketch in a folder with the same base name:

```text
Firmware/
  KOTH_Timer_v0_4_0/
    KOTH_Timer_v0_4_0.ino
```

External libraries:

- WebSockets
- TM1637Display

Wi-Fi, WebServer and Preferences are provided by the Arduino Nano ESP32 board package.

Previous versions are available from the [GitHub Releases](https://github.com/Salted-Peanuts/KOTH-Timer/releases) page.

## PCBWay

PCBWay has supported manufacturing for later KOTH Timer PCB revisions.

The tested PCB v0.2 is also available as a shared project:

[View KOTH Timer PCB v0.2 on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

That page represents the original v0.2 hardware. If you are building for firmware v0.4.0, use the current BOM and hardware notes in this repository.

PCBWay's manufacturing support helped with producing and testing real board revisions. The design notes, test results and recommendations in this repository are maintained independently.

## Licence

MIT Licence.
