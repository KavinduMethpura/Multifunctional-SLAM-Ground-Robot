// /*
//  * SLAM Car Project - I2C Bus Scanner
//  * Scans all 127 addresses and reports found devices
//  *
//  * Expected results:
//  *   0x29 → VL53L0X
//  *   0x68 → MPU6050
//  */

// #include <Arduino.h>
// #include <Wire.h>

// #define SDA_PIN 21
// #define SCL_PIN 22

// void setup() {
//   Serial.begin(115200);
//   Serial.println("\n=== SLAM Car - I2C Scanner ===");

//   Wire.begin(SDA_PIN, SCL_PIN);
//   delay(100);

//   Serial.println("Scanning I2C bus...");
//   Serial.println("──────────────────────────");

//   uint8_t found = 0;

//   for (uint8_t addr = 1; addr < 127; addr++) {
//     Wire.beginTransmission(addr);
//     uint8_t error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.printf("Found device at 0x%02X", addr);

//       // Identify known devices
//       if      (addr == 0x29) Serial.print("  → VL53L0X (ToF sensor)");
//       else if (addr == 0x68) Serial.print("  → MPU6050 (IMU) AD0=LOW");
//       else if (addr == 0x69) Serial.print("  → MPU6050 (IMU) AD0=HIGH");
//       else                   Serial.print("  → Unknown device");

//       Serial.println();
//       found++;
//     }
//   }

//   Serial.println("──────────────────────────");
//   if (found == 0) {
//     Serial.println("No devices found! Check wiring.");
//   } else {
//     Serial.printf("Scan complete. %d device(s) found.\n", found);
//   }
// }

// void loop() {
//   // Nothing — scanner only runs once in setup
// }


/*
 * SLAM Car Project - I2C Combined Sensor Test
 * MPU6050 (0x68) + VL53L0X (0x29) on same I2C bus
 *
 * Wiring (both sensors share same lines):
 *   SDA -> D21
 *   SCL -> D22
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <VL53L0X.h>

#define SDA_PIN 21
#define SCL_PIN 22

MPU6050 mpu;
VL53L0X tof;

bool mpuOK = false;
bool tofOK = false;

// ─── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== SLAM Car - Combined I2C Sensor Test ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  // ── Init MPU6050 ──
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 OK  (0x68)");
    mpuOK = true;
  } else {
    Serial.println("MPU6050 FAIL — check wiring!");
  }

  // ── Init VL53L0X ──
  if (tof.init()) {
    tof.setTimeout(500);
    tof.setMeasurementTimingBudget(50000); // 50ms — balanced speed/accuracy
    tof.startContinuous();
    Serial.println("VL53L0X OK  (0x29)");
    tofOK = true;
  } else {
    Serial.println("VL53L0X FAIL — check wiring!");
  }

  if (!mpuOK || !tofOK) {
    Serial.println("\nOne or more sensors failed. Fix before continuing.");
    Serial.println("Run I2C scanner to verify addresses.");
  }

  Serial.println("───────────────────────────────────────────────────────────────");
  Serial.println("  Dist(mm) |    aX      aY      aZ   |    gX      gY      gZ");
  Serial.println("───────────────────────────────────────────────────────────────");
}

// ─── Main Loop ─────────────────────────────────────────────────────
void loop() {
  // ── Read MPU6050 ──
  float accelX = 0, accelY = 0, accelZ = 0;
  float gyroX  = 0, gyroY  = 0, gyroZ  = 0;

  if (mpuOK) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    accelX = (ax / 16384.0) * 9.81;
    accelY = (ay / 16384.0) * 9.81;
    accelZ = (az / 16384.0) * 9.81;
    gyroX  = gx / 131.0;
    gyroY  = gy / 131.0;
    gyroZ  = gz / 131.0;
  }

  // ── Read VL53L0X ──
  uint16_t distMm = 0;
  String distStr = "  ----  ";

  if (tofOK) {
    distMm = tof.readRangeContinuousMillimeters();
    if (tof.timeoutOccurred() || distMm >= 8190) {
      distStr = " OORange";
    } else {
      distStr = String(distMm) + "mm";
      // Pad for alignment
      while (distStr.length() < 8) distStr = " " + distStr;
    }
  }

  // ── Print combined output ──
  Serial.printf("%s | %7.2f %7.2f %7.2f | %7.2f %7.2f %7.2f\n",
                distStr.c_str(),
                accelX, accelY, accelZ,
                gyroX,  gyroY,  gyroZ);

  delay(200);
}