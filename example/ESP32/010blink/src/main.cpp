#include <Arduino.h>

#define LED_PIN    2
#define DELAY_TIME 1000

bool blinkBool = true;
unsigned long blinkTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n");

    Serial.printf("Hello ESP32!\n");
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    if(millis() > blinkTime) {
        digitalWrite(LED_PIN, blinkBool);
        Serial.printf("LED PIN: %s\n", blinkBool? "ON" : "OFF");
        blinkBool = !blinkBool;
        blinkTime = millis() + DELAY_TIME;
    }
}