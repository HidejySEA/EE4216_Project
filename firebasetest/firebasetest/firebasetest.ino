#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include "time.h"
#include <HTTPClient.h>

// ESP32-CAM server IP for taking photo
const char* serverName = "http://172.20.10.7/take_photo?trigger=EE4216";


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
#define DHTPIN 4         // DHT22 sensor pin
#define DHTTYPE DHT11    // DHT sensor type
#define PIRPIN 5         // PIR sensor pin
#define MQ2PIN 13        // MQ2 gas sensor pin (Analog)                                                                                                                                                                                                                                                                       
#define FLAMEPIN 16      // Flame sensor digital  pin
#define MICPIN 10      // Microphone  digital pin

DHT dht(DHTPIN, DHTTYPE);

// Timer variables
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;  // 3 minutes

// Firebase Cloud Messaging variables
String camModuleUrl = "http://<ESP32-CAM-IP>/capture";  // URL of the ESP32-CAM module for capturing photos

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
    return (0);
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

void sendTeleAlert() {

  HTTPClient http;
  http.begin(serverName);
  int httpResponseCode = http.GET();  // Send the request
  if (httpResponseCode > 0) {
    Serial.println("Flame/Smoke alert sent successfully.");
  } else {
    Serial.println("Error sending flame/smoke alert.");
  }
  http.end();
}

// Function to send alert to ESP32-CAM
void sendCamAlert() {

  HTTPClient http;
  http.begin(serverName);
  
  int httpResponseCode = http.GET();  // Send the request
  if (httpResponseCode > 0) {
    Serial.println("Photo taken successfully.");
  } else {
    Serial.println("Error taking photo.");
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initSensors();

  configTime(0, 0, "pool.ntp.org");

  // Firebase configuration
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  // Wait for user UID
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  // Set user UID and database path
  uid = auth.token.uid.c_str();
  databasePath = "/UsersData/" + uid + "/readings";
  
  // // Set PIR sensor pin as wake-up source and enter deep sleep
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_PIN, HIGH);  // Wake up on PIR HIGH signal
  // Serial.println("Entering deep sleep...");
  // delay(100);  // Allow some time for serial output to complete
  // esp_deep_sleep_start();  // Enter deep sleep mode
}

void loop() {
  // Send new readings every 3 minutes
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    int timestamp = getTime();
    String parentPath = databasePath + "/" + String(timestamp);

    // Read sensor data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int gasLevel = analogRead(MQ2PIN);  // Read gas level from MQ2
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
    json.set(timePath, String(timestamp));

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
