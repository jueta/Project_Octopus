#include "esp_camera.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <UniversalTelegramBot.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


// OV..... camera
// Use your pins from camera.h
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

#define LED_GPIO_NUM      21

#define FLASH_LED_PIN 4

String sendPhotoTelegram(WiFiClientSecure *clientTCP, String chatId, String BOTtoken);

void handleNewMessages(int numNewMessages, bool *sendPhoto, WiFiClientSecure *clientTCP, String chatId, String BOTtoken, UniversalTelegramBot *bot, bool *flashState);

void configInitCamera();