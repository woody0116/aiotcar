#include <Arduino.h>

#define FRONT_IN1   18
#define FRONT_IN2   2
#define FRONT_IN3   4
#define FRONT_IN4   16
#define BACK_IN1    13
#define BACK_IN2    25
#define BACK_IN3    27
#define BACK_IN4    26

#define MOTOR_PWM           80

#define FRONT_RIGHT_MOTOR_1_PWM ((MOTOR_PWM) + 0)
#define FRONT_RIGHT_MOTOR_2_PWM ((MOTOR_PWM) + 0)
#define FRONT_LEFT_MOTOR_1_PWM  ((MOTOR_PWM) + 0)
#define FRONT_LEFT_MOTOR_2_PWM  ((MOTOR_PWM) + 0)
#define BACK_RIGHT_MOTOR_1_PWM  ((MOTOR_PWM) + 0)
#define BACK_RIGHT_MOTOR_2_PWM  ((MOTOR_PWM) + 0)
#define BACK_LEFT_MOTOR_1_PWM   ((MOTOR_PWM) + 0)
#define BACK_LEFT_MOTOR_2_PWM   ((MOTOR_PWM) + 0)

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

void stop();

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n");
    Serial.printf("ESP32 Start\n");

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

void loop() {
    Serial.printf("FRONT RIGHT 1\n");
    ledcWrite(ledChannel1, FRONT_RIGHT_MOTOR_1_PWM);
    ledcWrite(ledChannel2, 0);
    delay(1000);
    Serial.printf("FRONT RIGHT 2\n");
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, FRONT_RIGHT_MOTOR_2_PWM);
    delay(1000);

    stop();

    Serial.printf("FRONT LEFT 1\n");
    ledcWrite(ledChannel3, FRONT_LEFT_MOTOR_1_PWM);
    ledcWrite(ledChannel4, 0);
    delay(1000);
    Serial.printf("FRONT LEFT 2\n");
    ledcWrite(ledChannel3, 0);
    ledcWrite(ledChannel4, FRONT_LEFT_MOTOR_2_PWM);
    delay(1000);

    stop();

    Serial.printf("BACK RIGHT 1\n");
    ledcWrite(ledChannel5, BACK_RIGHT_MOTOR_1_PWM);
    ledcWrite(ledChannel6, 0);
    delay(1000);
    Serial.printf("BACK RIGHT 2\n");
    ledcWrite(ledChannel5, 0);
    ledcWrite(ledChannel6, BACK_RIGHT_MOTOR_2_PWM);
    delay(1000);

    stop();

    Serial.printf("BACK LEFT 1\n");
    ledcWrite(ledChannel7, BACK_LEFT_MOTOR_1_PWM);
    ledcWrite(ledChannel8, 0);
    delay(1000);
    Serial.printf("BACK LEFT 2\n");
    ledcWrite(ledChannel7, 0);
    ledcWrite(ledChannel8, BACK_LEFT_MOTOR_2_PWM);
    delay(1000);

    stop();
}

void stop() {
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);
    ledcWrite(ledChannel3, 0);
    ledcWrite(ledChannel4, 0);
    ledcWrite(ledChannel5, 0);
    ledcWrite(ledChannel6, 0);
    ledcWrite(ledChannel7, 0);
    ledcWrite(ledChannel8, 0);
}