> **Current hardware recommendation:** PCB v0.2.1 is now preferred for new builds. This folder preserves the original manufactured/tested v0.2 revision and its historical fabrication outputs. For firmware v0.4.0 on v0.2, fit 10 kΩ at both R1 and R2.

# KOTH Timer PCB v0.2

**Status: manufactured, assembled and tested.**

The following text describes this historical v0.2 release. PCB v0.2.1 is now recommended for new builds.

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

## Next PCB revision

The newer [PCB v0.2.1](../v0.2.1/) is now the **recommended source and fabrication revision** for new builds. It specifies 10 kΩ resistors for R1/R2 and updates the silkscreen, while retaining the v0.2 track/via routing. The optional buzzer still connects externally via the DEV header.

**PCB v0.2 remains the manufactured and field-tested reference.** v0.2.1 has not been separately manufactured. This folder is preserved unchanged as a historical hardware source package except for README guidance.

## PCBWay

[View the KOTH-Timer shared project on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html).

This directory preserves the actual manufactured v0.2 files as a historical reference. The **recommended PCB for new builds is [v0.2.1](../v0.2.1/)**. PCBWay's public downloads may be updated to v0.2.1; verify which revision is being supplied before ordering.
