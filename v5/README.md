# Project Octopus v5

This is the active workspace for the wired redesign of Project Octopus.

The goal of `v5` is to keep the spirit of the project while simplifying the architecture enough to make steady progress again.

Current direction:

- wired power instead of a battery-first design,
- a smaller enclosure around the tiny camera board,
- a complete Telegram-connected camera firmware,
- room to decide later whether PIR still belongs in the product.

Workspace structure:

- [`Firmware/`](Firmware/): active PlatformIO firmware with camera capture, Telegram chatbot commands, PIR alerts, telemetry, and optional deep sleep
- [`Hardware/`](Hardware/): active CAD, wiring, and enclosure work
- [`docs/`](docs/): active notes, sketches, decisions, and planning
