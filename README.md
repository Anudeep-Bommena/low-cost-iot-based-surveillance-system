# 📸 ESP32-CAM Telegram Surveillance System

An IoT-based smart surveillance system using **ESP32-CAM**, **PIR motion sensor**, and **Telegram Bot**.
This project detects motion, captures images, and sends them instantly to your Telegram account.

---

## 🚀 Features

* 📷 Capture photos using ESP32-CAM
* 🕵️ Motion detection using PIR sensor
* 📩 Send images to Telegram automatically
* 🎮 Control devices via Telegram commands
* 🔁 Continuous monitoring system
* 🔌 Control relay (for lights/alarm)
* ⚙️ Control servo motor remotely

---

## 🛠️ Hardware Requirements

* ESP32-CAM module
* PIR Motion Sensor
* Servo Motor
* Relay Module
* Jumper wires
* Power supply (5V)

---

## 💻 Software Requirements

* Arduino IDE
* ESP32 Board Package
* Required Libraries:

  * `esp_camera.h`
  * `WiFi.h`
  * `WiFiClientSecure.h`
  * `UniversalTelegramBot.h`
  * `ESP32Servo.h`

---

## 🔌 Pin Configuration

| Component    | ESP32-CAM Pin |
| ------------ | ------------- |
| PIR Sensor   | GPIO 12       |
| Servo Motor  | GPIO 13       |
| Relay Module | GPIO 14       |

---

## ⚙️ Setup Instructions

1. Install Arduino IDE
2. Add ESP32 board support
3. Install required libraries
4. Open the `.ino` file
5. Update credentials:

   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* botToken = "YOUR_BOT_TOKEN";
   const char* chatID = "YOUR_CHAT_ID";
   ```
6. Upload code to ESP32-CAM
7. Open Serial Monitor (115200 baud)

---

## 🤖 Telegram Commands

| Command      | Action                 |
| ------------ | ---------------------- |
| `/photo`     | Capture and send image |
| `/servo_on`  | Rotate servo (180°)    |
| `/servo_off` | Reset servo (0°)       |
| `/relay_on`  | Turn ON relay          |
| `/relay_off` | Turn OFF relay         |

---

## 🔄 Working Principle

1. ESP32 connects to WiFi
2. PIR sensor detects motion
3. Camera captures images
4. Images sent to Telegram
5. User can control devices via commands

---

## 📷 Output Example

* Motion detected → 2 images sent to Telegram
* Manual `/photo` → instant capture

---

## ⚠️ Notes

* Ensure proper power supply (ESP32-CAM is sensitive)
* Use external 5V supply for stable operation
* Telegram bot must be created using BotFather

---

## 📈 Future Improvements

* Live video streaming
* Face recognition
* Cloud storage integration
* Mobile app control

---

## 👨‍💻 Author

**Anudeep**

---

## 📄 License

This project is open-source and free to use for educational purposes.

---

