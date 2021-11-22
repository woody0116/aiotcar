#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "SPIFFS.h"

#define LED_PIN 2

#define SSID   SSID
#define PASSWD PASSWD

AsyncWebServer server(80);
AsyncWebSocket ws    ("/ws");

void initWebSocket(void);
void handleWebSocketMessage(void    *, 
                            uint8_t *, 
                            size_t);
void onEvent(AsyncWebSocket         *, 
             AsyncWebSocketClient   *, 
             AwsEventType,
             void                   *, 
             uint8_t                *, 
             size_t);

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n");
    Serial.printf("ESP32 Start\n");

    pinMode(LED_PIN, OUTPUT);

    if (!SPIFFS.begin(true)) {
        Serial.println(F("An Error has occurred while mounting SPIFFS"));
        for(;;);
    }

    Serial.printf("Connect to %s...", SSID);
    WiFi.begin(SSID, PASSWD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.printf("\n");
    Serial.printf("ESP32 IP: %s\n", WiFi.localIP().toString().c_str());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
    });

    server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/main.js");
    });

    server.on("/style", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/style.css");
    });


    initWebSocket();
    server.begin();
}

void loop() {
    ws.cleanupClients();
}

void initWebSocket() {
    ws.    onEvent(onEvent);
    server.addHandler(&ws);
    delay(500);
}

void handleWebSocketMessage(void    *arg,
                            uint8_t *data,
                            size_t   len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        char *str = (char *)data;
        Serial.printf("websocket: %s\n", str);

        if (strncmp(str, "ON", len) == 0) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }
    }
}

void onEvent(AsyncWebSocket       *server, 
             AsyncWebSocketClient *client, 
             AwsEventType          type,
             void                 *arg, 
             uint8_t              *data, 
             size_t                len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}
