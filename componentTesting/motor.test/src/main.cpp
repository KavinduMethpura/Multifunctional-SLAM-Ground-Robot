/*
 * SLAM Car Project - Motor Driver Test
 * Board: ESP32 30-pin
 * Driver: L298N
 * Power: 8V to motor driver
 *
 * Pin Mapping:
 *   ENA  -> D18 (PWM speed control - Motor A)
 *   ENB  -> D23 (PWM speed control - Motor B)
 *   IN1  -> D14 (Motor A direction)
 *   IN2  -> D5  (Motor A direction)
 *   IN3  -> D2  (Motor B direction)
 *   IN4  -> D4  (Motor B direction)
 */

#include <Arduino.h>

// pin definition
#define ENA 18
#define ENB 23
#define IN1 14
#define IN2 5
#define IN3 2
#define IN4 4

// PWM configs
#define PWM_FREQ 1000    //1 kHz
#define PWM_RESOLUTION 8 //8-bit -> 0-255
#define PWM_CH_A 0       // LEDC channel for motor A
#define PWM_CH_B 1       //LEDC channel for motor B

// Speed settings
#define MOTOR_SPEED 200 // 0-255
#define MOVE_DELAY 2000 // ms each direction holds

// helper functions
void setSpeed(int speedA, int speedB){
    ledcWrite(PWM_CH_A, speedA);
    ledcWrite(PWM_CH_B, speedB);
}

void stopMotors(){
    // Break: all direction pins LOW, PWM can stay or go to 0
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    setSpeed(0, 0);
    Serial.println(">> STOP");
}

void moveForward(int speed){
    // Motor A forward: IN1=HIGH, IN2=LOW
    // Motor B forward: IN3=HIGH, IN4=LOW
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    setSpeed(speed, speed);
    Serial.printf(">> FORWARD  (speed=%d)\n", speed);
}

void moveBackward(int speed){
    // Motor A forward: IN1=HIGH, IN2=LOW
    // Motor B forward: IN3=HIGH, IN4=LOW
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    setSpeed(speed, speed);
    Serial.printf(">> BACKWARD (speed=%d)\n", speed);
}

// Optional extras — useful for future SLAM testing
void turnLeft(int speed) {
  // Left motor backward, right motor forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  setSpeed(speed, speed);
  Serial.printf(">> TURN LEFT (speed=%d)\n", speed);
}
 
void turnRight(int speed) {
  // Left motor forward, right motor backward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  setSpeed(speed, speed);
  Serial.printf(">> TURN RIGHT (speed=%d)\n", speed);
}

//-----------------------setup---------------------
void setup(){
    Serial.begin(115200);
    Serial.println("\n=== SLAM Car - Motor Driver Test ===");

    // Direction pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Configure LEDC PWM channels (ESP32 Arduino core v2.x style)
    ledcSetup(PWM_CH_A, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CH_B, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(ENA, PWM_CH_A);
    ledcAttachPin(ENB, PWM_CH_B);

    stopMotors();
    delay(1000);
}


// main loop
void loop(){
    // forward
    moveForward(MOTOR_SPEED);
    delay(MOVE_DELAY);

    // stop
    stopMotors();
    delay(500);

    // backward
    moveBackward(MOTOR_SPEED);
    delay(MOVE_DELAY);

    // stop
    stopMotors();
    delay(500);
}
