# KOTH-Timer PCB v0.2.1

PCB v0.2.1 is a minor update based on the manufactured and tested v0.2 design.

## Changes from v0.2

- Corrected the battery-divider values in the schematic: **R1 = 10 kΩ** and **R2 = 10 kΩ**.
- Updated the board silkscreen and revision identification.
- No intended circuit-topology or copper-routing changes.
- Reorganised the KiCad schematic into clearly labelled functional blocks, making the power/battery ADC, team buttons, display connections, battery indicator and Nano ESP32/DEV pinout easier to follow. This is a documentation/layout clean-up rather than a circuit redesign.
- **November 2026 correction:** documented F1 as a **1 A fast-blow glass cartridge fuse**, 6.3 × 32 mm, with matching fuse clips. This replaces the old 2.5–3 A entry; no change to PCB copper or drills. User reports KiCad ERC/DRC passing with the revised files.

The 10 kΩ / 10 kΩ divider has been verified on existing hardware with firmware v0.4.0. The v0.2.1 fabrication files have not been separately manufactured or physically tested.

## Firmware compatibility

Use firmware **v0.4.0** with R1 and R2 populated with 10 kΩ resistors.

The optional active buzzer remains connected through the **DEV breakout** using D13, 3.3 V and GND. It is not integrated as a new on-board buzzer circuit.

## Build source and manufacturing files

KiCad 9 source files:

- `KOTH_v0.2.1.kicad_pro`
- `KOTH_v0.2.1.kicad_sch`
- `KOTH_v0.2.1.kicad_pcb`

Release documents/fabrication outputs:

- [KOTH_v0.2.1.pdf](KOTH_v0.2.1.pdf) — current schematic PDF
- [KOTH_v0.2.1_GERBER.zip](KOTH_v0.2.1_GERBER.zip) — four-layer Gerbers, drills and PCBWay auxiliary BOM/placement data
- [Builder BOM (CSV)](../../docs/BOM.csv) / [Builder BOM (Excel)](../../docs/BOM.xlsx) — community parts list and notes

**Four-layer stack:** F.Cu / In1.Cu / In2.Cu / B.Cu. The Gerber archive contains the Excellon plated/non-plated drills, front/back masks and silkscreens, board outline, and the four copper layers.

**Fuse and complete assembly parts:** F1 = 1 A fast-blow 6.3 × 32 mm glass fuse; R1/R2 = 10 kΩ; C2–C6 = five 100 nF; C7/C8 = two 100 µF; R10–R13 = four 100 Ω. The `PCBWay_bom.csv` included inside the Gerber archive is an automatically generated **PCB-only assembly export**, not the complete builder BOM. For sourcing all parts, use the linked builder BOM above. The optional external buzzer is not a PCB-mounted component.

Existing photographs show the manufactured v0.2 PCB, not separately fabricated v0.2.1. Do not represent those photos as a v0.2.1 manufacturing test.

**Before ordering:** verify the current fabrication package, fuse cartridge and holder compatibility, and parts values. Also check the chosen 1 A fast-acting fuse's suitability under normal and startup current; ERC/DRC cannot confirm fuse behaviour. The existing PCBWay project may still distribute the historical v0.2 design until updated.

## Hardware licence

Copyright (c) 2026 Salted.

The **PCB v0.2.1 hardware design files** are provided under the **CERN Open Hardware Licence Version 2 – Strongly Reciprocal (CERN-OHL-S-2.0)**. See [LICENSE](LICENSE) for the full licence text.

Source location: https://github.com/Salted-Peanuts/KOTH-Timer/tree/main/PCB/v0.2.1

This is a hardware-only licensing update. The existing firmware v0.4.0 remains MIT-licensed. The historical PCB v0.1 and v0.2 files retain their original published licences.
