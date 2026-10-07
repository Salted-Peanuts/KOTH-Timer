# KOTH Timer documentation

This folder contains the current build, hardware and release documentation for KOTH Timer.

## Current release

Recommended firmware:

```text
v0.4.0
Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino
```

Current tested PCB:

```text
PCB/v0.2-tested/
```

An updated PCB revision is currently being developed.

## Read this before upgrading from v0.3.x

Firmware v0.4.0 uses a **10 kΩ / 10 kΩ battery divider** instead of the previous **100 kΩ / 100 kΩ** values.

The active buzzer added in v0.4.0 is **optional**. The firmware runs normally with no buzzer connected.

See:

- [Hardware upgrade notes](hardware-upgrade-v0.4.0.md)
- [v0.4.0 release notes](release-notes-v0.4.0.md)
- [Build guide](build-guide.md)

## BOM

Current builder BOM:

```text
docs/BOM.xlsx
docs/BOM.csv
```

Important corrections in the reviewed v0.4 BOM:

- R1/R2 are now **10 kΩ**.
- The old 220 Ω purchase URL accidentally pointed to a **220 kΩ** Jaycar part; it has been corrected.
- The active buzzer is listed as **optional**.
- PCB v0.2 compatibility notes are included.

### Legacy PCB assembly BOM

`docs/KOTH-Timer__PCB-BOM.xlsx` is retained as a historical PCB v0.2 assembly/manufacturing reference. It reflects the older board as originally manufactured and should **not** be treated as the current v0.4 builder BOM.

When assembling PCB v0.2 for firmware v0.4.0, substitute **10 kΩ at R1/R2** and add the optional buzzer externally if wanted.

## Schematic

The existing wiring schematic is:

```text
docs/Wiring_schematic.pdf
```

It predates the v0.4 hardware changes. Cross-check it with the v0.4 hardware-upgrade notes, especially the R1/R2 resistor values and optional D13 buzzer.

## Wi-Fi/admin interface

```text
SSID: KOTH-Timer
Password: none / open network
Admin page: http://10.10.10.1/
Connection test: http://10.10.10.1/ping
```

Automatic captive-portal/browser opening is intentionally removed in v0.4.0. Connect to the Wi-Fi and open the admin address manually.

## PCBWay v0.2 reference

The tested PCB v0.2 shared project remains available here:

https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html

That page represents the older v0.2 hardware/release material. Use this repository's current BOM and upgrade notes when running firmware v0.4.0.
