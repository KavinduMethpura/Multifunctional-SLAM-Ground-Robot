/*
 * SLAM Car Project - ToF Distance Sensor Test
 * Board  : ESP32 30-pin
 * Sensor : VL53L0X V2
 *
 * Wiring:
 *   VIN   -> 3.3V
 *   GND   -> GND
 *   SDA   -> D21
 *   SCL   -> D22
 */

#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup(){
  Serial.begin(115200);
  Serial.println("\n=== SLAM Car - ToF Sensor Test ===");

  Wire.begin(21, 22);

  if (!sensor.init()) {
    Serial.println("ERROR: VL53L0X not found!");
    Serial.println("Check wiring — SDA->D21, SCL->D22, VIN->3.3V only!");
    while (1);
  }

  sensor.setTimeout(500);

  //long range mode-up to ~2m
  sensor.setSignalRateLimit(0.1);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 10);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  sensor.setMeasurementTimingBudget(200000); // 200ms

  sensor.startContinuous();

  Serial.println("Sensor ready!");
  Serial.println("─────────────────────────────");
}

void loop() {
  uint16_t distanceMm = sensor.readRangeContinuousMillimeters();

  if (sensor.timeoutOccurred()) {
    Serial.println("TIMEOUT — check sensor connection");
    return;
  }

  if (distanceMm >= 8190) {
    Serial.println("Distance: OUT OF RANGE (> 2000mm)");
  } else {
    float distanceCm = distanceMm / 10.0;
    Serial.printf("Distance: %4u mm  |  %.1f cm", distanceMm, distanceCm);

    if (distanceMm < 150) {
      Serial.print("  !! OBSTACLE CLOSE!");
    } else if (distanceMm < 300) {
      Serial.print("  -> Approaching");
    }

    Serial.println();
  }

  delay(100);
}