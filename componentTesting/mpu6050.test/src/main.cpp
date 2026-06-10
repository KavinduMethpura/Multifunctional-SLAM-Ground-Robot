/*
 * SLAM Car Project - MPU6050 IMU Test
 * With I2C bus recovery on lockup
 */

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

#define SDA_PIN 21
#define SCL_PIN 22

MPU6050 mpu;

// ─── I2C Bus Recovery ──────────────────────────────────────────────
// Sends up to 9 clock pulses to unstick a frozen I2C bus,
// then issues a STOP condition to reset both devices.
void recoverI2C() {
  Serial.println("!! I2C lockup detected — recovering...");

  Wire.end();
  delay(50);

  pinMode(SDA_PIN, OUTPUT);
  pinMode(SCL_PIN, OUTPUT);

  digitalWrite(SDA_PIN, HIGH);
  digitalWrite(SCL_PIN, HIGH);

  // Toggle SCL up to 9 times to release stuck slave
  for (int i = 0; i < 9; i++) {
    if (digitalRead(SDA_PIN) == HIGH) break; // SDA free, stop early
    digitalWrite(SCL_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(SCL_PIN, HIGH);
    delayMicroseconds(5);
  }

  // Issue STOP condition: SDA LOW→HIGH while SCL HIGH
  digitalWrite(SDA_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(SCL_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SDA_PIN, HIGH);
  delayMicroseconds(5);

  delay(50);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  // Re-init sensor
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println(">> I2C recovered! MPU6050 back online.");
  } else {
    Serial.println(">> Recovery failed — check wiring.");
  }
}

// ─── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== SLAM Car - MPU6050 IMU Test ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("ERROR: MPU6050 not found!");
    while (1);
  }

  Serial.println("MPU6050 connected!");
  Serial.println("──────────────────────────────────────────────────────");
  Serial.println("        Accel (m/s²)           Gyro (°/s)");
  Serial.println("   aX      aY      aZ      gX      gY      gZ");
  Serial.println("──────────────────────────────────────────────────────");
}

// ─── Main Loop ─────────────────────────────────────────────────────
void loop() {
  static uint8_t errorCount = 0;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Detect all-zero lockup (real sensor never gives perfect zeros)
  bool allZero = (ax == 0 && ay == 0 && az == 0 &&
                  gx == 0 && gy == 0 && gz == 0);

  if (allZero) {
    errorCount++;
    if (errorCount >= 3) {   // 3 consecutive zero readings = locked up
      recoverI2C();
      errorCount = 0;
      delay(200);
      return;
    }
  } else {
    errorCount = 0;  // reset on good reading
  }

  // Convert raw values
  float accelX = (ax / 16384.0) * 9.81;
  float accelY = (ay / 16384.0) * 9.81;
  float accelZ = (az / 16384.0) * 9.81;
  float gyroX  = gx / 131.0;
  float gyroY  = gy / 131.0;
  float gyroZ  = gz / 131.0;

  Serial.printf("%7.2f %7.2f %7.2f   %7.2f %7.2f %7.2f\n",
                accelX, accelY, accelZ,
                gyroX,  gyroY,  gyroZ);

  delay(200);
}