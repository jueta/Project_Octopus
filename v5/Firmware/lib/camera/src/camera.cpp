#include <camera.h>

#include <esp_camera.h>

namespace {

constexpr int PWDN_GPIO_NUM = -1;
constexpr int RESET_GPIO_NUM = -1;
constexpr int XCLK_GPIO_NUM = 10;
constexpr int SIOD_GPIO_NUM = 40;
constexpr int SIOC_GPIO_NUM = 39;
constexpr int Y9_GPIO_NUM = 48;
constexpr int Y8_GPIO_NUM = 11;
constexpr int Y7_GPIO_NUM = 12;
constexpr int Y6_GPIO_NUM = 14;
constexpr int Y5_GPIO_NUM = 16;
constexpr int Y4_GPIO_NUM = 18;
constexpr int Y3_GPIO_NUM = 17;
constexpr int Y2_GPIO_NUM = 15;
constexpr int VSYNC_GPIO_NUM = 38;
constexpr int HREF_GPIO_NUM = 47;
constexpr int PCLK_GPIO_NUM = 13;

constexpr char TELEGRAM_HOST[] = "api.telegram.org";
constexpr uint16_t TELEGRAM_PORT = 443;
constexpr unsigned long RESPONSE_TIMEOUT_MS = 10000;
constexpr size_t UPLOAD_CHUNK_SIZE = 1024;

} // namespace

bool initializeCamera()
{
  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_DRAM;

  if (psramFound())
  {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
  }

  const esp_err_t error = esp_camera_init(&config);
  if (error != ESP_OK)
  {
    Serial.printf("Camera initialization failed with error 0x%x\n", error);
    return false;
  }

  Serial.printf("Camera ready (%s PSRAM).\n", psramFound() ? "with" : "without");
  return true;
}

bool sendPhotoToTelegram(WiFiClientSecure &client, const String &chatId, const String &botToken)
{
  // The first frame after startup is often stale or poorly exposed.
  camera_fb_t *frame = esp_camera_fb_get();
  if (frame != nullptr)
  {
    esp_camera_fb_return(frame);
  }

  frame = esp_camera_fb_get();
  if (frame == nullptr)
  {
    Serial.println("Camera capture failed.");
    return false;
  }

  Serial.printf("Uploading %u-byte JPEG to Telegram.\n", frame->len);
  if (!client.connect(TELEGRAM_HOST, TELEGRAM_PORT))
  {
    Serial.println("Could not connect to Telegram.");
    esp_camera_fb_return(frame);
    return false;
  }

  const String boundary = "ProjectOctopusBoundary";
  const String head = "--" + boundary +
                      "\r\nContent-Disposition: form-data; name=\"chat_id\"\r\n\r\n" + chatId +
                      "\r\n--" + boundary +
                      "\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"octopus.jpg\""
                      "\r\nContent-Type: image/jpeg\r\n\r\n";
  const String tail = "\r\n--" + boundary + "--\r\n";
  const size_t contentLength = head.length() + frame->len + tail.length();

  client.println("POST /bot" + botToken + "/sendPhoto HTTP/1.1");
  client.println("Host: " + String(TELEGRAM_HOST));
  client.println("Connection: close");
  client.println("Content-Length: " + String(contentLength));
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println();
  client.print(head);

  for (size_t offset = 0; offset < frame->len; offset += UPLOAD_CHUNK_SIZE)
  {
    const size_t bytesRemaining = frame->len - offset;
    const size_t chunkSize = min(UPLOAD_CHUNK_SIZE, bytesRemaining);
    client.write(frame->buf + offset, chunkSize);
  }

  client.print(tail);
  esp_camera_fb_return(frame);

  const unsigned long startedAt = millis();
  while (!client.available() && client.connected() && millis() - startedAt < RESPONSE_TIMEOUT_MS)
  {
    delay(20);
  }

  const String statusLine = client.readStringUntil('\n');
  const bool accepted = statusLine.indexOf(" 200 ") >= 0;
  Serial.println("Telegram response: " + statusLine);

  while (client.available())
  {
    client.read();
  }
  client.stop();
  return accepted;
}
