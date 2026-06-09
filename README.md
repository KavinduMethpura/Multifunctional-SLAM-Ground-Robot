# Multifunctional SLAM Ground Robot

An autonomous ground robot designed to perform **Simultaneous Localization and Mapping (SLAM)** and navigation in unknown environments.

---

## 📌 Project Overview
This project focuses on building an autonomous ground robot capable of exploring unknown environments, mapping them in real-time, and navigating successfully. Starting from a known initial point, the robot uses various sensors to build an internal map of its surroundings while simultaneously keeping track of its own position.

---

## 🛠️ Hardware Components

The system is controlled using an **ESP32** microcontroller and incorporates the following hardware:

*   **Microcontroller:** ESP32 (30-pin development board)
*   **Locomotion:** Two DC motors driven by a compatible motor driver.
*   **Odometry:** Two rotary encoders to track motor rotations and calculate wheel odometry (distance traveled).
*   **Mapping & Obstacle Detection:** A servo-mounted scanner housing **two Time-of-Flight (ToF) sensors** to measure distances and scan the environment.
*   **Navigation & Orientation:** An **MPU-6050** Inertial Measurement Unit (IMU) to measure angular velocity and acceleration, ensuring accurate heading control.

---

## 🚀 Key Features
*   **Autonomous Navigation:** Explores unknown environments autonomously starting from a reference point.
*   **Real-time SLAM:** Uses ToF sensor sweeps combined with wheel odometry and IMU heading data to construct maps.
*   **Active Scanning:** The servo motor rotates the ToF sensors to scan a wider field of view without needing to rotate the entire robot.