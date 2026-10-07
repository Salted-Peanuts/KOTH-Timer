# KOTH Timer Build Guide

This guide covers the current standalone KOTH Timer build for **firmware v0.4.0**.

KOTH Timer is a two-team King of the Hill timer built around an Arduino Nano ESP32, two large arcade buttons, four TM1637 displays, battery monitoring, and a local referee/admin webpage.

The timer does not require internet access during gameplay.

> [!IMPORTANT]
> Firmware v0.4.0 uses a **10 kΩ / 10 kΩ battery divider**.
>
> Earlier documentation and PCB v0.2 show **100 kΩ / 100 kΩ** at R1/R2. If you are using v0.4.0, fit **10 kΩ at R1 and R2** before relying on battery voltage or low-battery protection.
>
> The active buzzer is **optional**. The firmware still works normally with no buzzer connected.

For an existing build, also read [v0.4.0 hardware upgrade notes](hardware-upgrade-v0.4.0.md).

## Build-guide scope

There are currently two practical ways to build the timer:

1. **Prototype / hand-wired build** using the pinout in this guide.
2. **PCB v0.2 build** using the tested PCB files, with the v0.4 R1/R2 substitution and optional external buzzer.

PCB v0.2 is the current tested PCB revision.

An **updated PCB revision is currently being developed** to integrate the newer v0.4 hardware changes more cleanly. It is not yet the recommended board because it has not yet replaced the tested v0.2 hardware.

## Current recommended versions

```text
Firmware: v0.4.0
PCB:      v0.2-tested
```

Firmware path:

```text
Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino
```

PCB path:

```text
PCB/v0.2-tested/
```

The tested PCB v0.2 is also available as a PCBWay shared project:

[Order / view PCB v0.2 on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

That page reflects the older v0.2 hardware/release material. When using firmware v0.4.0, follow this repository's current BOM and upgrade notes.

## What you are building

Each team has:

- one large arcade button;
- one button LED;
- two TM1637 4-digit displays showing that team's remaining time.

The timer also has:

- Arduino Nano ESP32;
- single 18650 battery;
- 5 V boost converter;
- fuse;
- main power switch;
- five-segment battery indicator;
- battery-check button;
- local Wi-Fi referee/admin page;
- optional active buzzer.

## Game behaviour

During a running match:

- only Team A held -> Team A counts down;
- only Team B held -> Team B counts down;
- both held -> contested, no scoring;
- neither held -> no scoring;
- a team wins when its timer reaches zero.

The match can be started from the webpage or physically by holding both team buttons for 5 seconds.

## Parts required

Use the reviewed BOM as the primary parts reference:

```text
docs/BOM.xlsx
docs/BOM.csv
```

Main parts:

| Qty | Part | Notes |
|---:|---|---|
| 1 | Arduino Nano ESP32 | Arduino-branded Nano ESP32 / NORA-W106 |
| 1 | 18650 Li-ion cell | Single-cell battery |
| 1 | 18650 holder | Match your cell style |
| 1 | 5 V boost converter | Powers the 5 V timer rail |
| 1 | 2.5–3 A fuse + holder | Current BOM value; use suitably rated wiring |
| 1 | Main rocker switch | Device power |
| 2 | Large arcade buttons | One per team |
| 2 | Arcade button LEDs | Often integrated into the buttons |
| 4 | TM1637 4-digit displays | Two per team |
| 1 | Five-segment LED bar | Battery indicator |
| 1 | Battery-check push button | Momentary |
| 2 | **10 kΩ resistors** | R1/R2 battery divider |
| 7 | 220 Ω resistors | LED current limiting |
| 1 | 470 µF electrolytic capacitor | Power smoothing |
| 1 | 100 nF ceramic capacitor | Decoupling |
| 1 | 3.3–5 V active buzzer | **Optional** |
| as needed | Wire / heat shrink / terminals | Build dependent |
| 1 | Enclosure | 3D printed or custom |

### BOM correction from older versions

The old BOM linked the 220 Ω resistor row to Jaycar part **RR0628**, which is actually **220 kΩ**.

The corrected 220 Ω example part is **RR0556**.

The current BOM files have been corrected.

## Tools

Recommended:

- soldering iron and solder;
- wire cutters/strippers;
- small screwdrivers;
- multimeter;
- USB cable for the Arduino Nano ESP32;
- computer with Arduino IDE 2.x.

A multimeter is strongly recommended, especially for the battery-divider and boost-converter checks.

## Power system

The timer uses a single 18650 cell and a 5 V boost converter.

Typical power path:

```text
18650
  -> fuse
  -> main switch
  -> 5 V boost converter
  -> 5 V timer rail
```

All grounds must be common.

Do not connect a raw 18650 cell directly to a 5 V rail.

### Battery-sense rail

The A0 divider must measure the **raw single-cell battery voltage**, not the boosted 5 V output.

The tested build measures from the protected battery side of the power system. Follow the current schematic/layout for your build and verify the measured point with a multimeter.

## Arduino Nano ESP32 pinout

### Team buttons

The buttons switch to GND and use the ESP32 internal pull-ups.

| Function | Pin | Wiring |
|---|---|---|
| Team A button | D2 | D2 -> button -> GND |
| Team B button | D3 | D3 -> button -> GND |
| Battery display button | D4 | D4 -> button -> GND |

Logic:

```text
Pressed  = LOW
Released = HIGH
```

### Arcade button LEDs

| Function | Pin |
|---|---|
| Team A LED | D5 |
| Team B LED | D7 |

Typical wiring:

```text
GPIO -> 220 Ω -> LED -> GND
```

Check the voltage/current requirements of the exact arcade-button LED you use.

### TM1637 displays

All four displays share one clock line and have separate data lines.

| Display | CLK | DIO |
|---|---|---|
| Team A display 1 | D8 | D9 |
| Team A display 2 | D8 | D10 |
| Team B display 1 | D8 | D11 |
| Team B display 2 | D8 | D6 |

Typical module wiring:

```text
VCC -> 5 V rail
GND -> GND
CLK -> D8
DIO -> assigned data pin
```

D6 is intentionally used for Team B display 2. D12 was avoided after reliability problems during development.

### Battery voltage divider

Firmware v0.4.0 uses:

```text
R1 = 10 kΩ
R2 = 10 kΩ
```

Wiring:

```text
raw battery +
     |
   R1 10 kΩ
     |
     +---- A0
     |
   R2 10 kΩ
     |
    GND
```

Do **not** connect the battery directly to A0.

The previous 100 kΩ / 100 kΩ divider was replaced because its high source impedance allowed the ADC input/loading to shift the midpoint enough to make battery readings unreliable.

### Five-segment battery indicator

| Segment | Pin |
|---|---|
| Red | A1 |
| Yellow | A2 |
| Green 1 | A3 |
| Green 2 | A4 |
| Green 3 | A5 |

The current firmware assumes each segment is active-high.

Use current-limiting resistors as shown by the BOM/schematic.

### Optional active buzzer

Firmware v0.4.0 supports an active 3-pin buzzer:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

The tested type is a simple 3.3–5 V active buzzer module.

The buzzer is **not required**.

If D13 is left unconnected:

- the game still runs normally;
- the webpage still works;
- displays/buttons/LEDs still work;
- battery protection still works;
- only the audible feedback is missing.

When fitted, the buzzer provides:

- scoring chirps;
- manual-start countdown;
- victory sound;
- critical-battery warning.

## PCB v0.2 builds

PCB v0.2 remains the current tested PCB.

For firmware v0.4.0:

1. fit **10 kΩ** at R1;
2. fit **10 kΩ** at R2;
3. optionally connect the active buzzer externally using D13/3.3 V/GND.

The historical v0.2 KiCad and Gerber files are intentionally left unchanged so they accurately represent the board that was manufactured and tested.

Do not assume the old 100 kΩ values in those files are the current v0.4 recommendation.

## Wiring checklist

Before first power-up:

- confirm the boost converter output is 5 V;
- confirm battery polarity;
- confirm capacitor polarity;
- confirm all grounds are common;
- confirm R1/R2 are 10 kΩ for v0.4;
- confirm A0 is connected only to the divider midpoint;
- confirm TM1637 CLK/DIO wiring;
- confirm LED resistor values;
- confirm fuse and power wiring;
- if fitted, confirm buzzer VCC/GND/SIG orientation.

## Firmware upload

### 1. Install Arduino IDE

Use Arduino IDE 2.x.

### 2. Select the correct board

Select:

```text
Arduino Nano ESP32
```

The code uses Arduino Nano pin labels such as D2, D8 and D13.

### 3. Required libraries

The firmware uses:

- WiFi;
- WebServer;
- Preferences;
- WebSocketsServer;
- TM1637Display.

WiFi, WebServer and Preferences are provided by the Arduino Nano ESP32 board package.

Install the external WebSockets and TM1637Display libraries through Arduino Library Manager if they are not already installed.

The v0.4.0 firmware no longer uses DNSServer because captive-portal/automatic webpage launching was removed.

### 4. Open the firmware

Use:

```text
Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino
```

Arduino IDE expects the sketch folder and `.ino` file to share the same base name.

### 5. Upload

Connect the Nano ESP32 over USB and upload the sketch.

After boot, the device creates its own Wi-Fi network.

## Referee/admin webpage

Connect to:

```text
SSID: KOTH-Timer
Password: none / open network
```

Then manually open:

```text
http://10.10.10.1/
```

Connection test:

```text
http://10.10.10.1/ping
```

### Why the page does not open automatically

Development builds experimented with captive-portal/automatic browser launching.

Behaviour varied between Windows, Android and iOS, so that code was removed for v0.4.0.

The supported connection method is deliberately simple:

```text
join KOTH-Timer Wi-Fi
        ↓
open http://10.10.10.1/
```

If the phone/laptop says the Wi-Fi has no internet, choose the option to remain connected.

## Optional Wi-Fi password

The AP is open by default.

In the firmware:

```cpp
static const char* AP_PASS = "";
```

To use a password, set a WPA-compatible password of at least eight characters and re-upload the firmware.

## First power-on test

### Displays

At boot, all four TM1637 modules should show the configured starting time.

If a display is blank or incorrect, check:

- 5 V and GND;
- D8 shared CLK;
- the display's individual DIO pin;
- display-module pin order;
- cable/connectors.

### Buttons

Before gameplay, verify:

- Team A button changes state correctly;
- Team B button changes state correctly;
- both held is detected as contested;
- battery button activates the battery bar.

### LEDs

During a running game:

- Team A capture -> Team A LED on;
- Team B capture -> Team B LED on;
- contested -> both LEDs on;
- idle -> both off.

### Manual physical start

With the game stopped:

1. hold both team buttons;
2. keep holding for 5 seconds;
3. the match start sequence begins.

With a buzzer fitted, the sound is:

```text
short beep
pause
short beep
pause
LONG beep = GO
```

The match goes live at the start of the long beep.

Without a buzzer, the same physical hold still starts the match; it is simply silent.

### Battery reading

Open the webpage and compare its voltage with a multimeter measured at the battery **under load**.

The current calibration was established from two measured points using the ESP32's own ADC reading.

If your hardware differs materially, recalibrate rather than blindly changing the low-battery threshold.

### Battery bar

Current thresholds:

```text
5 bars: >= 4.05 V
4 bars: 3.90 V to < 4.05 V
3 bars: 3.75 V to < 3.90 V
2 bars: 3.60 V to < 3.75 V
1 bar : 3.45 V to < 3.60 V
<3.45 V: flashing red/final bar
```

## Critical low-battery behaviour

The firmware enters the critical state below:

```text
3.40 V
```

It clears only above:

```text
3.45 V
```

This hysteresis prevents rapid on/off switching around the threshold.

During critical battery:

- all four displays flash `LO:LO`;
- the optional buzzer repeats a low-battery alarm;
- a running match is frozen;
- game state is saved to NVS.

The protection uses repeated instantaneous averaged readings for the critical decision rather than waiting for the slower UI smoothing filter.

## Battery-swap recovery test

This is an important v0.4.0 test.

1. Start a game.
2. Let both teams accumulate some state.
3. Reduce/replace the battery so the timer enters critical low battery.
4. Confirm gameplay freezes.
5. Power the timer off.
6. Replace the battery.
7. Power it back on.
8. Confirm the saved scores return.
9. Confirm the game is **paused**.
10. Press Resume from the admin page when ready.

The firmware deliberately never auto-resumes after a battery swap.

## Winner behaviour

At game end:

- the winning team's displays run a spinner animation;
- then blink `00:00`;
- the animation repeats until reset;
- the losing team remains steady on its final remaining time;
- a draw animates both teams.

With a buzzer fitted, the winner also gets the victory sound.

## Referee time correction

The admin webpage can add or subtract time from either team during gameplay.

This is intended for referee corrections without resetting the match.

## Final pre-game checklist

Before an event:

- fully charge/test the 18650;
- verify battery voltage against a multimeter;
- confirm R1/R2 are 10 kΩ;
- confirm all four displays;
- confirm both team buttons;
- confirm both button LEDs;
- confirm battery button/bar;
- test the physical 5-second start;
- if fitted, test all buzzer sounds;
- verify `http://10.10.10.1/`;
- test Pause/Resume/Reset;
- test referee time adjustment;
- test one complete match to a winner;
- test low-battery freeze/recovery after any hardware/firmware change.

## Troubleshooting

### Wi-Fi appears but the webpage does not load

Manually enter:

```text
http://10.10.10.1/
```

Then try:

```text
http://10.10.10.1/ping
```

Do not wait for an automatic captive-portal popup; v0.4.0 intentionally does not use one.

### Battery voltage is wrong

Check in this order:

1. R1 and R2 are both 10 kΩ.
2. A0 is connected to the divider midpoint.
3. The divider measures the raw single-cell battery rail, not 5 V.
4. Measure actual battery voltage under load.
5. Open `http://10.10.10.1/state` and compare `ba0` / `bv` with your meter.

The firmware calibration should be changed only after confirming the hardware points above.

### Battery warning appears too early/late

Do not change the 3.40 V threshold first.

First verify that the webpage battery voltage agrees with a multimeter under load. A calibration/wiring problem should be corrected at the measurement stage.

### Buzzer does not sound

The buzzer is optional.

If fitted, check:

- it is an **active** buzzer module;
- VCC is on 3.3 V;
- GND is common;
- SIG is on D13;
- module polarity/pin order.

A missing buzzer does not indicate a firmware fault if the rest of the timer operates correctly.

### One TM1637 display is unstable

Check the exact display module, power/ground, cable length and DIO connection.

PCB v0.1 had known display reliability problems. PCB v0.2 is the recommended tested board.

## Safety

This is a hobby project.

Use appropriate care with Li-ion cells:

- do not use damaged cells;
- avoid shorts;
- use a fuse;
- insulate exposed conductors;
- verify polarity before power-up;
- use wiring/connectors appropriate for the expected current.

## Version

This guide is aligned to:

```text
KOTH Timer firmware v0.4.0
PCB v0.2-tested (with R1/R2 = 10 kΩ for v0.4)
Optional active buzzer on D13
```

An updated PCB revision is currently being developed.
