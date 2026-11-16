# KOTH-Timer PCB v0.2.1

PCB v0.2.1 is a minor update based on the manufactured and tested v0.2 design.

## Changes from v0.2

- Corrected the battery-divider values in the schematic: **R1 = 10 kΩ** and **R2 = 10 kΩ**.
- Updated the board silkscreen and revision identification.
- No intended circuit-topology or copper-routing changes.

The 10 kΩ / 10 kΩ divider has been verified on existing hardware with firmware v0.4.0. The v0.2.1 fabrication files have not been separately manufactured or physically tested.

## Firmware compatibility

Use firmware **v0.4.0** with R1 and R2 populated with 10 kΩ resistors.

The optional active buzzer remains connected through the **DEV breakout** using D13, 3.3 V and GND. It is not integrated as a new on-board buzzer circuit.

## Build source and manufacturing files

KiCad 9 source files:

- `KOTH_v0.2.1.kicad_pro`
- `KOTH_v0.2.1.kicad_sch`
- `KOTH_v0.2.1.kicad_pcb`

Release documents/fabrication outputs: `KOTH_v0.2.1.pdf` and `KOTH_v0.2.1_GERBER.zip` (Gerbers, drills, and PCBWay auxiliary BOM/placement data).

**Four-layer stack:** F.Cu / In1.Cu / In2.Cu / B.Cu. The Gerber archive contains the Excellon plated/non-plated drills, front/back masks and silkscreens, board outline, and the four copper layers.

**Complete assembly parts:** R1/R2 = 10 kΩ; C2–C6 = five 100 nF; C7/C8 = two 100 µF; R10–R13 = four 100 Ω. Check `PCBWay_bom.csv` inside the Gerber archive. The optional external buzzer is not a PCB-mounted component.

Existing photographs show the manufactured v0.2 PCB, not separately fabricated v0.2.1. Do not represent those photos as a v0.2.1 manufacturing test.

**Before ordering:** verify the latest fabrication package, four-layer board stack and parts values. The existing PCBWay project may still distribute the historical v0.2 design until updated.

## Hardware licence

Copyright (c) 2026 Salted.

The **PCB v0.2.1 hardware design files** are provided under the **CERN Open Hardware Licence Version 2 – Strongly Reciprocal (CERN-OHL-S-2.0)**. See [LICENSE](LICENSE) for the full licence text.

Source location: https://github.com/Salted-Peanuts/KOTH-Timer/tree/main/PCB/v0.2.1

This is a hardware-only licensing update. The existing firmware v0.4.0 remains MIT-licensed. The historical PCB v0.1 and v0.2 files retain their original published licences.
