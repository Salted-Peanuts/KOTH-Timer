# KOTH Timer PCB files

## Revisions

| Revision | Status |
|---|---|
| v0.1 | First manufactured board. Known TM1637 display problems. Reference only. |
| v0.2 | Manufactured, assembled and tested. Current recommended PCB. |
| Next revision | In development. Will include the v0.4 hardware changes. |

## PCB v0.2 with firmware v0.4.0

PCB v0.2 predates the new battery-divider values and buzzer support, but it is still usable.

For v0.4.0:

```text
R1 = 10 kΩ
R2 = 10 kΩ
Buzzer = optional, D13 / 3.3 V / GND
```

The original v0.2 KiCad and Gerber files are left unchanged because they are the files for the board that was actually manufactured and tested.

The next PCB revision is being worked on now. It will fold the v0.4 hardware changes into the board rather than requiring the substitutions above.

## v0.1

v0.1 worked through the web UI, but some TM1637 displays showed flicker, bad segments or unstable output.

It is kept for reference only.

## v0.2

v0.2 fixed the display problems seen on v0.1 with changes to display decoupling, TM1637 routing and the ground/power layout. It also added a development breakout.

## Before ordering v0.2

Read the [v0.4 hardware notes](../docs/hardware-upgrade-v0.4.0.md) and use the current BOM.

## PCBWay

PCB v0.2 is published here:

[View KOTH Timer PCB v0.2 on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html)

The PCBWay page shows the original v0.2 hardware.
