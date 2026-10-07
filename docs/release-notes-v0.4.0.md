# KOTH Timer firmware v0.4.0 — battery safety, physical start and optional buzzer

v0.4.0 is a major update from v0.3.1.

It adds physical match starting, optional audible feedback, a new winner animation, significantly reworked battery monitoring, and automatic low-battery game recovery.

## Read this first — hardware change

> [!IMPORTANT]
> Firmware v0.4.0 uses a **10 kΩ / 10 kΩ battery divider**.
>
> Existing builds using the old **100 kΩ / 100 kΩ** divider should replace **R1 and R2 with 10 kΩ** before relying on the v0.4 battery voltage, battery bar or low-battery protection.

The buzzer is **optional**. Firmware v0.4.0 still works normally with no buzzer connected.

See [hardware upgrade notes](hardware-upgrade-v0.4.0.md).

## Battery-divider change

Old:

```text
Battery -> 100 kΩ -> A0 -> 100 kΩ -> GND
```

v0.4.0:

```text
Battery -> 10 kΩ -> A0 -> 10 kΩ -> GND
```

The original divider's high impedance allowed the Arduino ADC/input loading to shift the midpoint significantly. The lower-impedance 10 kΩ / 10 kΩ divider produced much more repeatable readings and is the basis of the new firmware calibration.

## Optional active buzzer

v0.4.0 supports a 3.3–5 V active buzzer:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

When fitted, the buzzer provides:

- a short chirp for accepted scoring seconds;
- the manual-start countdown;
- the victory pattern;
- a repeating critical-battery alarm.

If no buzzer is connected, all non-audio features continue to operate normally.

## Physical match start

A match can now be started without the referee webpage.

When stopped, hold both team buttons for 5 seconds.

With a buzzer fitted:

```text
BEEP ... BEEP ... BEEEEEEP
                    ^
                    GO
```

The match goes live at the start of the long beep.

If both buttons remain held at GO, the normal contested rule applies and neither team scores until only one button remains held.

## Battery monitoring and protection

v0.4.0 uses a calibrated ESP32 ADC reading and the new 10 kΩ / 10 kΩ divider.

Battery bar thresholds:

```text
5 bars: >= 4.05 V
4 bars: 3.90 V to < 4.05 V
3 bars: 3.75 V to < 3.90 V
2 bars: 3.60 V to < 3.75 V
1 bar : 3.45 V to < 3.60 V
<3.45 V: flashing red/final segment
```

Critical battery:

```text
Enter: below 3.40 V
Clear: above 3.45 V
```

When a running game enters the critical state:

- all four TM1637 displays flash `LO:LO`;
- the optional buzzer sounds a repeating warning;
- gameplay is frozen;
- Team A and Team B remaining time is saved;
- partial-second scoring state is saved.

After battery replacement/reboot, the interrupted match is restored **paused**. It never automatically resumes; the referee chooses when to Resume.

## Winner display

The winning team's displays now run a dedicated animation:

```text
3 spinner rotations
       ↓
blinking 00:00
       ↓
repeat until reset
```

The losing team's displays remain steady on their final time. A draw animates both teams.

## Referee/admin interface

The local interface remains at:

```text
SSID: KOTH-Timer
http://10.10.10.1/
```

Captive-portal/automatic browser launching was tested during development but was inconsistent across platforms, so it has been removed.

The supported flow is now deliberately simple:

```text
connect to KOTH-Timer Wi-Fi
        ↓
open http://10.10.10.1/
```

## Other firmware cleanup

- improved Start/Pause/Resume state guarding;
- improved webpage control enabled/disabled states;
- validated team-colour inputs before saving;
- no-cache responses for local UI/state data;
- retained WebSocket live updates with polling fallback;
- cleaner Wi-Fi startup;
- no DNSServer/captive-portal dependency;
- reviewed battery documentation and thresholds;
- corrected even battery-bar voltage steps.

## PCB compatibility

PCB v0.2 remains the current **tested** PCB revision.

To use it with v0.4.0:

- fit 10 kΩ at R1/R2;
- optionally connect the active buzzer externally using D13/3.3 V/GND.

An updated PCB revision is currently being developed to incorporate these newer changes directly.

The v0.2 KiCad/Gerber files remain unchanged as the record of the board that was actually manufactured and tested.

## BOM review

The v0.4 BOM has been reviewed alongside this release.

Notable corrections:

- R1/R2: 100 kΩ -> **10 kΩ**;
- optional active buzzer added;
- the old purchase link for the 220 Ω resistors was found to point to a **220 kΩ** Jaycar part (RR0628); the current BOM points to the correct **220 Ω RR0556** part;
- PCB v0.2 compatibility notes added.

## Upgrade from v0.3 / v0.3.1

Required before relying on the new battery features:

```text
R1: 100 kΩ -> 10 kΩ
R2: 100 kΩ -> 10 kΩ
```

Optional:

```text
3.3–5 V active buzzer
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

No change is required to the existing team buttons, button LEDs, TM1637 display pinout, battery button or five-segment battery indicator wiring.

## Recommended release combination

```text
Firmware: v0.4.0
PCB:      v0.2-tested with R1/R2 = 10 kΩ
Buzzer:   optional
```

Before event use, bench-test the exact firmware/hardware combination, especially battery voltage accuracy and the low-battery freeze -> power cycle -> paused recovery -> Resume sequence.
