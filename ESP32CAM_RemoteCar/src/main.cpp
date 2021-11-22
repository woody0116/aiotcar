#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"
#include "SPIFFS.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_PIN 4

#define SSID        SSID
#define PASSWD      PASSWD
char* mydns="myip";

AsyncWebServer server(80);
AsyncWebSocket wsCamera("/Camera");

uint32_t cameraClientId = 0;

void setupCamera();
void handleCameraWebSocketMessage(void    *, 
                                  uint8_t *, 
                                  size_t);
void onCameraWebSocketEvent(AsyncWebSocket*, 
                            AsyncWebSocketClient*, 
                            AwsEventType,
                            void*,
                            uint8_t*, 
                            size_t);

void sendCameraPicture();

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n");
    Serial.printf("ESP32 Start\n");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    if (!SPIFFS.begin(true)) {
        Serial.println(F("An Error has occurred while mounting SPIFFS"));
        for(;;);
    }

    Serial.printf("Connect to %s...", SSID);
    WiFi.begin(SSID, PASSWD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.printf(".");
    }

    Serial.printf("\n");
    Serial.printf("ESP32 IP: %s\n", WiFi.localIP().toString().c_str());

    //use IP or myip.local to access webserver
    if (MDNS.begin(mydns)) {
        Serial.println("MDNS responder started");
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
    });

    server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/main.js");
    });

    wsCamera.onEvent(onCameraWebSocketEvent);
    server.addHandler(&wsCamera);

    setupCamera();
    server.begin();
}

void loop() {
    wsCamera.cleanupClients();
    sendCameraPicture();
    delay(150);
}

void setupCamera() {
    camera_config_t config;
    config.ledc_channel   = LEDC_CHANNEL_0;
    config.ledc_timer     = LEDC_TIMER_0;
    config.pin_d0         = Y2_GPIO_NUM;
    config.pin_d1         = Y3_GPIO_NUM;
    config.pin_d2         = Y4_GPIO_NUM;
    config.pin_d3         = Y5_GPIO_NUM;
    config.pin_d4         = Y6_GPIO_NUM;
    config.pin_d5         = Y7_GPIO_NUM;
    config.pin_d6         = Y8_GPIO_NUM;
    config.pin_d7         = Y9_GPIO_NUM;
    config.pin_xclk       = XCLK_GPIO_NUM;
    config.pin_pclk       = PCLK_GPIO_NUM;
    config.pin_vsync      = VSYNC_GPIO_NUM;
    config.pin_href       = HREF_GPIO_NUM;
    config.pin_sscb_sda   = SIOD_GPIO_NUM;
    config.pin_sscb_scl   = SIOC_GPIO_NUM;
    config.pin_pwdn       = PWDN_GPIO_NUM;
    config.pin_reset      = RESET_GPIO_NUM;
    config.xclk_freq_hz   = 20000000;
    config.pixel_format   = PIXFORMAT_JPEG;
    
    config.frame_size     = FRAMESIZE_VGA;
    config.jpeg_quality   = 10;
    config.fb_count       = 1;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }  

    if (psramFound()) {
        heap_caps_malloc_extmem_enable(20000);  
        Serial.printf("PSRAM initialized. malloc to take memory from psram above this size");    
    }  
}

void onCameraWebSocketEvent(AsyncWebSocket       *server, 
                            AsyncWebSocketClient *client, 
                            AwsEventType          type,
                            void                 *arg, 
                            uint8_t              *data, 
                            size_t                len) {                      
    switch(type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            cameraClientId = client->id();
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            cameraClientId = 0;
            break;
        case WS_EVT_DATA:
            handleCameraWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
        default:
            break;
    }
}

void sendCameraPicture() {
    if(cameraClientId == 0)
        return;

    unsigned long  startTime1 = millis();

    //capture a frame
    camera_fb_t * fb = esp_camera_fb_get();
    if(!fb) {
        Serial.println("Frame buffer could not be acquired");
        return;
    }

    unsigned long  startTime2 = millis();
    wsCamera.binary(cameraClientId, fb->buf, fb->len);
    esp_camera_fb_return(fb);
      
    //Wait for message to be delivered
    while (true) {
        AsyncWebSocketClient * clientPointer = wsCamera.client(cameraClientId);
        if(!clientPointer || !(clientPointer->queueIsFull()))
            break;
        delay(1);
    }
    
    unsigned long  startTime3 = millis();  
    Serial.printf("Time taken Total: %d|%d|%d\n",startTime3 - startTime1, startTime2 - startTime1, startTime3 - startTime2 );
}

void handleCameraWebSocketMessage(void    *arg,
                                  uint8_t *data,
                                  size_t   len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        char *str = (char *)data;
        Serial.printf("websocket: %s\n", str);

        if(strncmp(str, "ON", len) == 0) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }
    }
}
