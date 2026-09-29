# RiderShield – Smart Helmet Safety System

RiderShield is an **ESP32-based smart helmet safety system** designed to improve rider safety by detecting helmet usage, identifying possible accidents, tracking the rider's location, and sending emergency alerts.

## Features

* 🪖 **Helmet Detection** using Hall Effect and IR sensors
* 🚨 **Accident Detection** using MPU6500 accelerometer
* 📍 **GPS Location Tracking** using GPS module
* 📱 **Emergency SMS Alerts** using SIM900A GSM module
* 📞 **Emergency Phone Calls** to predefined contacts
* 🔔 **Buzzer Alert** for accidents and no-helmet conditions
* 🖥️ **OLED Display** for helmet status and sensor readings
* 🗺️ **Google Maps Location Link** included in emergency SMS

## Hardware Components

* ESP32 Development Board
* MPU6500 Motion Sensor
* Hall Effect Sensor
* IR Sensor
* GPS Module
* SIM900A GSM Module
* 0.96" OLED Display (SSD1306)
* Buzzer
* Jumper Wires
* Smart Helmet

## Software and Libraries

The project is written in **C++ for ESP32 Arduino**.

Required libraries:

```text
Wire
MPU6500_WE
Adafruit GFX Library
Adafruit SSD1306
TinyGPSPlus
```

## Pin Configuration

| Component           | ESP32 Pin |
| ------------------- | --------: |
| I2C SDA             |   GPIO 21 |
| I2C SCL             |   GPIO 22 |
| SIM900A RX          |   GPIO 16 |
| SIM900A TX          |   GPIO 17 |
| GPS RX              |   GPIO 32 |
| GPS TX              |   GPIO 27 |
| Hall Sensor         |   GPIO 34 |
| IR Sensor           |   GPIO 35 |
| Buzzer              |   GPIO 18 |
| OLED I2C Address    |      0x3C |
| MPU6500 I2C Address |      0x68 |

```
## How It Works

### 1. System Initialization

The ESP32 initializes:

* OLED display
* MPU6500
* GPS
* SIM900A GSM module
* Hall Effect sensor
* IR sensor
* Buzzer

### 2. Helmet Detection

The Hall Effect and IR sensors are used to determine helmet status.

```text
Hall = 0, IR = 0  →  HELMET DETECT
Hall = 1, IR = 0  →  NO HELMET
Other combinations → OFF
```

### 3. Accident Detection

The MPU6500 continuously measures acceleration on the X, Y and Z axes.

The current program uses:

```cpp
bool alert = (y > 0.80 || z < 0.70);
```

If the condition is satisfied, the system treats it as an emergency condition.

### 4. Emergency Response

When an accident is detected:

```text
Accident Detected
       ↓
Activate Buzzer
       ↓
Read GPS Location
       ↓
Generate Google Maps Link
       ↓
Send SMS to Phone 1
       ↓
Send SMS to Phone 2
       ↓
Call Phone 1
       ↓
Call Phone 2
```

The SMS contains the accident alert and GPS location.

Example:

```text
EMERGENCY ALERT!

Accident Detected.

Location:
https://maps.google.com/?q=12.600000,80.080000
```

### 5. OLED Display

The OLED displays:

```text
Helmet Monitor

Helmet: HELMET DETECT

Y: 0.XX
Z: 0.XX

Status: NORMAL
```

During an emergency:

```text
Status: EMERGENCY
```

## Installation

### Using VS Code + PlatformIO

1. Install **VS Code**.
2. Install the **PlatformIO IDE** extension.
3. Create an ESP32 project.
4. Select the appropriate ESP32 development board.
5. Place the program in:

```text
src/main.cpp
```

6. Install the required libraries.
7. Connect the ESP32 to the computer.
8. Build and upload the program.

## Required Libraries

Install these libraries through PlatformIO or the Arduino Library Manager:

```text
MPU6500_WE
Adafruit GFX Library
Adafruit SSD1306
TinyGPSPlus
```

`Wire.h` is included with the Arduino framework.

## Important Configuration

Before uploading the program, update the emergency contact numbers:

```cpp
String phone1 = "YOUR_PHONE_NUMBER";
String phone2 = "YOUR_PHONE_NUMBER";
```

Also verify the GPS, GSM and sensor wiring according to the pin configuration.

## Emergency Location

If a valid GPS location is available, the system uses the current coordinates.

If GPS is unavailable, the program uses the predefined default coordinates:

```cpp
#define DEFAULT_LAT 12.600000
#define DEFAULT_LON 80.080000
```

These values should be changed to an appropriate fallback location if required.

## Technologies Used

* **ESP32**
* **Embedded C++**
* **Arduino Framework**
* **MPU6500**
* **GPS**
* **GSM**
* **OLED**
* **Hall Effect Sensor**
* **IR Sensor**

## Project Objective

The main objective of RiderShield is to provide an integrated safety mechanism for two-wheeler riders by combining:

**Helmet Detection + Accident Detection + GPS Tracking + GSM Emergency Communication**

into a single embedded system.
