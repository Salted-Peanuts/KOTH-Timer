# KOTH Timer PCB v0.2 — tested hardware revision

This folder contains the second manufactured PCB revision for KOTH Timer.

## Status

**Manufactured, assembled and tested successfully.**

PCB v0.2 remains the current tested PCB revision while the next PCB revision is being developed.

> [!IMPORTANT]
> PCB v0.2 was designed before the firmware v0.4.0 battery-divider and buzzer changes.
>
> For firmware v0.4.0, fit **10 kΩ at R1 and R2**, not the original 100 kΩ values shown in the historical v0.2 design files.

## Firmware v0.4.0 compatibility

PCB v0.2 can be used with firmware v0.4.0 with two considerations:

1. **Battery divider — required:** substitute 10 kΩ resistors at R1 and R2.
2. **Buzzer — optional:** connect a 3.3–5 V active buzzer externally using D13, 3.3 V and GND if audible feedback is wanted.

The timer firmware still works normally with **no buzzer connected**.

## Why the KiCad files still show 100 kΩ

The files in this directory are the historical record of the actual v0.2 board that was manufactured and tested.

They are intentionally not being rewritten to make the newer 10 kΩ divider look like part of the original tested revision. The required v0.4 substitution is documented here and in the current BOM/build guide instead.

## What changed from v0.1

v0.2 was redesigned after testing PCB v0.1, with improvements around:

- display power decoupling;
- TM1637 CLK/DIO routing;
- display signal reliability;
- ground and power layout;
- development breakout access;
- PCB revision clarity.

## Updated PCB

A newer PCB revision is currently being worked on. Its goal is to integrate the v0.4-era hardware changes more cleanly rather than requiring the v0.2 substitutions above.

Until that board is manufactured and tested, v0.2 remains the current **tested** PCB revision.

## PCBWay project

The v0.2 board is available as a PCBWay shared project:

[Order / view the KOTH Timer PCB v0.2 project on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

That page reflects the older v0.2 hardware/release material. Cross-check it against the current repository documentation before ordering parts for firmware v0.4.0.
