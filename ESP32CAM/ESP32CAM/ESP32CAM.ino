#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <esp_now.h>  // Include ESP-NOW library

// Replace with your network credentials
const char* ssid = "notyouriphone";
const char* password = "hidejy123";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// GPIO where the camera will connect
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Define the trigger token
const char* telegramBotToken = "7745485363:AAFQeGyYECRdHTVSqLgckj0msOFiNMuvxDk"; // Replace with your bot token
const char* chatId = "1939482390"; // Replace with your chat ID

// ESP-NOW structure to receive alert data
typedef struct struct_alert {
  bool flameDetected;
  bool takePhoto;
} struct_alert;

struct_alert incomingAlert;  // Create an instance to store the received alert data

// Function to send photo to Telegram
void sendPhotoToTelegram(camera_fb_t *fb) {
  String url = "https://api.telegram.org/bot" + String(telegramBotToken) + "/sendPhoto";
  HTTPClient http;

  // Set up the connection
  http.begin(url);

  // Prepare headers
  String boundary = "--------------------------boundary";
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

  // Prepare the body
  String body = "--" + boundary + "\r\n";
  body += "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n";
  body += String(chatId) + "\r\n";
  body += "--" + boundary + "\r\n";
  body += "Content-Disposition: form-data; name=\"photo\"; filename=\"image.jpg\"\r\n";
  body += "Content-Type: image/jpeg\r\n\r\n";

  // Send the request
  int httpResponseCode = http.sendRequest("POST", body + String((char*)fb->buf, fb->len) + "\r\n--" + boundary + "--\r\n");

  // Handle the response
  if (httpResponseCode > 0) {
    Serial.printf("Photo sent to Telegram, response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("Error sending photo to Telegram: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  // End the HTTP connection
  http.end();
}

void sendTextToTelegram(const String &text) {
  String url = "https://api.telegram.org/bot";
  url += telegramBotToken;
  url += "/sendMessage?chat_id=";
  url += chatId;
  url += "&text=";
  url += text;

  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();  // Use GET for simple text messages

  if (httpResponseCode > 0) {
    Serial.printf("Message sent to Telegram, response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("Error sending message to Telegram: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}

// ESP-NOW callback function to handle received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingAlert, incomingData, sizeof(incomingAlert));
  Serial.println("ESP-NOW message received");

  if (incomingAlert.flameDetected) {
    Serial.println("Alert: Flame or Smoke Detected!");
    sendTextToTelegram("Alert: Flame or Smoke Detected!");
  }

  if (incomingAlert.takePhoto) {
    Serial.println("Alert: Take a Photo!");
    // Take a photo and send it to Telegram
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
    } else {
      Serial.println("Camera capture successful");
      sendTextToTelegram("Someone is at the door!");
      sendPhotoToTelegram(fb);
      esp_camera_fb_return(fb);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.println("WiFi connected");

  // Configure the camera
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
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);  // Register the callback to receive ESP-NOW messages

  // Start server
  server.begin();
  Serial.println("Camera server started. Ready to receive ESP-NOW messages.");
}

void loop() {
  // The loop is empty because all ESP-NOW actions are handled in the callback
}
