#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>  // ESP32-compatible servo library

// ===========================
// Camera model
// ===========================
#include "board_config.h"

// ===========================
// WiFi credentials
// ===========================
const char* ssid = "satya";
const char* password = "12345678";

// ===========================
// Telegram Bot
// ===========================
const char* botToken = "8206474263:AAEagPodndaRpn9aKndVoWTBP2ja_oKRbYA";
const char* chatID   = "8532403612";

WiFiClientSecure telegramClient;
UniversalTelegramBot bot(botToken, telegramClient);

// ===========================
// PIR Sensor & Servo
// ===========================
#define PIR_PIN 12
#define SERVO_PIN 13
#define RELAY_PIN 14
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000; // 1 minute

Servo myServo;

// ===========================
// Function prototypes
// ===========================
void startCameraServer();
void setupLedFlash();
void sendPhotoToTelegram(camera_fb_t *fb);
void checkTelegramCommands();

// ===========================
// Telegram Photo Function
// ===========================
void sendPhotoToTelegram(camera_fb_t *fb) {
  if (!telegramClient.connect("api.telegram.org", 443)) {
    Serial.println("Telegram connection failed");
    return;
  }

  String boundary = "ESP32CAM";
  String head =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n" +
    String(chatID) + "\r\n"
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"photo\"; filename=\"image.jpg\"\r\n"
    "Content-Type: image/jpeg\r\n\r\n";

  String tail = "\r\n--" + boundary + "--\r\n";
  int contentLength = head.length() + fb->len + tail.length();

  telegramClient.print(
    "POST /bot" + String(botToken) + "/sendPhoto HTTP/1.1\r\n"
    "Host: api.telegram.org\r\n"
    "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
    "Content-Length: " + String(contentLength) + "\r\n\r\n"
  );

  telegramClient.print(head);
  telegramClient.write(fb->buf, fb->len);
  telegramClient.print(tail);

  telegramClient.stop();
  Serial.println("Photo sent to Telegram");
}

// ===========================
// Setup
// ===========================
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);   // Relay OFF at start
  pinMode(PIR_PIN, INPUT);
  telegramClient.setInsecure();

  // Attach Servo
  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Initial position

  // Camera configuration
  camera_config_t config;
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
  config.frame_size = FRAMESIZE_VGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  startCameraServer();
  Serial.print("Camera Ready! http://");
  Serial.println(WiFi.localIP());
}

// ===========================
// Telegram Command Listener
// ===========================
void checkTelegramCommands() {

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {

    for (int i = 0; i < numNewMessages; i++) {

      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      Serial.print("Message received: ");
      Serial.println(text);

      if (text == "/photo") {

        bot.sendMessage(chat_id, "Taking photo...", "");

        camera_fb_t *fb = esp_camera_fb_get();

        if (!fb) {
          bot.sendMessage(chat_id, "Camera capture failed", "");
          Serial.println("Camera capture failed");
          return;
        }

        sendPhotoToTelegram(fb);
        esp_camera_fb_return(fb);

        Serial.println("Photo sent");
      }

      else if (text == "/servo_on") {
        myServo.write(180);
        bot.sendMessage(chat_id, "Servo ON", "");
      }

      else if (text == "/servo_off") {
        myServo.write(0);
        bot.sendMessage(chat_id, "Servo OFF", "");
      }

      else if (text == "/relay_on") {
        digitalWrite(RELAY_PIN, HIGH);
        bot.sendMessage(chat_id, "Relay ON", "");
      }

      else if (text == "/relay_off") {
        digitalWrite(RELAY_PIN, LOW);
        bot.sendMessage(chat_id, "Relay OFF", "");
      }

    }

    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

// ===========================
// Main loop
// ===========================
void loop() {
  checkTelegramCommands();  // Listen for /photo, /on, /off

  // PIR trigger
  if (digitalRead(PIR_PIN) == HIGH) {
   
    if (millis() - lastSendTime >= sendInterval) {
      lastSendTime = millis();
    
      Serial.println("PIR HIGH - Capturing 2 photos");

      for (int i = 0; i < 2; i++) {

        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
          Serial.println("Camera capture failed");
          continue;
        }

        sendPhotoToTelegram(fb);
        esp_camera_fb_return(fb);
        Serial.printf("Photo %d sent\n", i + 1);
        delay(2000);
      }

      
    }
  }
  delay(1000);

}