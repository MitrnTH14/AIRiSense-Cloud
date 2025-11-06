# AIRiSense Cloud: An AI-Enhanced IoT Air Quality Monitor

![WhatsApp Image 2025-10-31 at 22 25 41_a81ff713](https://github.com/user-attachments/assets/c938e981-afc7-423e-a2c2-b70f7270d0b2)
![WhatsApp Image 2025-11-01 at 09 57 36_a833adee](https://github.com/user-attachments/assets/d5ee234d-f627-4866-b4db-24a807054279)

**AIRiSense Cloud** is a low-cost, AI-enhanced IoT air quality monitoring system built on the ESP32-C6 platform. It addresses the critical need for accessible and intelligent environmental monitoring by converting raw sensor data into calibrated Parts Per Million (PPM) values for high accuracy.

The system classifies air quality as "Good," "Moderate," or "Poor" and provides immediate local feedback via an OLED display and an audible buzzer. All data is streamed to a custom Blynk dashboard for real-time remote monitoring, data logging, and interactive remote control.

---

## 🚀 Key Features

* **Accurate Scientific Data:** Implements a two-stage calibration process to convert raw analog sensor readings into scientifically accurate **Parts Per Million (PPM)** values.
* **Multi-Sensor Integration:** Measures Volatile Organic Compounds (VOCs), Carbon Monoxide (CO), and ambient light (Lux) in real-time.
* **AI-Based Classification:** A rule-based algorithm on the device classifies the air quality into "Good," "Moderate," or "Poor."
* **Local Feedback System:**
    * **Visual:** A crisp OLED display shows all sensor values, AI status, and connectivity status (`OK/OK`).
    * **Audible:** A transistor-driven buzzer sounds an alarm when air quality is "POOR."
* **Cloud Connectivity:** Uses the ESP32-C6's Wi-Fi to stream all data to the Blynk IoT platform.
* **Remote Control:** Features a **bidirectional** "Silence Alarm" switch on the dashboard that sends a command from the cloud to the device to mute the buzzer for 5 minutes.

---

## 🛠️ Hardware Requirements

* **Microcontroller:** ESP32-C6 DevKitC-1
* **Gas Sensors:**
    * MQ-135 (for VOCs)
    * MQ-7 (for CO)
* **Light Sensor:** BH1750 (I2C)
* **Display:** SSD1306 128x64 OLED Display (I2C)
* **Audible Alert:** Active Buzzer (3.3V/5V)
* **Buzzer Driver Circuit:**
    * 1x NPN Transistor (e.g., BC547 or 2N2222)
    * 1x 10kΩ Resistor
* **Other:** Breadboard, Jumper Wires

---

## 🔧 Software & Setup

### 1. Arduino IDE Libraries

Before uploading the code, you must install the following libraries via the Arduino Library Manager:
* `Blynk` by Volodymyr Shymanskyy
* `Adafruit SSD1306` by Adafruit
* `Adafruit GFX Library` by Adafruit
* `BH1750` by Christopher Laws

### 2. Blynk Cloud Setup

1.  **Create a Blynk Account** (if you don't have one).
2.  **Create a new Template** (e.g., "AIRiSense Cloud").
3.  **Create 5 Datastreams:**
    * `V0`: Name: `VOC Level`, Type: `Double`, Units: `PPM`, Min: `0`, Max: `500`
    * `V1`: Name: `CO Level`, Type: `Double`, Units: `PPM`, Min: `0`, Max: `1000`
    * `V2`: Name: `Ambient Light`, Type: `Double`, Units: `Lux`, Min: `0`, Max: `2000`
    * `V3`: Name: `AI Status`, Type: `String`
    * `V4`: Name: `Silence Alarm`, Type: `Integer`, Min: `0`, Max: `1`
4.  **Build a Web Dashboard:**
    * Add **Gauges** for V0 and V1.
    * Add a **Value Display** for V2.
    * Add a **Label** for V3.
    * Add a **Switch** for V4.
5.  **Create a New Device** from your template to get your **Auth Credentials**.

### 3. Firmware Setup

1.  Open the main `.ino` project file.
2.  Fill in your credentials at the top of the file:
    ```cpp
    // --- Your Wi-Fi and Blynk Credentials ---
    const char* ssid = "YOUR_WIFI_SSID";
    const char* pass = "YOUR_WIFI_PASSWORD";

    #define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
    #define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
    #define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"
    ```

---

##  calibration_sketch.ino - **CRITICAL STEP**

To get accurate PPM readings, you **must** calibrate your sensors first.

1.  Open and upload the `calibration_sketch.ino` file to your ESP32.
2.  Take the device to a **clean air environment** (outdoors is best).
3.  Let it run for **20-30 minutes** for the sensors to warm up and stabilize.
4.  Watch the Serial Monitor. Record the final, stable `R0` values.
5.  Open the main `AIRiSense_Cloud.ino` file and update the `R0` values with your new ones:
    ```cpp
    // ✅ Replace these values with your new ones from recalibration
    #define R0_MQ135 26.84 // Your new value here
    #define R0_MQ7   20.37 // Your new value here
    ```

---

## 🔌 Circuit & Wiring

| Component | Pin | Connects To |
| :--- | :--- | :--- |
| **ESP32-C6** | `3.3V` | VCC on all sensors/OLED, Buzzer `+` |
| **ESP32-C6** | `GND` | GND on all sensors/OLED, Transistor `Emitter` |
| **MQ-135** | `A0` | ESP32 `GPIO 1` |
| **MQ-7** | `A0` | ESP32 `GPIO 2` |
| **OLED (SDA)** | `SDA` | ESP32 `GPIO 8` |
| **BH1750 (SDA)** | `SDA` | ESP32 `GPIO 8` |
| **OLED (SCL)** | `SCL` | ESP32 `GPIO 9` |
| **BH1750 (SCL)**| `SCL` | ESP32 `GPIO 9` |
| **ESP32-C6** | `GPIO 5`| 10kΩ Resistor |
| **10kΩ Resistor** | (Other end) | Transistor **Base** (Middle Pin) |
| **Transistor** | **Collector** (Right Pin)| Buzzer `-` (Negative) |
| **Transistor** | **Emitter** (Left Pin) | ESP32 `GND` |

---

## 💡 How It Works

1.  **Boot Up:** The device connects to Wi-Fi and Blynk. The OLED displays connection status.
2.  **Monitor:** Every 5 seconds, the device reads all sensors, calculates PPM, and classifies the air quality.
3.  **Display:** The data is shown on the local OLED and sent to the Blynk dashboard.
4.  **Alert (if "POOR"):** The `Status: POOR` message appears, and the buzzer starts beeping.
5.  **Silence (Optional):** The user can press the "Silence Alarm" switch on the Blynk app. This mutes the buzzer for 5 minutes, and an `[S]` icon appears on the OLED as confirmation.

---

## 🔮 Future Scope

* **True Edge AI:** Replace the rule-based algorithm with a Neural Network model trained using **Edge Impulse** on collected data.
* **Advanced Alerts:** Implement Blynk's **Push Notification** feature to send alerts directly to a smartphone.
* **Hardware Expansion:** Add a **PM2.5 particulate matter sensor** (like a PMS5003) for a more comprehensive air quality profile.

---

## 📄 License

This project is licensed under the MIT License - see the `LICENSE.md` file for details.
