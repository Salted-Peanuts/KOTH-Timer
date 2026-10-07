# KOTH Timer PCB files

This folder contains the PCB design history for KOTH Timer.

## Revision status

| Revision | Status | Use |
|---|---|---|
| `v0.1` | Manufactured; known TM1637 display reliability issues | Reference only |
| `v0.2` | Manufactured, assembled and tested with no known PCB faults | Current tested PCB |
| Next revision | In development | Will integrate newer v0.4 hardware changes |

## Important firmware v0.4.0 compatibility note

PCB v0.2 predates the current v0.4.0 battery-divider and buzzer changes.

It remains usable, but for firmware v0.4.0:

- fit **10 kΩ at R1 and R2** instead of the original 100 kΩ values;
- optionally connect a **3.3–5 V active buzzer** to **D13, 3.3 V and GND**;
- no buzzer is required for normal timer operation.

The original v0.2 KiCad/Gerber files are deliberately kept as the record of the tested board. They are not being edited in-place to imply that untested changes were part of that revision.

An updated PCB revision is currently being developed to incorporate the newer hardware changes directly.

## PCB v0.1

PCB v0.1 was the first manufactured revision. It can operate through the web UI, but some TM1637 displays showed interference, flicker, incorrect segments or unstable output.

It remains in the repository for reference and troubleshooting only.

## PCB v0.2

PCB v0.2 addressed the display problems seen on v0.1, including improvements to:

- local display decoupling;
- TM1637 signal routing;
- ground/power layout;
- display reliability;
- development breakout access.

The manufactured v0.2 board was assembled and tested successfully.

## Before ordering or assembling v0.2

Check:

- the current firmware release;
- the [v0.4 hardware upgrade notes](../docs/hardware-upgrade-v0.4.0.md);
- R1/R2 values (10 kΩ for v0.4);
- Arduino Nano ESP32 pin assignments;
- TM1637 display header pin order;
- battery/capacitor polarity;
- fuse and switch wiring;
- whether you want the optional external buzzer.

## PCBWay

The tested PCB v0.2 is published on PCBWay:

[Order / view PCB v0.2 on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

The PCBWay page reflects the older v0.2 hardware/release material. Use the current repository documentation when pairing it with firmware v0.4.0.
