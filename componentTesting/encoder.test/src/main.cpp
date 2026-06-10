/*
 * SLAM Car Project - Wheel Encoder Test
 * Board  : ESP32 30-pin
 * Sensor : MD0369 IR Speed Sensor (U-Type Slot)
 * D0     -> D26
 *
 * Wiring:
 *   VCC -> 3.3V or 5V
 *   GND -> GND
 *   D0  -> D26
 *   A0  -> Not connected (not needed for counting)
 *
 * How it works:
 *   Each slot in the encoder disc breaks the IR beam → triggers an interrupt
 *   Count pulses → calculate RPM and distance
 */

#include <Arduino.h>

// pin
#define ENCODER_PIN 26

// Encoder Config
#define SLOTS_PER_REV 20
#define WHEEL_DIAMETER_CM 2.5
#define WHEEL_CIRCUMFERENCE_CM  (WHEEL_DIAMETER_CM * 3.14159)

// volatile var (modified inside ISR)
volatile uint32_t pulseCount = 0;

// ISR
// called everytime the beam is broken (FAILING = HIGH -> LOW transition)
void IRAM_ATTR onPulse(){
  pulseCount++;
}

//setup
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== SLAM Car - Wheel Encoder Test ===");
 
  pinMode(ENCODER_PIN, INPUT);
 
  // Attach interrupt — FALLING edge fires when disc slot breaks the beam
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), onPulse, FALLING);
 
  Serial.println("Rotate the wheel by hand or run the motor...");
  Serial.println("─────────────────────────────────────────");
}

//loop
void loop(){
  static uint32_t lastCount = 0;
  static uint32_t lastTime = 0;

  uint32_t now = millis();

  // Print stats every 500ms
  if (now - lastTime >= 500) {
    // Snapshot pulse count safely
    noInterrupts();
    uint32_t currentCount = pulseCount;
    interrupts();
 
    uint32_t deltaPulses = currentCount - lastCount;
    float deltaTimeSec   = (now - lastTime) / 1000.0;
 
    // RPM = (pulses in interval / slots per rev) / time * 60
    float revolutions = (float)deltaPulses / SLOTS_PER_REV;
    float rpm         = (revolutions / deltaTimeSec) * 60.0;
 
    // Distance = total revolutions * circumference
    float totalRevs     = (float)currentCount / SLOTS_PER_REV;
    float distanceCm    = totalRevs * WHEEL_CIRCUMFERENCE_CM;
 
    Serial.printf("Pulses: %4u  |  RPM: %6.1f  |  Distance: %.2f cm  |  Total pulses: %u\n",
                  deltaPulses, rpm, distanceCm, currentCount);
 
    lastCount = currentCount;
    lastTime  = now;
  } 
}