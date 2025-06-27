# Temperature-Initiated Object Detection Using TM4C123G

This project demonstrates a real-time embedded system for temperature-based object detection, developed using the **TM4C123G** microcontroller. It combines temperature sensing, object scanning, and user feedback through integrated sensors and actuators.

## 🔧 Project Description

The system enters **deep sleep mode** to conserve power, wakes upon detecting a **temperature threshold breach**, then scans the environment with an **ultrasonic sensor** on a stepper motor to detect nearby objects. Real-time feedback is provided via a **Nokia 5110 LCD**, **RGB LEDs**, and a **speaker**.

### ✨ Key Features

- **Temperature Monitoring**:
  - **LM35** (analog): Primary trigger via comparator
  - **BMP280** (digital via I2C): Secondary validation sensor

- **Object Detection**:
  - **HC-SR04 Ultrasonic Sensor** with **Stepper Motor** scan (-90° to +90°)

- **User Feedback**:
  - **Nokia 5110 LCD** via SPI
  - **RGB LEDs** indicate object proximity
  - **Speaker** alerts on high temperature

- **User Interaction**:
  - **4x4 Keypad** for threshold configuration
  - **Pushbuttons** to control system state
  - **Trimpot** for analog threshold setting

- **Power Efficiency**:
  - Deep sleep via analog comparator
  - Wake on LM35 threshold violation
  - External power and transistor drivers for high-current peripherals

## 🧠 Software Architecture

Structured using modular and interrupt-driven programming:

| Module           | Functionality                                      |
|------------------|----------------------------------------------------|
| `main.c`         | System initialization and logic control            |
| `lm35_control.h` | LM35 monitoring with comparator + sleep logic      |
| `bmp280.h`       | BMP280 initialization, filtering, temperature read |
| `DistanceSensor.h`| HC-SR04 pulse/echo and distance calculation       |
| `Stepper_Scan.h` | Stepper control and environment scanning           |
| `LedSpeaker.h`   | RGB LED + speaker feedback logic                   |
| `plot.h`         | LCD plotting logic for scanned objects             |
| `keypad.h`       | Keypad input handling                              |
| `Nokia5110.c/h`  | LCD display module (based on Valvano's driver)     |

All modules are integrated through interrupts (comparator, timer, GPIO, I2C) to ensure non-blocking, responsive system behavior.

## ⚙️ Hardware Overview

| Component        | Purpose                                 |
|------------------|------------------------------------------|
| TM4C123G         | Central microcontroller (Tiva C Series)  |
| LM35             | Analog temperature threshold detection   |
| BMP280           | Digital temperature validation (I2C)     |
| HC-SR04          | Distance sensing                         |
| Stepper Motor    | Rotates sensor for scanning area         |
| Nokia 5110 LCD   | Visual output via SPI                    |
| RGB LEDs         | Visual feedback on proximity             |
| Speaker          | Audible alert on temperature threshold   |
| 4x4 Keypad       | User input for threshold configuration   |
| Trimpot          | Analog threshold adjustment              |

Power LED and speaker are externally powered via NPN transistors and a 9V source.

## 📈 System Flow

1. **Sleep Mode** – System waits for LM35 to exceed threshold.
2. **Wakeup** – Comparator interrupt wakes TM4C123G.
3. **Validation** – BMP280 confirms high temperature.
4. **Scan** – Stepper rotates, ultrasonic sensor scans environment.
5. **Feedback** – Results shown on LCD, RGB LEDs blink, speaker alerts.
