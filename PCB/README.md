# KOTH Timer PCB Files

This folder contains PCB design files for the KOTH Timer.

The firmware and original prototype build are working, but PCB revisions should be treated separately. Check the status of each PCB revision before ordering or manufacturing boards.


## PCB Revision Status

| Revision | Status                                                | Use                                             |
| -------- | ----------------------------------------------------- | ----------------------------------------------- |
| `v0.1`   | Manufactured, partially working, known display issues | Reference only / not recommended for new builds |
| `v0.2`   | In manufacturing, not yet tested                      | Experimental / wait for test results            |


## PCB v0.1

PCB v0.1 was the first manufactured PCB revision.

It is functionally usable through the timer’s web UI, but it has known display reliability issues.

Known issue:

* Some TM1637 displays may show interference, flicker, incorrect segments, or unstable output.

Possible contributing factors include:

* Shared TM1637 clock routing
* Not enough local decoupling near the display headers
* Display power noise
* Signal integrity issues on CLK/DIO traces
* Long display wiring or cable effects

This revision is kept for reference and comparison, but it is not recommended for new builders.


## PCB v0.2

PCB v0.2 is the next hardware revision.

It is currently in manufacturing and has not yet been assembled or tested.

v0.2 is intended to address lessons from v0.1, especially around display reliability, display signal routing, and local display power decoupling.

Do not treat v0.2 as confirmed working until test results are added to this repo.


## Recommendation

For new builders:

* Use the current tested v0.3 firmware.
* Follow the main build guide.
* Do not order a PCB revision unless its README says it has been tested.
* Treat early PCB files as experimental hardware.


## Supported By PCBWay

PCB manufacturing for newer KOTH Timer PCB revisions has been supported by PCBWay.

Their support helps make it possible to test real PCB revisions and keep the project open source.

Project files, known issues, testing notes, and recommendations remain documented openly so other builders can inspect and improve the design.
