# KOTH Timer v0.4.0

v0.4.0 is a bigger update than v0.3.1. It adds a physical start sequence, optional buzzer support, better battery handling and match recovery after a low-battery shutdown.

## Before flashing

The battery divider changed.

Old:

```text
R1 = 100 kΩ
R2 = 100 kΩ
```

v0.4.0:

```text
R1 = 10 kΩ
R2 = 10 kΩ
```

The 10 kΩ / 10 kΩ divider is what the new battery calibration and low-battery protection were tested with.

The buzzer is optional. The firmware works normally with no buzzer connected.

## Optional buzzer

Use a 3.3–5 V active buzzer:

```text
VCC -> 3.3 V
GND -> GND
SIG -> D13
```

If fitted, it is used for scoring chirps, the start countdown, victory sound and low-battery warning.

## Physical start

When the game is stopped, hold both team buttons for 5 seconds.

With a buzzer fitted:

```text
BEEP ... BEEP ... BEEEEEEP
                    ^
                    GO
```

The game starts at the beginning of the long beep. If both buttons are still held, the point starts contested.

## Battery changes

Battery bar:

```text
5 bars  >= 4.05 V
4 bars  3.90–4.04 V
3 bars  3.75–3.89 V
2 bars  3.60–3.74 V
1 bar   3.45–3.59 V
<3.45 V flashing red bar
```

Critical battery:

```text
Enter below 3.40 V
Clear above 3.45 V
```

If the battery goes critical during a match, the timer:

- freezes the game
- flashes `LO:LO` on all four displays
- saves both team timers and partial-second state
- sounds the low-battery alarm if a buzzer is fitted

After a battery swap or reboot, the saved game comes back **paused**. It never resumes by itself.

## Winner display

The winning team's displays now run three spinner rotations followed by blinking `00:00`, repeating until reset.

The losing team stays on its final time. A draw animates both teams.

## Referee page

The admin page is still:

```text
http://10.10.10.1/
```

The automatic captive-portal page opening has been removed. Connect to `KOTH-Timer` Wi-Fi and open the address manually.

## PCB v0.2

PCB v0.2 still works with v0.4.0.

Use 10 kΩ at R1/R2 and add the buzzer externally only if you want audio.

A revised PCB is being developed with these changes built in. Until that board has been manufactured and tested, v0.2 remains the current tested PCB.

## BOM corrections

The v0.4 BOM also fixes two older documentation problems:

- R1/R2 are now listed as 10 kΩ
- the old link for the 220 Ω resistors accidentally pointed to a 220 kΩ Jaycar part; the BOM now links to the correct 220 Ω part

## Tested

The `release/v0.4.0` firmware was compiled, flashed and tested on the current KOTH Timer hardware before release.
