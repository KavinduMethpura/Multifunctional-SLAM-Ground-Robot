/*
 * SLAM Car Project - Servo Motor Test
 * Board  : ESP32 30-pin
 * Servo  : MG90S
 * Signal : D15
 *
 * Wiring:
 *   Brown  -> GND (shared with ESP32)
 *   Red    -> External 5V (NOT ESP32 5V pin)
 *   Yellow -> D15
 */
 
#include <Arduino.h>

// pins
#define SERVO_PIN 15
#define SERVO_CH 2 //LEDC channel (0,1 used by this)

#define SERVO_FREQ 50 //50 Hz - standard for servo
#define SERVO_RES 16 //16-bit for fine pulse control (0-65535)

//MG90S pulse width: 500us (0) to 240us (180)
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2400

// helper: microseconds -> LEDC duty
// At 50Hz, period = 20,000us
// duty = (pulse_us / 20000) * 65535
uint32_t usToDuty(int us){
  return (uint32_t)((us / 20000.0) * 65535);
}

//move servo to the angle

void setAngle(int angle) {
  angle = constrain(angle, 0, 180);
  int pulseUs = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  ledcWrite(SERVO_CH, usToDuty(pulseUs));
  Serial.printf(">> Servo -> %d°  (pulse: %dµs)\n", angle, pulseUs);
}

//setup
void setup(){
  Serial.begin(115200);
  Serial.println("\n=== SLAM Car - Servo Test ===");
 
  ledcSetup(SERVO_CH, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_PIN, SERVO_CH);
 
  // Center the servo on boot
  setAngle(90);
  delay(1000);
}

//loop
void loop(){
  //0 - 90 - 180 -90 - repeat

  setAngle(0);
  delay(1000);

  setAngle(90);
  delay(1000);

  setAngle(180);
  delay(1000);

  setAngle(90);
  delay(1000);
}