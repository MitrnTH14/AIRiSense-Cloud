## ⚙️ How It Works: System Workflow

The AIRiSense Cloud firmware operates on a non-blocking loop to ensure real-time responsiveness. Here is the step-by-step logic flow:

### **Step 1: Initialization & Connectivity**
* On boot, the **ESP32-C6** initializes the I2C bus (OLED + Light Sensor) and the analog pins (Gas Sensors).
* It loads the **TensorFlow Lite** model into memory.
* It connects to **Wi-Fi** and authenticates with the **Blynk IoT Cloud**.
* *Visual:* The OLED displays `Booting AI Model...` followed by connection status (`OK/OK`).

### **Step 2: Data Acquisition & Calibration**
* Every 5 seconds, the system reads raw voltage from the **MQ-135** and **MQ-7** sensors.
* **PPM Conversion:** It does not use raw values. Instead, it calculates the sensor's physical resistance ($R_S$) and compares it against the calibrated clean-air baseline ($R_0$) established during the calibration phase.
* It applies logarithmic regression to convert this ratio into accurate **Parts Per Million (PPM)**.

### **Step 3: Edge AI Inference (The "Brain")**
* The system aggregates 5 data points: `[VOC, CO, Light, Temperature, Humidity]`.
* This feature array is fed into the **Edge Impulse Neural Network** running locally on the chip.
* The AI analyzes the non-linear relationship between these variables and outputs a probability score for three classes:
    * `0: GOOD`
    * `1: MODERATE`
    * `2: POOR`

### **Step 4: Decision & Local Alerts**
* If the AI predicts **"POOR"** (and the alarm isn't silenced):
    * **Visual:** The OLED updates to show `Status: POOR`.
    * **Audible:** The GPIO triggers the **NPN Transistor**, powering the active buzzer continuously.
    * **Notification:** A flag is set to trigger a **Push Notification** to the user's phone via Blynk (limited to once per event to prevent spam).

### **Step 5: Cloud Sync & Remote Control**
* The device pushes the calculated PPM values, Lux, and AI Status to the **Blynk Cloud**.
* **Bidirectional Listener:** The device simultaneously listens for incoming commands. If the user toggles the **"Silence Alarm"** switch on the dashboard:
    1.  Blynk sends a `1` to Virtual Pin `V4`.
    2.  The ESP32 intercepts this via `BLYNK_WRITE(V4)`.
    3.  It immediately cuts power to the buzzer and starts a **5-minute non-blocking timer**.
    4.  An `[S]` icon appears on the OLED to indicate "Silenced Mode."
