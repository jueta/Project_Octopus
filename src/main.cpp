#include <camera.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "esp_camera.h"
#include "esp_sleep.h"

// Replace with your WiFi and bot credentials
const char *ssid = "Casassanta2";
const char *password = "becky123";
String chatId = "7013307530";
String BOTtoken = "7444230490:AAH6sSvrSa69PozjPk_JgiOw_o0Fu9r02-c";

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#define PIR_GPIO GPIO_NUM_2 // D1 on XIAO ESP32S3

void blink_led(int times, int delay_ms = 200)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delay_ms);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delay_ms);
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(PIR_GPIO, INPUT_PULLDOWN); // Most PIRs idle LOW, go HIGH on motion
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);

  // Only run after wakeup from PIR
  // if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
  // {
    // Serial.println("Woke up from PIR motion!");

    // Initialize camera
    configInitCamera();

    // Connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    blink_led(5); // Blink onboard LED 5 times before sleep

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Send photo
    bot.sendMessage(chatId, "Motion detected! Sending photo...", "");
    sendPhotoTelegram(&clientTCP, chatId, BOTtoken);
  // }

  blink_led(5, 100); // Blink onboard LED 5 times before sleep

  // Wait for PIR to go LOW before sleeping
  while (digitalRead(PIR_GPIO) == HIGH)
  {
    Serial.println("Waiting for PIR to go LOW before sleep...");
    delay(500);
  }

  // EXT1: Wake on HIGH signal from PIR
  esp_sleep_enable_ext1_wakeup(1ULL << PIR_GPIO, ESP_EXT1_WAKEUP_ANY_HIGH);

  Serial.println("Sleeping now...");
  delay(100);
  esp_deep_sleep_start();
}

void loop()
{
  // Nothing here – deep sleep handles everything
}
