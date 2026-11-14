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

The KiCad source project (schematic and board layout), schematic PDF, and fabrication Gerbers/drill files belong in this directory.

**Before ordering:** Verify that the revision-labelled Gerbers, editable KiCad sources and BOM agree on R1/R2 = 10 kΩ. Until those files have been uploaded and checked, use the existing documented v0.2 build instructions.

## Hardware licence

Copyright (c) 2026 Salted.

The **PCB v0.2.1 hardware design files** are provided under the **CERN Open Hardware Licence Version 2 – Strongly Reciprocal (CERN-OHL-S-2.0)**. See [LICENSE](LICENSE) for the full licence text.

Source location: https://github.com/Salted-Peanuts/KOTH-Timer/tree/main/PCB/v0.2.1

This is a hardware-only licensing update. The existing firmware v0.4.0 remains MIT-licensed. The historical PCB v0.1 and v0.2 files retain their original published licences.
