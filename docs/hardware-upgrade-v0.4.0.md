# KOTH Timer v0.4.0 hardware upgrade notes

Firmware v0.4.0 introduces a substantial battery-monitoring update and optional audible feedback.

This page is specifically for builders upgrading an existing v0.2/v0.3-era timer or assembling the existing PCB v0.2 for the new firmware.

## Required change: battery-divider resistors

Earlier documentation and PCB v0.2 use:

```text
R1 = 100 kΩ
R2 = 100 kΩ
```

For firmware v0.4.0, use:

```text
R1 = 10 kΩ
R2 = 10 kΩ
```

The divider remains a 1:1 divider; only the resistor values change.

```text
raw 1-cell battery rail
        |
       R1 10 kΩ
        |
        +---- A0
        |
       R2 10 kΩ
        |
       GND
```

### Why this changed

The original 100 kΩ / 100 kΩ divider has a relatively high source impedance. During testing, the divider midpoint measured correctly by itself but shifted significantly once connected to the Arduino Nano ESP32 ADC input.

Dropping both resistors to 10 kΩ makes the divider much less sensitive to ADC/input loading and produced substantially more repeatable readings.

The v0.4.0 firmware calibration was measured using the ESP32's own calibrated ADC millivolt reading and an under-load battery measurement.

### Do not mix the old divider with the new calibration

The v0.4 battery voltage display, battery bar thresholds and automatic low-battery protection are calibrated around the 10 kΩ / 10 kΩ divider.

If R1/R2 are still 100 kΩ, do not assume the displayed voltage or critical-battery behaviour is accurate.

## Optional change: active buzzer on D13

Firmware v0.4.0 supports a 3-pin **active buzzer module**.

Recommended wiring:

```text
Buzzer VCC -> 3.3 V
Buzzer GND -> GND
Buzzer SIG -> D13
```

The tested module type operates from 3.3–5 V and is driven as a simple digital active buzzer.

### The buzzer is optional

You do **not** need to fit a buzzer to use v0.4.0.

Leaving D13 unconnected is safe for this firmware. The timer, displays, buttons, webpage, battery monitoring and game logic continue to work normally. You simply lose the audible:

- scoring chirps;
- manual-start countdown;
- victory pattern;
- critical low-battery alarm.

PCB v0.2 exposes D13/3.3 V/GND through its development connections, so the buzzer can be added externally.

## PCB v0.2 status

PCB v0.2 remains the current tested PCB revision.

For firmware v0.4.0:

- substitute 10 kΩ at R1/R2;
- optionally wire the buzzer externally;
- otherwise use the existing v0.2 connections as documented.

The historical v0.2 KiCad/Gerber files remain unchanged so that the repository accurately records the board that was actually manufactured and tested.

## Updated PCB in development

An updated PCB revision is currently being worked on to integrate the newer hardware changes directly.

It should not be treated as the recommended PCB until it has been manufactured and tested. Until then, PCB v0.2 with the documented v0.4 substitutions remains the known tested option.

## Battery protection behaviour in v0.4.0

The current firmware uses:

```text
Critical entry: below 3.40 V
Critical clear: above 3.45 V
```

A running match entering the critical state is paused and saved to non-volatile memory.

After a battery replacement/reboot, the saved match is restored **paused**. The referee must explicitly press Resume when players are ready.

The five-segment battery bar uses:

```text
5 bars: >= 4.05 V
4 bars: 3.90 V to < 4.05 V
3 bars: 3.75 V to < 3.90 V
2 bars: 3.60 V to < 3.75 V
1 bar : 3.45 V to < 3.60 V
<3.45 V: flashing red/final segment
```

## Upgrade checklist

Before flashing v0.4.0 onto an existing timer:

- replace R1/R2 with 10 kΩ;
- verify A0 is connected to the battery divider midpoint;
- verify the divider is measuring the raw single-cell battery rail, not the boosted 5 V rail;
- optionally fit the active buzzer to D13/3.3 V/GND;
- flash the v0.4.0 firmware;
- compare the webpage battery voltage with a multimeter under load;
- test the battery bar;
- test the critical-battery freeze/recovery sequence before event use.
