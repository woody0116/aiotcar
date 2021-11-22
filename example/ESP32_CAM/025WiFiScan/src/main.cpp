#include <Arduino.h>
#include <WiFi.h>

#define DELAY_TIME 10000

unsigned long scanTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n");

    WiFi.mode(WIFI_MODE_STA);
}

void loop() {
    if(millis() > scanTime) {
        Serial.printf("---------------Start Scan---------------\n");

        int16_t  networkNum = WiFi.scanNetworks();
        Serial.printf("Number of Network: %d\n", networkNum);
        
        if(networkNum == 0) {
            Serial.printf("No Networks Found.\n");

        } else {
            for(size_t i = 0; i < networkNum; i++) {
                Serial.printf("ID: %d SSID: %s RSSI: %d Type: %d\n", i, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.encryptionType(i));
            }
        }
        Serial.printf("---------------End Scan---------------\n\n");
        scanTime = millis() + DELAY_TIME;
    }
}