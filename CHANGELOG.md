# Changelog

## v0.4.0

Major standalone firmware and hardware-documentation update.

### Added

- five-second physical match start by holding both team buttons;
- optional active buzzer support on D13;
- scoring chirps, start countdown, victory sound and low-battery alarm when a buzzer is fitted;
- critical low-battery `LO:LO` display warning;
- automatic low-battery game freeze and NVS recovery;
- winner spinner/blinking-zero display animation;
- explicit v0.4 hardware-upgrade documentation.

### Changed

- battery divider changed from 100 kΩ / 100 kΩ to 10 kΩ / 10 kΩ;
- battery ADC calibration reworked around measured ESP32 ADC values;
- battery-bar thresholds changed to even 0.15 V steps;
- critical battery entry/clear thresholds documented at 3.40 V / 3.45 V;
- local admin connection simplified to manual `http://10.10.10.1/`;
- captive-portal/automatic-browser code removed;
- web control state handling and input validation cleaned up;
- BOM reviewed and corrected.

### Hardware status

PCB v0.2 remains the current tested board and is compatible with v0.4.0 after fitting 10 kΩ at R1/R2. The buzzer is optional and can be connected externally. A newer PCB revision is in development.

## v0.3.1

Small stable-UI bug-fix release based on v0.3.

- fixed team-colour inputs being overwritten by live updates;
- fixed match-duration editing while live UI updates were active;
- no core gameplay or hardware pinout change in that release.
