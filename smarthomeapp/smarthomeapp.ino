#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "esp_camera.h"
#include <time.h>  // For time functions
#include <addons/RTDBHelper.h>  

#define WIFI_SSID "OPPO A3 Pro 5G"
#define WIFI_PASSWORD "n82yq7cm"

#define FIREBASE_PROJECT_ID "smarthomesecurity-60a6e"
#define FIREBASE_URL "https://smarthomesecurity-60a6e-default-rtdb.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyDzOeSdCzdS5RW4SoygAkTnHWURMAeFsuU"
#define FIREBASE_STORAGE_BUCKET "smarthomesecurity-60a6e.firebasestorage.app"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

void setupTime() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {  // 15 seconds timeout
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFailed to connect to WiFi! Restarting...");
        ESP.restart();  // Restart ESP32 if connection fails
    }

    Serial.println("\nConnected to WiFi");
    Serial.println(WiFi.localIP());  // Print IP address



    config.api_key = FIREBASE_API_KEY;
    config.database_url = FIREBASE_URL;
    auth.user.email = "vedupachchigar@gmail.com";
    auth.user.password = "Vedant123";

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    setupTime();  // Initialize NTP time

    camera_config_t cameraConfig;
    cameraConfig.ledc_channel = LEDC_CHANNEL_0;
    cameraConfig.ledc_timer = LEDC_TIMER_0;
    cameraConfig.pin_d0 = 5;
    cameraConfig.pin_d1 = 18;
    cameraConfig.pin_d2 = 19;
    cameraConfig.pin_d3 = 21;
    cameraConfig.pin_d4 = 36;
    cameraConfig.pin_d5 = 39;
    cameraConfig.pin_d6 = 34;
    cameraConfig.pin_d7 = 35;
    cameraConfig.pin_xclk = 0;
    cameraConfig.pin_pclk = 22;
    cameraConfig.pin_vsync = 25;
    cameraConfig.pin_href = 23;
    cameraConfig.pin_sscb_sda = 26;
    cameraConfig.pin_sscb_scl = 27;
    cameraConfig.pin_pwdn = 32;
    cameraConfig.pin_reset = -1;
    cameraConfig.xclk_freq_hz = 20000000;
    cameraConfig.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        cameraConfig.frame_size = FRAMESIZE_SXGA;
        cameraConfig.jpeg_quality = 8;
        cameraConfig.fb_count = 2;
    } else {
        cameraConfig.frame_size = FRAMESIZE_SXGA;
        cameraConfig.jpeg_quality = 10;
        cameraConfig.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&cameraConfig);
    if (err != ESP_OK) {
        Serial.println("Camera init failed!");
        return;
    }
    Serial.println("Camera initialized successfully.");
}

void loop() {
    if (Serial.available()) {  
        char command = Serial.read();
        if (command == 'C') {  
            Serial.println("Capturing Image...");
            captureAndUpload();
            Serial.println("DONE");
        }
    }
}

void captureAndUpload() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed!");
        return;
    }

    Serial.println("Uploading to Firebase Storage...");

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

    String imagePath = "images/capture_" + String(millis()) + ".jpg";
    String contentType = "image/jpeg";

    if (Firebase.Storage.upload(&fbdo, FIREBASE_STORAGE_BUCKET, fb->buf, fb->len, imagePath, contentType)) {
        Serial.println("Upload Success");
        String imageUrl = fbdo.downloadURL();
        Serial.println("Image URL: " + imageUrl);

        Firebase.RTDB.setString(&fbdo, "/images/" + imagePath + "/url", imageUrl);
        Firebase.RTDB.setString(&fbdo, "/images/" + imagePath + "/timestamp", timestamp);
    } else {
        Serial.println("Upload Failed: " + fbdo.errorReason());
    }

    esp_camera_fb_return(fb);
}