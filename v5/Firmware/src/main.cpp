#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <camera.h>
#include <esp_sleep.h>

#include "config.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#include "secrets.example.h"
#endif

namespace {

WiFiClientSecure telegramClient;
String botToken = TELEGRAM_BOT_TOKEN;
String authorizedChatId = TELEGRAM_CHAT_ID;
UniversalTelegramBot bot(botToken, telegramClient);

unsigned long lastBotPollAt = 0;
unsigned long lastMotionAt = 0;
bool lastPirState = false;
bool statusLedOn = false;

bool credentialsAreConfigured()
{
  return strcmp(WIFI_SSID, "YOUR_WIFI_SSID") != 0 &&
         strcmp(TELEGRAM_BOT_TOKEN, "YOUR_TELEGRAM_BOT_TOKEN") != 0 &&
         strcmp(TELEGRAM_CHAT_ID, "YOUR_TELEGRAM_CHAT_ID") != 0;
}

void setStatusLed(bool on)
{
  statusLedOn = on;
  digitalWrite(octopus::STATUS_LED_PIN, on ? octopus::LED_ON_LEVEL : !octopus::LED_ON_LEVEL);
}

void blinkStatusLed(uint8_t times, uint16_t intervalMs = 120)
{
  const bool originalState = statusLedOn;
  for (uint8_t i = 0; i < times; ++i)
  {
    setStatusLed(true);
    delay(intervalMs);
    setStatusLed(false);
    delay(intervalMs);
  }
  setStatusLed(originalState);
}

float readBatteryVoltage()
{
  const uint16_t raw = analogRead(octopus::BATTERY_ADC_PIN);
  return (static_cast<float>(raw) / octopus::ADC_MAX_VALUE) *
         octopus::ADC_REFERENCE_VOLTAGE * octopus::BATTERY_DIVIDER_RATIO;
}

String wakeupReason()
{
  switch (esp_sleep_get_wakeup_cause())
  {
  case ESP_SLEEP_WAKEUP_EXT1:
    return "PIR motion";
  case ESP_SLEEP_WAKEUP_TIMER:
    return "timer";
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    return "power-on or reset";
  default:
    return "other wake source";
  }
}

bool connectToWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return true;
  }

  Serial.printf("Connecting to Wi-Fi network %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < octopus::WIFI_TIMEOUT_MS)
  {
    Serial.print('.');
    delay(250);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nWi-Fi connection timed out.");
    return false;
  }

  Serial.printf("\nWi-Fi connected. IP: %s\n", WiFi.localIP().toString().c_str());
  blinkStatusLed(2);
  return true;
}

String statusReport()
{
  String report = "Project Octopus v5 status\n";
  report += "Uptime: " + String(millis() / 1000) + " s\n";
  report += "Wake reason: " + wakeupReason() + "\n";
  report += "Wi-Fi RSSI: " + String(WiFi.RSSI()) + " dBm\n";
  report += "IP: " + WiFi.localIP().toString() + "\n";

  if (octopus::ENABLE_BATTERY_MONITOR)
  {
    report += "Battery: " + String(readBatteryVoltage(), 2) + " V\n";
  }

  report += "Free heap: " + String(ESP.getFreeHeap() / 1024) + " KiB";
  return report;
}

bool sendPhoto(const String &caption)
{
  if (!connectToWiFi())
  {
    return false;
  }

  if (!caption.isEmpty())
  {
    bot.sendMessage(authorizedChatId, caption, "");
  }

  blinkStatusLed(1);
  const bool sent = sendPhotoToTelegram(telegramClient, authorizedChatId, botToken);
  blinkStatusLed(sent ? 2 : 5, sent ? 90 : 180);
  return sent;
}

void enterDeepSleep()
{
  bot.sendMessage(authorizedChatId, "Sleeping. PIR motion will wake me.", "");
  delay(250);

  setStatusLed(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  while (digitalRead(octopus::PIR_PIN) == HIGH)
  {
    delay(100);
  }

  esp_sleep_enable_ext1_wakeup(1ULL << octopus::PIR_PIN, ESP_EXT1_WAKEUP_ANY_HIGH);
  Serial.println("Entering deep sleep.");
  Serial.flush();
  esp_deep_sleep_start();
}

String normalizedCommand(String command)
{
  command.trim();
  const int suffixStart = command.indexOf('@');
  if (suffixStart >= 0)
  {
    command.remove(suffixStart);
  }
  return command;
}

void handleTelegramMessage(const telegramMessage &message)
{
  const String chatId = String(message.chat_id);
  if (chatId != authorizedChatId)
  {
    bot.sendMessage(chatId, "Unauthorized user.", "");
    return;
  }

  const String command = normalizedCommand(message.text);
  Serial.printf("Telegram command from %s: %s\n", message.from_name.c_str(), command.c_str());

  if (command == "/photo")
  {
    sendPhoto("Taking a fresh photo...");
  }
  else if (command == "/status" || command == "/readings")
  {
    bot.sendMessage(authorizedChatId, statusReport(), "");
  }
  else if (command == "/flash" || command == "/led")
  {
    setStatusLed(!statusLedOn);
    bot.sendMessage(authorizedChatId, statusLedOn ? "Status LED on." : "Status LED off.", "");
  }
  else if (command == "/sleep")
  {
    enterDeepSleep();
  }
  else if (command == "/start" || command == "/help")
  {
    String help = "Project Octopus v5\n\n";
    help += "/photo - capture and send a photo\n";
    help += "/status - device, network, and battery telemetry\n";
    help += "/led - toggle the onboard status LED\n";
    help += "/sleep - sleep until PIR motion\n";
    help += "/help - show this command list";
    bot.sendMessage(authorizedChatId, help, "");
  }
  else
  {
    bot.sendMessage(authorizedChatId, "Unknown command. Send /help for the command list.", "");
  }
}

void pollTelegram()
{
  if (millis() - lastBotPollAt < octopus::BOT_POLL_INTERVAL_MS)
  {
    return;
  }
  lastBotPollAt = millis();

  if (!connectToWiFi())
  {
    return;
  }

  int messageCount = bot.getUpdates(bot.last_message_received + 1);
  while (messageCount > 0)
  {
    for (int i = 0; i < messageCount; ++i)
    {
      handleTelegramMessage(bot.messages[i]);
    }
    messageCount = bot.getUpdates(bot.last_message_received + 1);
  }
}

void detectMotion()
{
  if (!octopus::ENABLE_PIR_SENSOR)
  {
    return;
  }

  const bool motion = digitalRead(octopus::PIR_PIN) == HIGH;
  const bool cooldownFinished = lastMotionAt == 0 ||
                                millis() - lastMotionAt >= octopus::MOTION_COOLDOWN_MS;

  if (motion && !lastPirState && cooldownFinished)
  {
    lastMotionAt = millis();
    sendPhoto("Motion detected.");
    if (octopus::AUTO_SLEEP_AFTER_MOTION)
    {
      enterDeepSleep();
    }
  }

  lastPirState = motion;
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println("\nProject Octopus v5 starting.");

  pinMode(octopus::STATUS_LED_PIN, OUTPUT);
  setStatusLed(false);
  if (octopus::ENABLE_PIR_SENSOR)
  {
    pinMode(octopus::PIR_PIN, INPUT_PULLDOWN);
    lastPirState = digitalRead(octopus::PIR_PIN) == HIGH;
  }
  if (octopus::ENABLE_BATTERY_MONITOR)
  {
    analogReadResolution(12);
  }

  if (!initializeCamera())
  {
    Serial.println("Camera initialization failed; rebooting in five seconds.");
    blinkStatusLed(10, 100);
    delay(5000);
    ESP.restart();
  }

  if (!credentialsAreConfigured())
  {
    Serial.println("Credentials are placeholders. Copy include/secrets.example.h to include/secrets.h and edit it.");
    return;
  }

  telegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  if (!connectToWiFi())
  {
    return;
  }

  bot.sendMessage(authorizedChatId, "Project Octopus v5 is online. Send /help for commands.", "");

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1)
  {
    sendPhoto("Woke up after PIR motion.");
    if (octopus::AUTO_SLEEP_AFTER_MOTION)
    {
      enterDeepSleep();
    }
  }
}

void loop()
{
  if (!credentialsAreConfigured())
  {
    delay(1000);
    return;
  }

  detectMotion();
  pollTelegram();
  delay(10);
}
