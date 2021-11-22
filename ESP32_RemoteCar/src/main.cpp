#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESP32GithubOTA.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

#define FRONT_IN1   18
#define FRONT_IN2   2
#define FRONT_IN3   4
#define FRONT_IN4   16
#define BACK_IN1    13
#define BACK_IN2    25
#define BACK_IN3    27
#define BACK_IN4    26

#define MOTOR_PWM   80

#define FRONT_RIGHT_MOTOR_1_PWM ((MOTOR_PWM) + 0)
#define FRONT_RIGHT_MOTOR_2_PWM ((MOTOR_PWM) + 0)
#define FRONT_LEFT_MOTOR_1_PWM  ((MOTOR_PWM) + 0)
#define FRONT_LEFT_MOTOR_2_PWM  ((MOTOR_PWM) + 0)
#define BACK_RIGHT_MOTOR_1_PWM  ((MOTOR_PWM) + 0)
#define BACK_RIGHT_MOTOR_2_PWM  ((MOTOR_PWM) + 0)
#define BACK_LEFT_MOTOR_1_PWM   ((MOTOR_PWM) + 0)
#define BACK_LEFT_MOTOR_2_PWM   ((MOTOR_PWM) + 0)

#define STOP        0x00
#define UP          0x01
#define DOWN        0x02
#define LEFT        0x03
#define RIGHT       0x04
#define UP_LEFT     0x05
#define UP_RIGHT    0x06
#define DOWN_LEFT   0x07
#define DOWN_RIGHT  0x08
#define TURN_LEFT   0x09
#define TURN_RIGHT  0x0A

#define BACKWARD    0x00
#define FORWARD     0x01

#define ESP32_DNS   "myesp32"
#define SSID        SSID
#define PASSWD      PASSWD
#define ESP32_VER   "1.0"

#define UPDATE_URL          UPDATE_URL
#define SPIFFS_BIN_URL      SPIFFS_BIN_URL
#define FIRMWARE_BIN_URL    FIRMWARE_BIN_URL
#define GITHUB_TOKEN_TOKEN  GITHUB_TOKEN_TOKEN
#define GITHUB_TOKEN        GITHUB_TOKEN

const int freq        = 5000;
const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;
const int ledChannel4 = 3;
const int ledChannel5 = 4;
const int ledChannel6 = 5;
const int ledChannel7 = 6;
const int ledChannel8 = 7;
const int resolution  = 8;

AsyncWebServer server(80);
AsyncWebSocket ws    ("/ws");

ESP32GithubOTA githubOTA;

static void setPinMode();
static void processCarMovement(int);
static void rotateMotor(int);
static void checkUpdate(const char *, const char *);
String getSpiffsVer();

void initWebSocket(void);
void handleWebSocketMssage( void    *, 
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

    setPinMode();

    if (!SPIFFS.begin(true)) {
        Serial.println(F("An Error has occurred while mounting SPIFFS"));
        for(;;);
    }

    String spiffsVer = getSpiffsVer();
    Serial.printf("ESP32  ver: %s\n", ESP32_VER);
    Serial.printf("Spiffs ver: %s\n", spiffsVer.c_str());

    Serial.printf("Connect to %s...", SSID);
    WiFi.begin(SSID, PASSWD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.printf("\n");
    Serial.printf("ESP32 IP: %s\n", WiFi.localIP().toString().c_str());

    if (!MDNS.begin("esp32")) {
        Serial.printf("Error setting up MDNS responder!\n");
        for(;;);
    }
    Serial.printf("ESP32 mDNS started: %s.local\n", ESP32_DNS);

    checkUpdate(spiffsVer.c_str(), ESP32_VER);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
    });

    server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/main.js");
    });

    initWebSocket();
    server.begin();

    Serial.printf("ESP32: Enter Loop.\n");
    delay(1000);
    
}

void loop() {
    ws.cleanupClients();
}

static void setPinMode() {
    ledcSetup(ledChannel1, freq, resolution);
    ledcSetup(ledChannel2, freq, resolution);
    ledcSetup(ledChannel3, freq, resolution);
    ledcSetup(ledChannel4, freq, resolution);
    ledcSetup(ledChannel5, freq, resolution);
    ledcSetup(ledChannel6, freq, resolution);
    ledcSetup(ledChannel7, freq, resolution);
    ledcSetup(ledChannel8, freq, resolution);

    //Motor1 front right
    ledcAttachPin(FRONT_IN1, ledChannel1);     //Front L298N - IN1
    ledcAttachPin(FRONT_IN2, ledChannel2);     //Front L298N - IN2
    
    //Motor2 front left
    ledcAttachPin(FRONT_IN3, ledChannel3);     //Front L298N - IN3
    ledcAttachPin(FRONT_IN4, ledChannel4);     //Front L298N - IN4

    //Motor3 back right
    ledcAttachPin(BACK_IN1,  ledChannel5);     //Back L298N - IN1
    ledcAttachPin(BACK_IN2,  ledChannel6);     //Back L298N - IN2

    //Motor4 back left
    ledcAttachPin(BACK_IN3,  ledChannel7);     //Back L298N - IN3
    ledcAttachPin(BACK_IN4,  ledChannel8);     //Back L298N - IN4
}

static void processCarMovement(int value) {
    switch(value) {
        case UP:
            rotateMotor(0b01011010); //BL.BR.FR.FL
            break;
        
        case DOWN:
            rotateMotor(0b10100101);
            break;

        case LEFT:
            rotateMotor(0b01101001);
            break;

        case RIGHT:
            rotateMotor(0b10010110);
            break;

        case UP_LEFT:
            rotateMotor(0b01001000);
            break;

        case UP_RIGHT:
            rotateMotor(0b00010010); 
            break;

        case DOWN_LEFT:
            rotateMotor(0b00100001);
            break;

        case DOWN_RIGHT:
            rotateMotor(0b10000100);
            break;

        case TURN_LEFT:
            rotateMotor(0b10011001);
            break;

        case TURN_RIGHT:
            rotateMotor(0b01100110);
            break;

        case STOP:
        default:
            rotateMotor(0b00000000);
            break;
    }
}

static void rotateMotor(int value) {
    ledcWrite(ledChannel1, (value & 1)? FRONT_RIGHT_MOTOR_1_PWM : 0);
    printf("value: %d |",  (value & 1)? FRONT_RIGHT_MOTOR_1_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel2, (value & 1)? FRONT_RIGHT_MOTOR_2_PWM : 0);
    printf(" %d |",        (value & 1)? FRONT_RIGHT_MOTOR_2_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel3, (value & 1)? FRONT_LEFT_MOTOR_1_PWM : 0);
    printf(" %d |",        (value & 1)? FRONT_LEFT_MOTOR_2_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel4, (value & 1)? FRONT_LEFT_MOTOR_2_PWM : 0);
    printf(" %d |",        (value & 1)? FRONT_LEFT_MOTOR_2_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel5, (value & 1)? BACK_RIGHT_MOTOR_1_PWM : 0);
    printf(" %d |",        (value & 1)? BACK_RIGHT_MOTOR_1_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel6, (value & 1)? BACK_RIGHT_MOTOR_2_PWM : 0);
    printf(" %d |",        (value & 1)? BACK_RIGHT_MOTOR_2_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel7, (value & 1)? BACK_LEFT_MOTOR_1_PWM : 0);
    printf(" %d |",        (value & 1)? BACK_LEFT_MOTOR_1_PWM : 0);
    value = value >> 1;

    ledcWrite(ledChannel8, (value & 1)? BACK_LEFT_MOTOR_2_PWM : 0);
    printf(" %d\n",        (value & 1)? BACK_LEFT_MOTOR_2_PWM : 0);
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
        String str2 = str;
        processCarMovement(str2.toInt());
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

String getSpiffsVer() {
    String str;
    File file = SPIFFS.open("/spiffs.txt");
    if(file){
        Serial.printf("Reading File...\n");
        str = file.readString();
    }else{
        Serial.printf("Read File Error!\n");
        str = "Error";
    }
    file.close();

    return str;
}

static void checkUpdate(const char *spiffsVer, const char *firmwareVer) {
    HTTPClient http;
    String githubVer;
    http.begin(UPDATE_URL);
    http.addHeader("Authorization", GITHUB_TOKEN_TOKEN);

    int httpCode = http.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            githubVer = http.getString();
            Serial.printf("GitHub Ver: %s\n", githubVer.c_str());
        } else {
            Serial.printf("Error Code: %d\n", httpCode);
            return;
        }
    } else {
        Serial.printf("Error Code: %d\n", httpCode);
        return;
    }

    if(githubVer.indexOf(spiffsVer) != 0) {
        Serial.printf("Spiffs update start![ %s -> %s ]\n", spiffsVer, githubVer.c_str());
        if(!githubOTA.spiffsOTA(SPIFFS_BIN_URL, GITHUB_TOKEN)) {
            Serial.printf("Error Code: %d\n", githubOTA.getErrorCode());
            return;
        }
    } else if (githubVer.indexOf(firmwareVer) != 0) {
        Serial.printf("Firmware update start![ %s -> %s ]\n", firmwareVer, githubVer.c_str());
        if(!githubOTA.firmwareOTA(FIRMWARE_BIN_URL, GITHUB_TOKEN)) {
            Serial.printf("Error Code: %d\n", githubOTA.getErrorCode());
            return;
        }
    }

    Serial.printf("Firmware and SPIFFS ver is latest version![%s]\n", githubVer.c_str());
    http.end();
}
