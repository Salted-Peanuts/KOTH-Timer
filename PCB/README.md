# KOTH Timer PCB files

## Revisions

| Revision | Status |
|---|---|
| v0.1 | First manufactured board. Known TM1637 display problems. Reference only. |
| v0.2 | Manufactured, assembled and field-tested; historical reference. |
| **v0.2.1** | **Recommended for new builds.** Based on tested v0.2 routing, with corrected R1/R2 values, cleaned schematic, updated silkscreen and a later 1 A fast-blow fuse documentation correction. Not separately manufactured. |

## PCB v0.2 with firmware v0.4.0

PCB v0.2 predates the new battery-divider values and buzzer support, but it is still usable.

For v0.4.0:

```text
R1 = 10 kΩ
R2 = 10 kΩ
Buzzer = optional, D13 / 3.3 V / GND
```

The original v0.2 KiCad and Gerber files are left unchanged because they are the files for the board that was actually manufactured and tested.

PCB v0.2.1 is now published in [v0.2.1](v0.2.1/). The resistor changes, clarified fuse specification and schematic cleanup are reflected in the source and current builder BOM. The buzzer remains optional via the existing DEV header, not integrated on the PCB.

## v0.1

v0.1 worked through the web UI, but some TM1637 displays showed flicker, bad segments or unstable output.

It is kept for reference only.

## v0.2

v0.2 fixed the display problems seen on v0.1 with changes to display decoupling, TM1637 routing and the ground/power layout. It also added a development breakout.

## Before ordering

Read the [v0.4 hardware notes](../docs/hardware-upgrade-v0.4.0.md) and [build guide](../docs/build-guide.md). For a new build, use `PCB/v0.2.1/` files and the updated builder BOM (F1 = 1 A fast-blow glass cartridge, 6.3 × 32 mm). The 4-layer Gerber stack contains F.Cu, In1.Cu, In2.Cu and B.Cu. For existing v0.2 builds, fit 10 kΩ at R1/R2.

## PCBWay

[View the KOTH-Timer project on PCBWay](https://www.pcbway.com/project/shareproject/King_Of_The_Hill_ESP32_based_timer_1aeedfa7.html).

**PCB v0.2.1 is the recommended revision for new builds.** Its current design files are in [PCB/v0.2.1](v0.2.1/). The existing [PCB v0.2](v0.2-tested/) remains the manufactured/tested reference.

The PCBWay shared-project downloads are being brought into line with v0.2.1. **Verify the revision available on PCBWay before ordering**, as the public downloads may not yet have been replaced. PCBWay support will manually add CERN-OHL-S-2.0 to the updated project.

## Hardware licence

The new **v0.2.1 design files** use [CERN-OHL-S-2.0](v0.2.1/LICENSE) (strongly reciprocal). The historical v0.1 and v0.2 PCB files retain their previously published licence terms. Firmware v0.4.0 remains MIT-licensed.
