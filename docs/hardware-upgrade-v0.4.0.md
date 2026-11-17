# v0.4.0 hardware changes

This is for anyone upgrading an older KOTH Timer, assembling the manufactured PCB v0.2, or building the recommended PCB v0.2.1 for firmware v0.4.0.

## Battery divider

Change both battery-divider resistors:

```text
R1: 100 kΩ -> 10 kΩ
R2: 100 kΩ -> 10 kΩ
```

The wiring stays the same:

```text
Battery+
   |
  R1 10 kΩ
   |
   +---- A0
   |
  R2 10 kΩ
   |
  GND
```

During testing, the old 100 kΩ / 100 kΩ divider changed voltage noticeably once connected to the Nano ESP32 ADC input. The 10 kΩ / 10 kΩ divider was much more consistent.

v0.4.0 battery calibration was done with the 10 kΩ divider, so don't rely on the displayed voltage or automatic low-battery cutoff if the old resistor values are still fitted.

## Optional buzzer

v0.4.0 supports a 3-pin active buzzer:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

It is optional.

If D13 is left unconnected, the timer still works normally. You just won't get the scoring chirps, start countdown, victory sound or low-battery alarm.

PCB v0.2 exposes D13, 3.3 V and GND on the development breakout, so the buzzer can be added without modifying the main signal wiring.

## PCB v0.2

PCB v0.2 is the manufactured/tested baseline, while PCB v0.2.1 is recommended for new fabrication.

For v0.4.0:

```text
R1 = 10 kΩ
R2 = 10 kΩ
Buzzer = optional
```

The existing v0.2 KiCad/Gerber files are left as-is because they document the board that was actually manufactured and tested.

The recommended source/manufacturing revision **PCB v0.2.1** now specifies 10 kΩ for R1 and R2 in its schematic and BOM and has revised silkscreen. It retains the same circuitry and routing as manufactured/tested v0.2, but has not been separately manufactured.

The active buzzer **is not built onto v0.2.1**; attach it externally via the existing DEV breakout.

## PCB v0.2.1 fabrication notes

The manufacturing package is **four-layer**, including F.Cu, In1.Cu, In2.Cu and B.Cu, with Excellon drill files. For PCB v0.2.1, also fit C2–C6 (five 100 nF), C7/C8 (two 100 µF) and R10–R13 (four 100 Ω) in addition to the existing builder BOM entries. For the complete assembly-specific component information see the PCBWay BOM bundled in the Gerber ZIP.

## Battery protection

v0.4.0 enters the critical state below 3.40 V and clears it above 3.45 V.

A running game is frozen and saved when the battery goes critical. After a battery replacement or reboot, the saved game is restored paused and must be resumed by the referee.

Battery bar:

```text
5 bars  >= 4.05 V
4 bars  3.90–4.04 V
3 bars  3.75–3.89 V
2 bars  3.60–3.74 V
1 bar   3.45–3.59 V
<3.45 V flashing red bar
```

## Upgrade checklist

1. Replace R1 and R2 with 10 kΩ.
2. Check A0 is connected to the divider midpoint on the raw 1-cell battery rail.
3. Add the active buzzer to D13 if wanted.
4. Flash v0.4.0.
5. Compare the webpage battery voltage with a multimeter under load.
6. Test the battery bar and low-battery recovery before event use.
