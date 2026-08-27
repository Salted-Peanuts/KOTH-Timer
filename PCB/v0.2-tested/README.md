# KOTH Timer PCB v0.2 - Tested

This folder contains the second PCB revision for the KOTH Timer.

## Status

**Manufactured, assembled, and tested with no known faults.**

PCB v0.2 is the current recommended PCB revision for new builds.

## What Changed From v0.1

PCB v0.2 was redesigned after testing PCB v0.1.

The v0.1 board was functionally usable through the web UI, but some TM1637 displays showed reliability issues.

PCB v0.2 improves the design with better attention to:

- Display power decoupling
- TM1637 CLK/DIO routing
- Display signal reliability
- Ground and power layout
- Dev Pinout for easier future module development 
- PCB revision clarity

## Recommendation

Use this revision for new PCB-based KOTH Timer builds.

## Before Ordering

Even though PCB v0.2 has been tested successfully, builders should still check:

- Arduino Nano ESP32 pin assignments
- TM1637 display header pinout
- Battery/Capacitor polarity
- Fuse and switch wiring
- Display module pin order
- BOM compatibility
- Firmware version

## PCBWay Project Link

This tested PCB revision is also available as a PCBWay shared project:

[Order / view the KOTH Timer PCB v0.2 project on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

This link is useful for builders who want to order the tested PCB without manually uploading the Gerber files.

## Version

PCB version:

```text
v0.2
```
Status:

```text
Tested and working
```
Recommended for new builds:

```text
Yes
```
