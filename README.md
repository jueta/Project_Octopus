# Project Octopus

From a battery-powered monster camera concept to a focused `v5` wired room camera.

<p align="center">
  <img src="docs/Screenshot%202026-03-26%20at%2020.10.29.png" alt="Latest Project Octopus CAD concept" width="45%" />
  <img src="docs/34A62697-033D-4E2D-BD9E-1FB0015F8787_1_105_c.jpeg" alt="Project Octopus front CAD render" width="45%" />
</p>

## Overview

Project Octopus is a personal hardware and firmware project to build a custom room security camera around a tiny ESP32 camera board, with its own enclosure, electronics, and visual identity.

The original vision was ambitious and very personal:

- create a monster-like camera object inspired by an octopus shape,
- learn Fusion 360 by designing the enclosure from scratch,
- use a 3D printer to turn the design into real hardware,
- build a battery-powered wireless camera that could stay in a room for a long time,
- wake only when motion is detected,
- capture media and send it through Telegram or email.

This repository now preserves that battery-era prototype as an archive while opening a cleaner `v5` workspace for the next iteration.

## Why It Is Called Octopus

The project was never meant to be only a utilitarian security camera. From the start, the goal was to create an object with personality: something between a creature, a sculpture, and a useful device.

That is where the name **Octopus** came from. The enclosure explorations gradually moved toward a soft monster or octopus-like form, and that visual identity became part of the motivation for the whole build.

## Project Story

Project Octopus started as a dream project that combined several interests at once:

- embedded electronics,
- camera systems,
- low-power design,
- Telegram automation,
- 3D modeling in Fusion 360,
- physical prototyping with a 3D printer.

The early idea was to create a small wireless room camera using an ESP32 camera module. Over time, that evolved into a more compact board choice based on the `seeed_xiao_esp32s3` platform currently used in this repository.

The original `v4` concept was built around a battery and a PIR sensor:

- the PIR sensor would stay passive while the main board slept,
- motion would wake the ESP32,
- the camera would take a picture, or eventually record a short video,
- the device would send the result through Telegram or email,
- then it would go back to sleep to save power.

This was the architecture that motivated the electronics work, the soldering, the enclosure fitting, and the firmware that now lives in the archived `v4` area of the repository.

## What Was Built In v4

By the time the project paused, a lot of real work had already been completed:

- several generations of enclosure and hardware experiments,
- real Fusion 360 modeling and print iteration,
- purchased and assembled electronics,
- soldered prototypes,
- battery voltage monitoring,
- PIR-triggered wakeup design,
- working camera capture and Telegram delivery,
- a physical prototype that proved the main interaction loop.

The project did not stop because nothing worked. It stopped because the most important assumption did not hold up in real-world testing.

## Why Development Paused

The biggest bottleneck was energy consumption.

The dream was for the device to run wirelessly for a very long time, ideally for months, by sleeping most of the time and only waking when the passive PIR sensor detected movement. On paper, that sounded like the right strategy.

In practice, the camera, Wi-Fi connection, and media transfer consumed much more energy than expected. During the first battery tests, the prototype only lasted for a few hours instead of anything close to the multi-month lifetime that had motivated the battery-powered design.

That result changed the emotional energy of the project. The idea was still exciting, but the original architecture no longer felt viable enough to keep pushing in the same direction, so the project was paused for several months.

This is an important part of the story and worth documenting clearly:

> `v4` was not a failure. It was a successful prototype that exposed the real power budget problem.

## Lessons From v4

The `v4` phase produced valuable technical and design learnings:

- battery-powered camera products live or die by power budgeting,
- a passive PIR sensor helps, but it does not solve the energy cost of camera capture and network transmission,
- low-power embedded ideas can still fail if the active phase is too expensive,
- enclosure design and electronics packaging are as important as the firmware,
- a prototype can be successful even when it proves that the first product direction is the wrong one.

These learnings are exactly why `v4` should be preserved, not discarded.

## The v5 Direction

The next phase of Project Octopus is a deliberate pivot.

Instead of trying to force the battery-powered architecture further right now, `v5` moves toward a more practical wired version:

- no battery as the primary power source,
- possibly no PIR sensor,
- a smaller enclosure designed specifically around the tiny ESP32 camera board,
- stable power for 24/7 operation,
- a more practical camera product for daily use,
- a cleaner technical base for a portfolio-quality build and possible future productization.

In other words, `v5` is not abandoning the project. It is choosing a more realistic path to make the project succeed.

## Repository Layout

```text
Project_Octopus/
|- archive/
|  `- v4/
|     |- Firmware/             Preserved battery-era firmware baseline
|     `- Hardware/             Preserved battery-era hardware references
|- v5/
|  |- Firmware/                Active firmware workspace for the wired redesign
|  |- Hardware/                Active hardware and enclosure work for v5
|  `- docs/                    Active design notes and decision records for v5
|- docs/
|  |- v0/ ... v4/              Historical prototype photos and design snapshots
|  `- *.jpeg / *.png           Latest v4-era loose renders and prototype photos
`- README.md                   Project story, archive strategy, and active direction
```

## Archive Strategy

The repository now keeps both tracks of the project:

- [`archive/v4/`](archive/v4/) preserves the last battery-first prototype as a stable reference,
- [`v5/`](v5/) is the active workspace for the new wired direction,
- [`docs/`](docs/) remains the shared visual history of the project.

This structure makes it easier to move forward without losing the learnings, code, and hardware context that came from `v4`.

## Archived v4 Baseline

The previous working prototype now lives under [`archive/v4/`](archive/v4/).

Main files:

- [`archive/v4/Firmware/src/main.cpp`](archive/v4/Firmware/src/main.cpp): battery reading, Wi-Fi connection, PIR handling, Telegram messaging, and wake/sleep flow
- [`archive/v4/Firmware/lib/camera/src/camera.cpp`](archive/v4/Firmware/lib/camera/src/camera.cpp): camera initialization and photo upload
- [`archive/v4/Hardware/references/`](archive/v4/Hardware/references/): pinout, PIR, and flashing references

### What the archived code already does

- boots the board,
- reads battery voltage from `A0`,
- initializes the camera,
- connects to Wi-Fi,
- sends a Telegram notification,
- captures and uploads a photo,
- prepares wake on PIR input.

### Current limitations in the archived code

- Wi-Fi and Telegram configuration still lives directly in the historical `main.cpp` snapshot,
- deep sleep is still disabled for debugging,
- the code is tied to the battery and PIR assumptions,
- there are no automated tests yet.

### Building the archived v4 firmware

```bash
cd archive/v4/Firmware
pio run -e seeed_xiao_esp32s3
```

## Active v5 Workspace

The active implementation lives under [`v5/`](v5/). Its firmware restores and consolidates the strongest software from the earlier prototypes into a portfolio-ready XIAO ESP32S3 camera:

- OV2640 camera initialization with PSRAM-aware frame buffers,
- TLS photo uploads to the Telegram Bot API,
- an interactive `/photo`, `/status`, `/led`, `/sleep`, and `/help` chatbot,
- owner-only command authorization,
- PIR-triggered photos with a motion cooldown,
- battery, network, wake-reason, uptime, and heap telemetry,
- optional PIR wake and deep sleep from the battery-era design,
- local credentials kept outside source control.

The wired v5 configuration remains online by default so Telegram commands are responsive. The low-power wake, photograph, and sleep loop can be enabled with one configuration switch.

See [`v5/Firmware/README.md`](v5/Firmware/README.md) for setup, architecture, build, and upload instructions. Hardware and enclosure work remains in [`v5/Hardware/`](v5/Hardware/), with active design notes in [`v5/docs/`](v5/docs/).

## Hardware Snapshot

The current prototype direction represented in the archive includes:

- Seeed XIAO ESP32S3-based camera board target
- PIR motion sensor
- battery power experiments
- resistor-divider battery reading on `A0`
- custom 3D-printed enclosure work

Useful archived hardware references:

- [ESP-CAM pin reference](archive/v4/Hardware/references/esp-cam-pin.png)
- [PIR sensor reference](archive/v4/Hardware/references/PIR-sensor.png)
- [ESP-CAM programming reference](archive/v4/Hardware/references/hardware-program-espcam.png)

## Working With The Current Prototype

Use the active v5 firmware for the complete camera and Telegram chatbot implementation.

### 1. Install PlatformIO

Recommended options:

- [PlatformIO IDE](https://platformio.org/platformio-ide) for VS Code
- [PlatformIO Core](https://platformio.org/install/cli) for terminal-based development

Active target:

- board: `seeed_xiao_esp32s3`
- framework: `arduino`

### 2. Create Telegram credentials

Use the following Telegram bots:

- `@BotFather` to create the bot and get the bot token
- `@myidbot` or `@userinfobot` to get your chat ID

### 3. Configure local secrets

```bash
cd v5/Firmware
cp include/secrets.example.h include/secrets.h
```

Edit `include/secrets.h` with your Wi-Fi SSID/password, Telegram bot token, and authorized chat ID. This local file is ignored by Git.

### 4. Build

```bash
cd v5/Firmware
pio run -e seeed_xiao_esp32s3
```

### 5. Upload

```bash
cd v5/Firmware
pio run -t upload -e seeed_xiao_esp32s3
```

### 6. Open the serial monitor

```bash
cd v5/Firmware
pio device monitor -b 115200
```

## Latest Prototype Gallery

The loose images directly under [`docs/`](docs/) represent the latest design and prototype state before the `v5` pivot.

### CAD and concept renders

<p align="center">
  <img src="docs/Screenshot%202026-03-26%20at%2020.10.29.png" alt="Latest Project Octopus CAD concept render" width="45%" />
  <img src="docs/34A62697-033D-4E2D-BD9E-1FB0015F8787_1_105_c.jpeg" alt="Project Octopus octopus-style CAD render" width="45%" />
</p>

### Printed prototype photos

<p align="center">
  <img src="docs/3C70D7E1-2D11-4DD6-9304-0E0AE23C6437_4_5005_c.jpeg" alt="Prototype placed on a table" width="22%" />
  <img src="docs/47348721-8C86-447E-971B-E8DB96C7AEB2_4_5005_c.jpeg" alt="Prototype in hand side view" width="22%" />
  <img src="docs/4DB199B9-DB48-4249-AE91-1B9658109802_4_5005_c.jpeg" alt="Prototype rear view with USB cable" width="22%" />
  <img src="docs/9D8A7F97-36C7-4DE6-BF87-432E71B4C19A_1_105_c.jpeg" alt="Prototype front view showing PIR and camera openings" width="22%" />
</p>

These images are useful because they show the exact point where the project paused: the device was real, assembled, and visually close to the intended direction, but the energy model still needed a fundamental rethink.

## Prototype History

Past versions are archived in [`docs/`](docs/) by generation:

- [`docs/v0/`](docs/v0/): early exposed electronics and proof-of-concept wiring
- [`docs/v1/`](docs/v1/): first wall-mounted ESP-CAM and PIR assembly
- [`docs/v2/`](docs/v2/): boxed prototype experiments
- [`docs/v3/`](docs/v3/): octopus enclosure exploration, internal fit studies, and CAD analysis
- [`docs/v4/`](docs/v4/): latest compact battery-era prototype and refinement snapshots

The shared 3D model link stored in [`docs/README`](docs/README) is:

- [Autodesk Viewer / model link](https://a360.co/4iRKNNP)

## References

Technical references that informed the prototype:

- [Random Nerd Tutorials: ESP32-CAM send photo to Telegram](https://randomnerdtutorials.com/telegram-esp32-cam-photo-arduino/)
- [Random Nerd Tutorials: ESP32-CAM shield and Telegram example](https://randomnerdtutorials.com/esp32-cam-shield-pcb-telegram/)
- [Brian Lough / Universal Arduino Telegram Bot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)

## Closing Note

Project Octopus is now entering a better-defined phase.

`v4` proved the concept, exposed the power limitations, and created a strong design foundation. `v5` is the chance to make the project simpler, more reliable, and more presentable without losing the ambition that made it exciting in the first place.
