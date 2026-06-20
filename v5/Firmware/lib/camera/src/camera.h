#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>

bool initializeCamera();
bool sendPhotoToTelegram(WiFiClientSecure &client, const String &chatId, const String &botToken);
