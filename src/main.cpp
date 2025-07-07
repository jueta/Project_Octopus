
#include <camera.h>


// Replace with your network credentials
const char *ssid = "Casassanta2";
const char *password = "becky123";

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
String chatId = "7013307530";

// Initialize Telegram BOT
String BOTtoken = "7444230490:AAH6sSvrSa69PozjPk_JgiOw_o0Fu9r02-c";


WiFiClientSecure clientTCP;

UniversalTelegramBot bot(BOTtoken, clientTCP);


bool flashState = LOW;

// Motion Sensor
bool motionDetected = false;
bool sendPhoto = false; // start taking picture


int botRequestDelay = 1000;   // mean time between scan messages
long lastTimeBotRan;     // last time messages' scan has been done




// // Indicates when motion is detected
// static void IRAM_ATTR detectsMovement(void * arg){
//   //Serial.println("MOTION DETECTED!!!");
//   motionDetected = true;
// }

void setup(){

  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);
  
  // pinMode(FLASH_LED_PIN, OUTPUT);
  // digitalWrite(FLASH_LED_PIN, flashState);

  // Config and init the camera
  configInitCamera();
  
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());



  // // PIR Motion Sensor mode INPUT_PULLUP
  // // err = gpio_install_isr_service(0); 
  // err = gpio_isr_handler_add(GPIO_NUM_13, &detectsMovement, (void *) 13);  
  // if (err != ESP_OK){
  //   Serial.printf("handler add failed with error 0x%x \r\n", err); 
  // }
  // err = gpio_set_intr_type(GPIO_NUM_13, GPIO_INTR_POSEDGE);
  // if (err != ESP_OK){
  //   Serial.printf("set intr type failed with error 0x%x \r\n", err);
  // }

  // Supposely blink flash
  // for(int i = 0; i < 5; i++){
  //   digitalWrite(FLASH_LED_PIN, i % 2);
  //   delay(500);
  // }

}

void loop(){
  if (sendPhoto == true){
    Serial.println("Preparing photo");
    sendPhotoTelegram(&clientTCP, chatId, BOTtoken); 
    sendPhoto = false; 
  }

  if(motionDetected == true){
    bot.sendMessage(chatId, "Motion detected!!", "");
    Serial.println("Motion Detected");
    sendPhotoTelegram(&clientTCP, chatId, BOTtoken);
    motionDetected = false;
  }
  
  if (millis() > lastTimeBotRan + botRequestDelay){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages){
      Serial.println("got response");
      handleNewMessages(numNewMessages, &sendPhoto, &clientTCP, chatId, BOTtoken, &bot, &flashState);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  delay(1000); // allow time for monitor to connect
  Serial.println("Hello from XIAO ESP32S3!");
}


