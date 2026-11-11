# Documentation

Current firmware:

```text
v0.4.0
Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino
```

Current tested PCB:

```text
PCB/v0.2-tested/
```

A revised PCB is being worked on.

## Start here

- [Build guide](build-guide.md)
- [v0.4.0 hardware changes](hardware-upgrade-v0.4.0.md)
- [v0.4.0 release notes](release-notes-v0.4.0.md)

## BOM

Current builder BOM:

```text
docs/BOM.csv
docs/BOM.xlsx
```

For v0.4.0:

- R1/R2 are 10 kΩ
- the active buzzer on D13 is optional
- PCB v0.2 is still usable with those changes

`docs/KOTH-Timer__PCB-BOM.xlsx` is kept as a reference for the original PCB v0.2 assembly and still reflects that older board.

## Schematic

```text
docs/Wiring_schematic.pdf
```

The schematic predates the v0.4 resistor and buzzer changes, so check it against the hardware-upgrade notes.

## Wi-Fi

```text
SSID: KOTH-Timer
Password: none
Admin: http://10.10.10.1/
Test:  http://10.10.10.1/ping
```

The network has no internet access. Automatic captive-portal opening is not used in v0.4.0.

## PCBWay

The tested v0.2 board is also available here:

https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html

That page reflects the original v0.2 hardware. Use the current BOM and v0.4 upgrade notes when pairing it with v0.4.0 firmware.
