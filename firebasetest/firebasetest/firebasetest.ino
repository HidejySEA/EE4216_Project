#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include "time.h"
#include <esp_now.h>

// ESP32-CAM server IP for taking photo (not used with ESP-NOW)
// const char* serverName = "http://172.20.10.7/take_photo?trigger=EE4216";

// Include the token generation process and RTDB helper.
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "notyouriphone"
#define WIFI_PASSWORD "hidejy123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyB2ivesHz3wQM1cdIfNyAr2Sx3m-33Dopg"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "menjiying@gmail.com"
#define USER_PASSWORD "Hidejy666"

// Insert RTDB URL
#define DATABASE_URL "https://ee4216-project-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
String tempPath = "/temperature";
String humPath = "/humidity";
String gasPath = "/gas";
String flamePath = "/flame";
String soundPath = "/sound";
String pirPath = "/pir";
String timePath = "/timestamp";

// Sensor pins
#define DHTPIN 4       // DHT22 sensor pin
#define DHTTYPE DHT11  // DHT sensor type
#define PIRPIN 5       // PIR sensor pin
#define MQ2PIN 13      // MQ2 gas sensor pin (Analog)
#define FLAMEPIN 16    // Flame sensor digital pin
#define MICPIN 10      // Microphone digital pin

DHT dht(DHTPIN, DHTTYPE);

// ESP-NOW structure to send alert data
typedef struct struct_alert {
  bool flameDetected;
  bool takePhoto;
} struct_alert;

struct_alert alertToSend;  // Create an instance of the alert structure

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t alertReceiverAddress[] = {0x08, 0xd1, 0xf9, 0x97, 0x6d, 0x14};  // Replace with your receiver's MAC address

// Callback function to get the status of transmitted packets
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Timer variables
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;  // 10 seconds

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// Function to get the current time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 0;
  }
  time(&now);
  return now;
}

// Function to initialize sensors
void initSensors() {
  dht.begin();
  pinMode(PIRPIN, INPUT);
  pinMode(FLAMEPIN, INPUT);
  pinMode(MICPIN, INPUT);
}

// Initialize ESP-NOW
void initESPNow() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, alertReceiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

// Function to send alert to another ESP32 using ESP-NOW
void sendTeleAlert() {
  // Set alert details
  alertToSend.flameDetected = true;
  alertToSend.takePhoto = false;

  // Send alert data via ESP-NOW
  esp_err_t result = esp_now_send(alertReceiverAddress, (uint8_t *) &alertToSend, sizeof(alertToSend));

  if (result == ESP_OK) {
    Serial.println("Flame/Smoke alert sent successfully via ESP-NOW.");
  } else {
    Serial.println("Error sending flame/smoke alert via ESP-NOW.");
  }
}

void sendCamAlert() {
  // Set alert details
  alertToSend.flameDetected = false;
  alertToSend.takePhoto = true;

  // Send alert data via ESP-NOW
  esp_err_t result = esp_now_send(alertReceiverAddress, (uint8_t *) &alertToSend, sizeof(alertToSend));

  if (result == ESP_OK) {
    Serial.println("Photo alert sent successfully via ESP-NOW.");
  } else {
    Serial.println("Error sending photo alert via ESP-NOW.");
  }
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initESPNow();
  initSensors();

  configTime(0, 0, "pool.ntp.org");

  // Firebase configuration
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  // Assign smaller response size if possible
  fbdo.setResponseSize(256);

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  // Wait for user UID
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  // Set user UID and database path
  uid = auth.token.uid.c_str();
  databasePath = "/UsersData/" + uid + "/readings";
}

void loop() {
  // Send new readings every 10 seconds
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    int timestamp = getTime();
    String parentPath = databasePath + "/" + String(timestamp);

    // Read sensor data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int gasLevel = analogRead(MQ2PIN);          // Read gas level from MQ2
    int flameDetected = digitalRead(FLAMEPIN);  // Read flame sensor state
    int soundDetected = digitalRead(MICPIN);    // Read microphone state
    int motionDetected = digitalRead(PIRPIN);   // Read PIR sensor state

    // Log sensor data to Firebase
    FirebaseJson json;
    json.set(tempPath.c_str(), String(temperature));
    json.set(humPath.c_str(), String(humidity));
    json.set(gasPath.c_str(), String(gasLevel));
    json.set(flamePath.c_str(), flameDetected ? "Flame Detected" : "No Flame");
    json.set(soundPath.c_str(), soundDetected ? "Sound Detected" : "No Sound");
    json.set(pirPath.c_str(), motionDetected ? "Motion Detected" : "No Motion");
    json.set(timePath.c_str(), String(timestamp));

    // Send data to Firebase
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());

    // If motion is detected, send alert to ESP32-CAM
    if (motionDetected) {
      sendCamAlert();
      Serial.println("Motion detected, alert sent to ESP32-CAM.");
    }

    if (gasLevel > 2000 || flameDetected == 1) {
      sendTeleAlert();
      Serial.println("Flame or smoke detected! Please check the web app.");
    }
  }
}
