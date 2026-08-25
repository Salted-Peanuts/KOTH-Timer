# KOTH-Timer

A DIY Arduino Nano ESP32 King of the Hill timer for Nerf, foam flinging, skirmish games, and objective-based events.

KOTH-Timer is an open-source physical game timer with arcade buttons, TM1637 displays, battery monitoring, and a local phone-friendly referee/admin web interface. It is designed for two-team King of the Hill games where players physically hold a button to capture the objective.

This project is based on a working timer that has been tested successfully at local Nerf events. A few people in the hobby scene asked how to build their own, so this repo exists to make that possible.

The goal is to keep it practical, buildable, and easy to modify.


## Current Recommended Version

The current recommended firmware release is:

```text
v0.3
```

v0.3 has been tested at an event and is now the recommended version for new builds.

v0.2 is still available for reference as an earlier prototype release.


## What It Does

KOTH-Timer is a physical objective timer for two-team King of the Hill style games.

Players press their team’s button to capture the objective. The timer tracks control time and shows the game state through:

* Large physical displays
* Button LED feedback
* A local referee/admin web interface

The timer runs locally from the ESP32, so it does not need internet access during gameplay.


## Prototype Photos

### Finished Prototype

![KOTH-Timer Timer finished prototype](Images/Prototype_ref_1.jpg)

### Inside Wiring

![KOTH-Timer Timer internal wiring](Images/Prototype_ref_3.jpg)


## Features

* Two-team King of the Hill timer
* Physical arcade button controls
* LED button feedback
* Four TM1637 4-digit display modules
* Local ESP32-hosted web interface
* Phone-friendly referee/admin controls
* Configurable match duration
* Configurable team colours
* Battery voltage display
* Pause and resume controls
* Reset lockout to prevent accidental resets
* Game-over winner display
* Live referee time adjustment during gameplay
* Open Wi-Fi network by default for quick event setup
* Optional Wi-Fi password support
* No internet required during gameplay


## Referee/Admin Connection

The timer creates its own local Wi-Fi network.

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

Some phones may warn that the network has no internet. This is normal. Choose the option to stay connected or use the network anyway.


## Optional Wi-Fi Password

The Wi-Fi network is open by default so referees can connect quickly during events.

If you want to add a password, edit the firmware near the top of the code.

Find:

```cpp
static const char* AP_PASS = "";
```

Change it to a password with at least 8 characters:

```cpp
static const char* AP_PASS = "kothtimer";
```

Then upload the firmware again.


## Who This Is For

This project is for hobbyists who want to build their own objective timer for games such as:

* Nerf events
* Foam flinging games
* Gel blaster games where legal
* Airsoft-style objective games where legal
* Backyard or club-based capture games
* Custom scenario games

You do not need to be an expert programmer, but you should be comfortable with basic wiring, soldering, and flashing an ESP32.


## Basic Hardware Needed

The current build uses:

* Arduino Nano ESP32
* 18650 Li-ion cell
* 18650 battery holder
* 5 V boost converter
* Fuse
* Rocker power switch
* 2 large arcade buttons with LEDs
* 4 TM1637 4-digit display modules
* 5-segment battery indicator
* Battery display button
* Resistors
* Wire
* Enclosure
* USB cable for flashing
* Basic soldering tools

See the `docs` folder for the BOM, schematic, and full build guide.


## Build Guide

Start here:

[Build Guide](docs/build-guide.md)

The build guide covers:

* Parts required
* Tools required
* Power wiring
* Arduino Nano ESP32 pinout
* Button wiring
* Display wiring
* Battery indicator wiring
* Firmware upload
* Wi-Fi connection
* Event-day testing
* Troubleshooting


## Firmware

The firmware is written for the Arduino Nano ESP32 using the Arduino IDE.

Current recommended firmware:

```text
Firmware/KOTH_Timer_v0_3/KOTH_Timer_v0_3.ino
```

For Arduino IDE compatibility, the `.ino` file should be inside a folder with the same name as the sketch.


## Hardware Files

Hardware documentation is in the `docs` folder.

Current files include:

```text
docs/BOM.xlsx
docs/BOM.csv
docs/Wiring_schematic.pdf
docs/build-guide.md
```
## PCB Hardware Status

The firmware and hand-wired prototype are working, but the PCB designs should be treated by revision.

PCB revision	Status	Recommendation
v0.1	Manufactured and partially working	Not recommended for new builds
v0.2	In manufacturing / not yet tested	Wait for test results before ordering
PCB v0.1

The first PCB revision was manufactured and the timer is functional through the web UI, but this board has known display reliability issues.

### Known v0.1 issue:

Some TM1637 displays may show interference, flicker, incorrect segments, or unstable output.

Likely causes include display power noise, shared clock routing, lack of local display decoupling, and signal integrity issues on the display lines.

The v0.1 PCB files are kept in the repo for reference, review, and comparison, but new builders should not order this revision unless they are comfortable debugging and modifying the board.

### PCB v0.2

PCB v0.2 is the next hardware revision and is currently in manufacturing.

v0.2 is intended to improve display reliability and board layout based on lessons from v0.1, including better display power decoupling and cleaner display signal routing.

Until v0.2 has been assembled and tested, it should be treated as untested hardware.

The currently recommended build remains the tested v0.3 firmware with the original hand-wired/prototype hardware approach.

## Supported By PCBWay

PCB manufacturing for the newer KOTH-Timer PCB revisions has been financially donated by PCBWay.

PCBWay are helping support this open-source project by providing me with free PCB prototype manufacturing while in development. They are also belivers in allowing the design files, firmware, and documentation to remain fully open source.

Their support helps make it easier to test real hardware revisions, improve the PCB design, and share the results back with other builders.

You can check out PCBWay’s PCB manufacturing and assembly services here:

[PCBWay](https://pcbway.com/g/EGJ27l)

Thanks to PCBWay for supporting open-source hardware creators and helping this project move from a hand-wired prototype toward a cleaner PCB-based build.

Note: PCBWay provided the PCB manufacturing for this project, but the design, testing notes, known issues, and recommendations in this repo are written independently. Untested PCB revisions should still be treated as experimental until they are built and verified.

## Future Development

Planned future work may include:

* Testing and reviewing the first KiCad PCB design
* Improving hardware documentation
* Adding more photos and wiring diagrams
* Adding extra game modes beyond King of the Hill
* Improving the referee/admin interface based on event feedback
* Exploring a Bluetooth or app-based version if there is enough interest

The v0.3 firmware is event-tested and is the current recommended release.

Any early PCB design should be treated as untested hardware until it has been manufactured, assembled, and tested.


## Project Status

This is a hobby project, not a commercial product.

The current firmware has been tested at an event, but builders should still test their own wiring and setup before using the timer in a game.

If you build one, feel free to modify it for your own event rules, enclosure, buttons, battery setup, or game style.


## Related Uses

This project may be useful for:

- Nerf King of the Hill games
- Foam flinging objective games
- DIY skirmish game timers
- Arduino ESP32 game props
- Capture point timers
- Referee-controlled event timers
- Local Wi-Fi game control panels


## Licence

This project is released under the MIT Licence.
