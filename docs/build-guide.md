# KOTH Timer Build Guide

This guide explains how to build the prototype version of the KOTH Timer.

KOTH Timer is a DIY King of the Hill timer for Nerf, foam flinging, and other objective-based hobby games. It uses an Arduino Nano ESP32, two large team buttons, four TM1637 display modules, LED feedback, a battery display, and a local Wi-Fi web interface for controlling the game.

This guide is based on the current v0.2 prototype.

> **Important:** This is still an early public build. Check the schematic, BOM, firmware, and photos before soldering. Future versions may change the wiring or hardware.

---

## What You Are Building

The timer has two teams.

Each team has:

* One large arcade button
* One LED output for button feedback
* Two 4-digit TM1637 display modules showing that team’s remaining time

The timer also has:

* An Arduino Nano ESP32 controller
* A single 18650 battery power system
* A 5 V boost converter
* A rocker power switch
* A fuse
* A 5-segment battery indicator
* A small battery-check button
* A local Wi-Fi web interface hosted by the ESP32

The game logic is simple:

* If Team A holds their button, Team A’s timer counts down.
* If Team B holds their button, Team B’s timer counts down.
* If both buttons are held, the point is contested and neither timer counts down.
* If no button is held, neither timer counts down.
* The match ends when one team’s timer reaches zero.

---

## Current Prototype Status

This build is based on the working v0.2 prototype.

It has been used successfully at local Nerf events, but the documentation and firmware are still being improved.

Known future work:

* Improved v0.3 firmware
* Better web UI
* More stable KOTH interface
* Possible extra game modes later

---

## Files Used

Useful files in this repo:

```text
Firmware/
  v0.2_KOTH_timer.ino

docs/
  BOM.csv
  BOM.xlsx
  Wiring_schematic.pdf

Images/
  20260212_203113.jpg
  20260212_203205.jpg
  20260213_172516.jpg
```

Use the schematic and prototype photos as the final reference when wiring.

---

## Parts Required

The prototype BOM includes:

|       Qty | Part                           | Notes                            |
| --------: | ------------------------------ | -------------------------------- |
|         1 | Arduino Nano ESP32             | Arduino-branded Nano ESP32       |
|         1 | 18650 Li-ion cell              | Single-cell battery              |
|         1 | 18650 battery holder           | Holds the battery                |
|         1 | 5 V step-up converter          | Boosts the 18650 voltage to 5 V  |
|         1 | 2.5 A to 3 A fuse              | Use with a suitable fuse holder  |
|         1 | Rocker switch                  | Main power switch                |
|         2 | Large arcade buttons with LEDs | One for each team                |
|         4 | TM1637 4-digit display modules | Two displays per team            |
|         1 | 5-segment LED bar graph        | Battery level display            |
|         1 | Small push button              | Battery display button           |
|         2 | 100 kΩ resistors               | Battery voltage divider          |
|         7 | 220 Ω resistors                | LED current limiting             |
|         1 | 470 µF electrolytic capacitor  | Power smoothing                  |
|         1 | 100 nF ceramic capacitor       | Power smoothing/decoupling       |
| As needed | Prototyping PCB                | For soldering                    |
| As needed | Wire                           | 22 AWG was used in the prototype |
| As needed | Heat shrink                    | For insulation                   |
| As needed | Terminal blocks                | Optional, but useful             |
|         1 | Enclosure                      | 3D printed or custom housing     |

---

## Tools Required

You will need:

* Soldering iron
* Solder
* Wire cutters
* Wire strippers
* Heat shrink or electrical tape
* Small screwdrivers
* Multimeter
* USB cable for the Arduino Nano ESP32
* Computer with Arduino IDE installed

A multimeter is strongly recommended. Do not skip voltage checks.

---

## Power Wiring Overview

The prototype is powered from a single 18650 cell.

Basic power path:

```text
18650 battery
  -> fuse
  -> rocker power switch
  -> 5 V boost converter
  -> Arduino Nano ESP32 VIN / 5 V rail
```

All grounds must be connected together.

```text
Battery GND
Boost converter GND
Arduino GND
Display GND
Button LED GND
Battery indicator GND
```

> **Important:** Do not connect the 18650 battery directly to the Arduino 5 V or VIN pin without the boost converter/power circuit.

> **Li-ion safety:** Use care when working with 18650 cells. Avoid shorts, use a fuse, and do not use damaged cells.

---

## Arduino Nano ESP32 Pinout

The v0.2 firmware uses the following pin assignments.

### Team Buttons

The team buttons are wired to ground and use the ESP32 internal pull-up resistors.

| Function               | Arduino Pin | Wiring                    |
| ---------------------- | ----------- | ------------------------- |
| Team A button          | D2          | Button between D2 and GND |
| Team B button          | D3          | Button between D3 and GND |
| Battery display button | D4          | Button between D4 and GND |

Button logic:

```text
Pressed = LOW
Released = HIGH
```

---

### Arcade Button LEDs

The arcade button LEDs are driven from GPIO pins.

| Function          | Arduino Pin | Wiring                       |
| ----------------- | ----------- | ---------------------------- |
| Team A button LED | D5          | D5 -> resistor -> LED -> GND |
| Team B button LED | D7          | D7 -> resistor -> LED -> GND |

LED logic:

```text
HIGH = LED on
LOW = LED off
```

Use suitable resistors for your LEDs. The prototype BOM lists 220 Ω resistors.

---

### TM1637 Displays

The prototype uses four TM1637 4-digit displays.

All four displays share the same clock pin, but each display has its own data pin.

| Display          | Clock Pin | Data Pin |
| ---------------- | --------- | -------- |
| Team A display 1 | D8        | D9       |
| Team A display 2 | D8        | D10      |
| Team B display 1 | D8        | D11      |
| Team B display 2 | D8        | D6       |

Typical TM1637 wiring:

```text
TM1637 VCC -> 5 V / VIN rail
TM1637 GND -> GND
TM1637 CLK -> D8
TM1637 DIO -> display data pin
```

The two displays for the same team show the same time. This lets the timer be visible from more than one angle.

---

### Battery Voltage Reading

The battery voltage is measured through a resistor divider.

| Function                         | Arduino Pin |
| -------------------------------- | ----------- |
| Battery voltage divider midpoint | A0          |

The prototype uses:

```text
R1 = 100 kΩ
R2 = 100 kΩ
```

Basic divider:

```text
Battery +
  -> R1
  -> A0
  -> R2
  -> GND
```

> **Important:** Do not connect the battery directly to A0. The battery voltage must go through the resistor divider.

The firmware includes battery calibration values. If the displayed battery voltage is wrong, measure the battery with a multimeter and adjust the calibration values in the firmware.

---

### 5-Segment Battery Indicator

The battery indicator uses five GPIO outputs.

| Segment         | Arduino Pin |
| --------------- | ----------- |
| Red segment     | A1          |
| Yellow segment  | A2          |
| Green segment 1 | A3          |
| Green segment 2 | A4          |
| Green segment 3 | A5          |

The prototype firmware assumes:

```text
GPIO HIGH = segment on
GPIO LOW = segment off
```

Each LED segment should have current limiting.

The battery bar display turns on temporarily when the battery display button is pressed.

---

## Wiring Checklist

Before soldering everything permanently, build and test in sections.

### 1. Power System

Check:

* Fuse is installed
* Rocker switch controls power
* Boost converter outputs 5 V
* Arduino powers up from the 5 V supply
* Ground is common everywhere

Use a multimeter before plugging in the Arduino.

---

### 2. Buttons

Wire:

```text
Team A button -> D2 and GND
Team B button -> D3 and GND
Battery button -> D4 and GND
```

No external pull-up resistor is needed for these buttons because the firmware uses `INPUT_PULLUP`.

---

### 3. Arcade Button LEDs

Wire:

```text
D5 -> resistor -> Team A LED -> GND
D7 -> resistor -> Team B LED -> GND
```

If your arcade buttons have built-in LEDs, check their voltage and polarity before wiring.

---

### 4. Displays

Wire all TM1637 modules to power and ground.

Then connect:

```text
All display CLK pins -> D8

Team A display 1 DIO -> D9
Team A display 2 DIO -> D10
Team B display 1 DIO -> D11
Team B display 2 DIO -> D6
```

If one display does not work, swap it with a known working one to check whether the issue is the display module or the wiring.

---

### 5. Battery Indicator

Wire the 5-segment bar graph using the schematic.

Firmware pin order:

```text
A1 = red
A2 = yellow
A3 = green 1
A4 = green 2
A5 = green 3
```

Pressing the battery display button should show the battery level for a few seconds.

---

## Firmware Upload

The firmware is written for the Arduino Nano ESP32 using the Arduino IDE.

### 1. Install Arduino IDE

Install Arduino IDE 2.x.

### 2. Install ESP32 board support

In Arduino IDE, install the board support needed for the Arduino Nano ESP32.

Select the Arduino Nano ESP32 board before compiling.

### 3. Install Required Libraries

The firmware uses:

* Wi-Fi support for the ESP32
* Web server support
* WebSockets support
* TM1637 display support
* Preferences storage

If the code fails to compile because a library is missing, install the missing library through:

```text
Arduino IDE -> Library Manager
```

Search for the missing library name shown in the compile error.

### 4. Open the Firmware

Open the `.ino` file from the `Firmware` folder.

For Arduino IDE compatibility, the sketch may need to be inside a folder with the same name as the `.ino` file.

Example:

```text
Firmware/
  KOTH_Timer_v0_2/
    KOTH_Timer_v0_2.ino
```

### 5. Upload

Connect the Arduino Nano ESP32 over USB.

Then click:

```text
Upload
```

After uploading, the ESP32 should start its local Wi-Fi access point.

---

## First Power-On Test

Before closing the enclosure, test everything on the bench.

### Check displays

On boot, the TM1637 displays should show the starting game time.

If the displays are blank:

* Check 5 V and GND
* Check D8 clock wiring
* Check each display’s DIO pin
* Check display orientation
* Check the firmware uploaded correctly

---

### Check Wi-Fi

The ESP32 creates a Wi-Fi network:

```text
KOTH-Timer
```

Connect to it with a phone or laptop.

There is no internet through this network. That is normal.

Open a browser and try:

```text
http://192.168.4.1
```

The KOTH Timer web interface should load.

---

### Check buttons

Start a test game from the web interface.

Then test:

* Hold Team A button: Team A timer should count down.
* Hold Team B button: Team B timer should count down.
* Hold both buttons: contested state, no timer should count down.
* Hold no buttons: idle state, no timer should count down.

---

### Check LED feedback

During a running game:

* Team A capturing should light Team A LED.
* Team B capturing should light Team B LED.
* Contested should light both LEDs.
* Idle should turn both LEDs off.

---

### Check battery display

Press the battery display button.

The 5-segment bar graph should light briefly to show the approximate battery level.

If the displayed battery level is wrong, measure the battery voltage with a multimeter and calibrate the firmware.

---

## Web Interface Use

The web interface allows the referee/admin to:

* Start the game
* Pause the game
* Resume the game
* Reset the game
* Set match duration
* Set team colours
* Identify teams
* View battery voltage
* View current game state
* See the winner when the match ends

The reset button is only intended to work when the game is paused or when the match has finished.

---

## Game Rules

The current firmware uses hold-to-capture logic.

| Button State         | Result                          |
| -------------------- | ------------------------------- |
| Team A held only     | Team A timer counts down        |
| Team B held only     | Team B timer counts down        |
| Both buttons held    | Contested, no timer counts down |
| No buttons held      | Idle, no timer counts down      |
| A timer reaches zero | Match ends                      |

This makes the timer useful for King of the Hill games where teams must physically hold the objective.

---

## Mounting Everything

The prototype uses a custom enclosure, but you can use any suitable housing.

When laying out the enclosure, consider:

* Buttons should be large and easy to hit during games.
* Displays should be visible from a distance.
* The power switch should be protected from accidental bumps.
* The USB port should still be accessible if possible.
* The battery should be secure and not able to rattle around.
* Wires should be strain-relieved.
* Solder joints should be insulated.
* The fuse should be accessible.

Do not leave bare battery connections exposed.

---

## Final Pre-Game Checklist

Before using the timer at an event:

* Battery fully charged
* Fuse installed
* Power switch working
* Displays visible
* Both team buttons working
* Both team LEDs working
* Battery indicator working
* Phone/laptop can connect to `KOTH-Timer`
* Web interface loads
* Start/pause/resume/reset tested
* Match duration set correctly
* Enclosure closed and secure

---

## Troubleshooting

### The ESP32 does not turn on

Check:

* Battery voltage
* Fuse
* Rocker switch
* Boost converter output
* 5 V connection to the Arduino
* Ground wiring

---

### The Wi-Fi network does not appear

Check:

* Firmware uploaded correctly
* Correct board selected in Arduino IDE
* Arduino is actually powered
* Try pressing reset on the board
* Try powering from USB first

---

### The web interface does not load

Check:

* Your phone/laptop is connected to `KOTH-Timer`
* Mobile data is not interfering
* Try opening `http://192.168.4.1`
* Try another browser
* Restart the timer

---

### A team button does not work

Check:

* One side of the button goes to the correct GPIO
* The other side goes to GND
* You used D2 for Team A and D3 for Team B
* The button is normally open
* The wire has not broken off the terminal

---

### A button works backwards

The firmware expects the buttons to connect the pin to GND when pressed.

Use the normally open contacts on the arcade button.

---

### A display is blank

Check:

* VCC
* GND
* CLK wire to D8
* DIO wire to the correct pin
* Display module orientation
* Solder joints
* Try swapping with a known working display

---

### Both displays for one team are wrong

Check the data pins for that team:

```text
Team A display 1 -> D9
Team A display 2 -> D10

Team B display 1 -> D11
Team B display 2 -> D6
```

---

### Battery reading is wrong

Check:

* The resistor divider is wired correctly
* R1 and R2 are both 100 kΩ
* The divider midpoint goes to A0
* Battery ground and Arduino ground are common
* The firmware calibration value matches your real measured battery voltage

Use a multimeter to compare the real battery voltage against the web interface reading.

---

### Battery bar graph does not light

Check:

* Battery display button wiring
* Bar graph common pin wiring
* Segment polarity
* Current-limiting resistors
* Pins A1 to A5
* Ground connection

---

### Timer does not count down

Check:

* Game has been started from the web interface
* Game is not paused
* Only one team button is being held
* The button wiring is correct
* The web interface state is not showing contested or idle

---

## Notes for Builders

This is not a polished commercial product. It is a hobby project based on a working prototype.

Expect to do some testing and troubleshooting.

You can modify:

* Team colours
* Enclosure design
* Button style
* Display colours
* Battery setup
* Match rules
* Firmware behaviour

If you improve the design, consider sharing your changes back with the project.

---

## Safety Notes

* Be careful with Li-ion batteries.
* Use a fuse.
* Do not short the battery.
* Insulate exposed solder joints.
* Do not leave loose wires inside the enclosure.
* Check polarity before powering the device.
* Do not use damaged cells.
* Do not charge the battery unattended unless your charging setup is designed for safe unattended use.

---

## Version

Guide written for:

```text
KOTH Timer v0.2 prototype
```

Future firmware or hardware versions may use different wiring.
