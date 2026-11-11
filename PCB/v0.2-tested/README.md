# KOTH Timer PCB v0.2

**Status: manufactured, assembled and tested.**

v0.2 is still the recommended PCB while the next revision is being developed.

## Using v0.2 with firmware v0.4.0

The board was designed before the v0.4 battery and buzzer changes.

For v0.4.0:

```text
R1 = 10 kΩ
R2 = 10 kΩ
```

The original design files show 100 kΩ at R1/R2. Use 10 kΩ when assembling the board for v0.4.0.

The buzzer is optional. If wanted, connect a 3.3–5 V active buzzer to:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

The firmware works normally with no buzzer fitted.

## Why the KiCad files still show 100 kΩ

Those are the files for the actual v0.2 board that was manufactured and tested, so they are being kept unchanged.

The v0.4 resistor substitution is documented here, in the current BOM and in the build guide.

## Changes from v0.1

v0.2 improved:

- display power decoupling
- TM1637 CLK/DIO routing
- ground and power layout
- display reliability
- development breakout access

## Next PCB

A new revision is being worked on with the v0.4 hardware changes included on the board.

Until it has been manufactured and tested, v0.2 remains the known-good PCB.

## PCBWay

[View KOTH Timer PCB v0.2 on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

That page shows the original v0.2 hardware, so use the current repo BOM when building for firmware v0.4.0.
