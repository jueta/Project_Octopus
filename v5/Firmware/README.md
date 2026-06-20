# Project Octopus v5 firmware

The active firmware is a working Telegram-connected camera for the Seeed XIAO ESP32S3 Sense. It restores the strongest parts of the earlier prototypes in one recruiter-readable codebase.

## Features

- initializes the OV2640 camera and adapts image buffers to available PSRAM,
- captures JPEGs and uploads them directly to the Telegram Bot API over TLS,
- accepts `/photo`, `/status`, `/led`, `/sleep`, and `/help` commands,
- rejects commands from chat IDs other than the configured owner,
- detects PIR rising edges and sends motion-triggered photos with a cooldown,
- reports Wi-Fi, uptime, heap, wake reason, and battery voltage telemetry,
- supports PIR wake from deep sleep while remaining online by default for wired v5 use,
- keeps Wi-Fi and Telegram credentials outside source control.

## Configure

```bash
cd v5/Firmware
cp include/secrets.example.h include/secrets.h
```

Edit `include/secrets.h` with your Wi-Fi SSID/password, Telegram bot token, and authorized chat ID. The file is ignored by Git.

Hardware and behavior switches are in `include/config.h`. `AUTO_SLEEP_AFTER_MOTION` is `false` for the wired v5 design; set it to `true` to reproduce the battery-era wake, photograph, and sleep loop.

## Build and upload

```bash
pio run -e seeed_xiao_esp32s3
pio run -t upload -e seeed_xiao_esp32s3
pio device monitor -b 115200
```

With no local `secrets.h`, the checked-in example credentials are used so CI and portfolio visitors can still compile the firmware. Networking remains disabled until real credentials are configured.
