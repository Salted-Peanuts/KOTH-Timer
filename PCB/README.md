# KOTH Timer PCB Files

This folder contains PCB design files for the KOTH Timer.

Check the status of each PCB revision before ordering or manufacturing boards.

## PCB Revision Status

| Revision | Status | Use |
|---|---|---|
| `v0.1` | Manufactured, functionally usable through the web UI, known display issues | Reference only / not recommended for new builds |
| `v0.2` | Manufactured, assembled, and tested with no known faults | Recommended PCB revision |

## PCB v0.1

PCB v0.1 was the first manufactured PCB revision.

It is functionally usable through the timer’s web UI, but it has known TM1637 display reliability issues.

Known issue:

- Some TM1637 displays may show interference, flicker, incorrect segments, or unstable output.

Possible contributing factors include:

- Shared TM1637 clock routing
- Not enough local decoupling near the display headers
- Display power noise
- Signal integrity issues on CLK/DIO traces
- Long display wiring or cable effects

This revision is kept for reference and comparison, but it is not recommended for new builders.

## PCB v0.2

PCB v0.2 is the current recommended PCB revision.

It has been manufactured, assembled, and tested with no known faults.

This revision improves on v0.1, especially around:

- TM1637 display reliability
- Local display power decoupling
- Cleaner display signal routing
- Improved PCB layout
- Reduced display interference issues

## Recommendation

For new builders:

- Use the current recommended firmware.
- Use PCB v0.2 if building from a PCB.
- Use PCB v0.1 only for reference or debugging.
- Check the main build guide before ordering parts or assembling the board.

## Supported By PCBWay

PCB manufacturing for newer KOTH Timer PCB revisions has been supported by PCBWay.

Their support helped make it possible to test real PCB revisions and keep the project open source.

Project files, known issues, testing notes, and recommendations remain documented openly so other builders can inspect, build, and improve the design.
