# 🌍☁️ AIRiSense Cloud

### AI-Powered Smart Air Quality Monitoring System using ESP32, Edge Impulse, Blynk IoT, and Environmental Sensors

---

## 📖 Overview

**AIRiSense Cloud** is an intelligent IoT-based environmental monitoring system that combines **real-time sensing**, **Edge AI inference**, **cloud connectivity**, and **smart safety alerts** to monitor indoor air quality.

The system continuously collects environmental data from multiple sensors and processes it locally on an **ESP32** using a machine learning model developed with **Edge Impulse**.

### Parameters Monitored

* 🌫️ Air Quality (VOC concentration using MQ135)
* ☠️ Carbon Monoxide (CO concentration using MQ7)
* 💡 Ambient Light Intensity (BH1750)
* 🌡️ Temperature (DHT11)
* 💧 Humidity (DHT11)

The AI model classifies environmental conditions into:

* ✅ GOOD
* ⚠️ MODERATE
* 🚨 POOR

When hazardous air quality is detected, the system automatically:

* Activates a local buzzer alarm
* Sends push notifications through Blynk Cloud
* Updates the OLED display
* Uploads sensor data to the cloud dashboard

---

# ✨ Features

## 🧠 Edge AI Inference

* Real-time machine learning inference on ESP32
* Powered by Edge Impulse
* No internet connection required for classification
* Low-latency decision making

## ☁️ IoT Cloud Monitoring

* Real-time data visualization through Blynk Cloud
* Remote monitoring via smartphone or web dashboard
* Instant push notifications for critical conditions

## 🚨 Smart Safety Alert System

* Automatic buzzer activation during hazardous conditions
* One-time notification logic prevents alert spamming
* Remote alarm silencing through Blynk app
* Automatic re-arming after configurable timeout

## 📺 OLED Dashboard

Displays:

* VOC Concentration
* CO Concentration
* Temperature
* Humidity
* Light Intensity
* Air Quality Status
* Network Status
* Current Time

## 🔔 Smart Alarm Management

* Remote alarm mute functionality
* Automatic alarm recovery
* Continuous monitoring even when muted

---

# 🛠 Hardware Components

| Component            | Purpose                     |
| -------------------- | --------------------------- |
| ESP32 Dev Board      | Main Controller             |
| MQ135 Gas Sensor     | VOC / Air Quality Detection |
| MQ7 Gas Sensor       | Carbon Monoxide Detection   |
| DHT11                | Temperature & Humidity      |
| BH1750               | Ambient Light Sensor        |
| SSD1306 OLED Display | Local Data Visualization    |
| Active Buzzer        | Safety Alerts               |
| Wi-Fi Network        | Cloud Connectivity          |

---

# 🏗 System Architecture

```text
            ┌─────────────────────┐
            │      Sensors        │
            │ MQ135 / MQ7         │
            │ DHT11 / BH1750      │
            └──────────┬──────────┘
                       │
                       ▼
            ┌─────────────────────┐
            │       ESP32         │
            │ Data Acquisition    │
            └──────────┬──────────┘
                       │
                       ▼
            ┌─────────────────────┐
            │   Edge Impulse AI   │
            │   Classification    │
            └──────────┬──────────┘
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
   OLED Display    Buzzer      Blynk Cloud
                                  │
                                  ▼
                        Mobile Notifications
```

---

# 🔌 Pin Configuration

| Device     | ESP32 Pin |
| ---------- | --------- |
| MQ135      | GPIO 0    |
| MQ7        | GPIO 1    |
| DHT11      | GPIO 4    |
| Buzzer     | GPIO 5    |
| OLED SDA   | GPIO 21   |
| OLED SCL   | GPIO 22   |
| BH1750 SDA | GPIO 21   |
| BH1750 SCL | GPIO 22   |

---

# 💻 Software Stack

## Development Environment

* Arduino IDE

## Libraries Used

```cpp
WiFi.h
BlynkSimpleEsp32.h
Wire.h
Adafruit_GFX.h
Adafruit_SSD1306.h
BH1750.h
DHT.h
Edge Impulse Inferencing SDK
```

## Cloud Platform

* Blynk IoT Cloud

## AI Platform

* Edge Impulse

---

# 🤖 Machine Learning Model

## Input Features

| Feature     |
| ----------- |
| MQ135 PPM   |
| MQ7 PPM     |
| Lux         |
| Temperature |
| Humidity    |

## Output Classes

| Label | Classification |
| ----- | -------------- |
| 0     | GOOD           |
| 1     | MODERATE       |
| 2     | POOR           |

Inference is executed directly on the ESP32 using the exported Edge Impulse Arduino Library.

---

# 📲 Blynk Virtual Pins

| Virtual Pin | Data                  |
| ----------- | --------------------- |
| V0          | MQ135 PPM             |
| V1          | MQ7 PPM               |
| V2          | Light Intensity (Lux) |
| V3          | Air Quality Status    |
| V4          | Alarm Silence Control |
| V5          | Temperature           |
| V6          | Humidity              |

---

# 🚨 Alert Logic

## Poor Air Quality Detection

When the AI model predicts **POOR**:

* Push notification sent through Blynk
* Buzzer activated
* OLED display updated
* Event logged to cloud dashboard

### Alarm Silence Feature

Users can remotely mute the buzzer using the Blynk application.

#### Behavior

* Buzzer turns OFF immediately
* Environmental monitoring continues
* Alarm automatically re-arms after 5 minutes

---

# ⚙️ Installation

## 1. Clone Repository

```bash
git clone https://github.com/yourusername/AIRiSense-Cloud.git
```

## 2. Install Required Libraries

Install all required libraries through the Arduino Library Manager.

## 3. Import Edge Impulse Library

Export the trained Edge Impulse model as an Arduino Library and install it in Arduino IDE.

## 4. Configure Credentials

```cpp
const char* ssid = "YOUR_WIFI";
const char* pass = "YOUR_PASSWORD";

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

## 5. Upload Firmware

Select:

* Board: ESP32 Dev Module
* Correct COM Port
* Upload the Sketch

---

# 🚀 Future Enhancements

* MQTT Integration
* Air Quality Index (AQI) Calculation
* Historical Cloud Analytics
* OTA Firmware Updates
* Battery-Powered Operation
* Additional Gas Sensors
* Advanced Mobile Dashboard
* Multi-Room Monitoring
* Predictive Air Quality Analytics

---

# 🎯 Applications

* Smart Homes
* Industrial Safety Monitoring
* Indoor Air Quality Assessment
* Laboratories
* Educational IoT Projects
* Offices and Workspaces
* Environmental Research
* Smart Buildings

---

# 👨‍💻 Author

Developed as part of the **AIRiSense Cloud – Smart Environmental Monitoring Project**.

Combining **Embedded Systems**, **Edge AI**, and **IoT Technologies** to create an intelligent environmental monitoring solution capable of real-time analysis and proactive safety alerts.

---

# 📄 License

This project is licensed under the **MIT License**.

Feel free to use, modify, and distribute this project for educational, research, and non-commercial purposes.
