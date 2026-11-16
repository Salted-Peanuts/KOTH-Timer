# Changelog

## PCB hardware v0.2.1 (minor revision; firmware remains v0.4.0)

- Recommended four-layer PCB source/manufacturing update, derived from manufactured and tested v0.2 routing and circuit.
- Corrected battery divider R1/R2 to 10 kΩ in the schematic/BOM, matching firmware v0.4.0 ADC calibration.
- Updated silkscreen to identify PCB v0.2.1 and clarified the schematic layout.
- Updated builder BOM coverage for C2–C6, C7/C8, and R10–R13.
- New PCB v0.2.1 source files distributed under CERN-OHL-S-2.0; firmware v0.4.0 and historical PCB files retain original MIT terms.
- No separate v0.2.1 manufacturing/field test claimed; buzzer remains optional through the existing DEV header.

## v0.4.0

### Added

- 5-second physical match start using both team buttons
- optional active buzzer on D13
- scoring chirps, start countdown, victory sound and low-battery alarm
- `LO:LO` critical-battery warning
- automatic low-battery freeze and saved-match recovery
- winner spinner / blinking-zero animation

### Changed

- R1/R2 battery divider: 100 kΩ -> 10 kΩ
- battery ADC calibration
- battery-bar voltage thresholds
- low-battery enter/clear thresholds: 3.40 V / 3.45 V
- admin connection now uses manual `http://10.10.10.1/`
- captive-portal code removed
- referee webpage control-state handling cleaned up
- BOM reviewed and corrected

### Hardware

PCB v0.2 remains the current tested board.

For v0.4.0, fit 10 kΩ at R1/R2. The buzzer is optional and can be connected externally.

That statement records the firmware v0.4.0 release-era status. The subsequent PCB-only update is documented above under PCB v0.2.1.

## v0.3.1

Small UI fix release based on v0.3.

- fixed team-colour fields being overwritten by live updates
- fixed match-duration editing while the UI was updating
- no gameplay or hardware pinout changes
