# Documentation

Current firmware:

```text
v0.4.0
Firmware/KOTH_Timer_v0_4_0/KOTH_Timer_v0_4_0.ino
```

Recommended PCB for new builds:

```text
PCB/v0.2.1/
```

Manufactured and field-tested reference: `PCB/v0.2-tested/`. PCB v0.2.1 is a minor source and silkscreen update using the same tested routing, but has not been separately manufactured.

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
- F1 is a 1 A fast-blow glass cartridge fuse (6.3 × 32 mm)
- the active buzzer on D13 is optional
- PCB v0.2 is still usable with those changes

`docs/BOM.csv` includes all PCB v0.2.1 passives (R10–R13, C3–C8 as well as R1/R2). The matching Excel builder BOM, [`docs/BOM.xlsx`](BOM.xlsx), has also been uploaded and matches the v0.2.1 component update. The archive-specific PCBWay assembly export is included inside the v0.2.1 Gerber ZIP but **is not a full builder parts list**. Older component references should be treated as historical.

## Schematic

For current builds, use the [cleaned-up PCB v0.2.1 schematic](../PCB/v0.2.1/KOTH_v0.2.1.pdf) (R1/R2 = 10 kΩ). Its KiCad 9 layout is now organised into labelled sections to help builders trace the power/battery sensing, team inputs, display connections and Nano ESP32 pins. Its matching editable source is [KOTH_v0.2.1.kicad_sch](../PCB/v0.2.1/KOTH_v0.2.1.kicad_sch).

The PDF copy in this directory is a convenience copy; always use the canonical [current schematic](../PCB/v0.2.1/KOTH_v0.2.1.pdf) under `PCB/v0.2.1/` for the latest correction. After any future schematic revision, both copies must be regenerated together.

The older `docs/Wiring_schematic.pdf` was removed from the current documentation to avoid conflicting instructions. It remains recoverable in Git history, and the original manufactured v0.2 KiCad files are retained in `PCB/v0.2-tested/`. The optional external buzzer is covered in the [build guide](build-guide.md).

## Wi-Fi

```text
SSID: KOTH-Timer
Password: none
Admin: http://10.10.10.1/
Test:  http://10.10.10.1/ping
```

The network has no internet access. Automatic captive-portal opening is not used in v0.4.0.

## PCBWay

The previously manufactured v0.2 board is also available here:

https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html

Verify the revision of the Gerbers offered by PCBWay before ordering; its page may still show the original v0.2 materials until manually updated. GitHub `PCB/v0.2.1/` is the recommended current source.
