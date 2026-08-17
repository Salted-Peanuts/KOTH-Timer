# KOTH Timer PCB v0.1 - Known Display Issues

This folder contains the first manufactured PCB design for the KOTH Timer.

## Status

**Manufactured and partially working, but not recommended for new builds.**

PCB v0.1 has been manufactured and the timer is functional through the web UI, but this revision has known display reliability issues.

## Known Issue

Some TM1637 displays may show:

* Interference
* Flickering
* Incorrect segments
* Unstable or inconsistent output

The main timer logic and web interface can still function, but the display behaviour is not reliable enough for this revision to be recommended as the main PCB build.

## Likely Causes

The display issues may be related to:

* Shared TM1637 clock routing
* Not enough local capacitance near each display header
* Display power noise
* Signal integrity issues on CLK/DIO traces
* Long display wiring or cable effects
* TM1637 modules being sensitive to power or clock noise

## Recommended Use

Use this revision for:

* Reference
* Debugging
* Comparing against newer PCB revisions
* Learning what changed between board versions

Do not order this revision for a new build unless you are comfortable debugging and modifying the board.

## Suggested Bodge Fixes

If you already have a v0.1 PCB, possible fixes to try include:

* Lower the TM1637 display brightness in firmware.
* Add a 100 uF electrolytic capacitor across 5V and GND near each display header.
* Add a 100 nF ceramic capacitor near each display header.
* Add a 100 ohm series resistor on the shared TM1637 CLK line.
* Add 47 to 100 ohm series resistors on the TM1637 DIO lines if needed.
* Keep display wiring as short and tidy as practical.

These fixes may improve reliability but are not guaranteed.

## Version

PCB version:

```text
v0.1
```

Status:

```text
Known display issues
```

Recommended for new builds:

```text
No
```
