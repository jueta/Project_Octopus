#pragma once

#include <Arduino.h>

namespace octopus {

// Hardware configuration for the XIAO ESP32S3 Sense prototype.
constexpr int PIR_PIN = 2; // D1 / GPIO2
constexpr int BATTERY_ADC_PIN = A0;
constexpr int STATUS_LED_PIN = LED_BUILTIN;
constexpr int LED_ON_LEVEL = LOW; // XIAO onboard LED is active-low.

// v5 is wired and stays online by default. These switches preserve the v4
// PIR, battery, and low-power behaviors for hardware experiments.
constexpr bool ENABLE_PIR_SENSOR = true;
constexpr bool ENABLE_BATTERY_MONITOR = true;
constexpr bool AUTO_SLEEP_AFTER_MOTION = false;

constexpr float ADC_MAX_VALUE = 4095.0F;
constexpr float ADC_REFERENCE_VOLTAGE = 3.3F;
constexpr float BATTERY_DIVIDER_RATIO = 2.0F;

constexpr unsigned long WIFI_TIMEOUT_MS = 20000;
constexpr unsigned long BOT_POLL_INTERVAL_MS = 1000;
constexpr unsigned long MOTION_COOLDOWN_MS = 30000;

} // namespace octopus
