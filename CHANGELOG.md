# Changelog

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

A revised PCB is in development.

## v0.3.1

Small UI fix release based on v0.3.

- fixed team-colour fields being overwritten by live updates
- fixed match-duration editing while the UI was updating
- no gameplay or hardware pinout changes
