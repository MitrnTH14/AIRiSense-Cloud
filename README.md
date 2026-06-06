AIRiSense Cloud 🌍☁️
AI-Powered Smart Air Quality Monitoring System using ESP32, Edge Impulse, Blynk IoT, and Environmental Sensors.

Overview
AIRiSense Cloud is an intelligent IoT-based air quality monitoring system that combines environmental sensing, edge AI inference, cloud connectivity, and real-time safety alerts.

The system continuously monitors:

Air Quality (VOC concentration using MQ135)
Carbon Monoxide (CO concentration using MQ7)
Ambient Light Intensity (BH1750)
Temperature (DHT11)
Humidity (DHT11)
Sensor data is processed locally on the ESP32 using an Edge Impulse machine learning model to classify air quality conditions into:

GOOD
MODERATE
POOR
When poor air quality is detected, the system automatically:

Activates a local buzzer alarm
Sends a push notification through Blynk Cloud
Updates a real-time OLED display
Uploads environmental data to the cloud dashboard
Features
Edge AI Inference
Real-time on-device machine learning inference using Edge Impulse.
No internet connection required for air quality classification.
Low-latency decision making.
IoT Cloud Monitoring
Real-time sensor visualization using Blynk Cloud.
Remote monitoring from smartphone or web dashboard.
Push notifications for critical air quality events.
Safety Alert System
Automatic buzzer activation during hazardous air conditions.
One-time notification logic to prevent alert spamming.
Remote alarm silencing through the Blynk app.
Automatic re-arming after a configurable timeout.
OLED Dashboard
Displays:

VOC concentration
CO concentration
Ambient light level
Temperature
Humidity
AI air quality status
Network connectivity
Current time
Smart Alarm Management
Remote silence control.
Automatic alarm recovery.
Continuous monitoring even during muted periods.
Hardware Components
Component	Purpose
ESP32 Dev Board	Main controller
MQ135 Gas Sensor	VOC/Air Quality detection
MQ7 Gas Sensor	Carbon Monoxide detection
DHT11	Temperature and Humidity
BH1750	Ambient Light Sensor
SSD1306 OLED Display (128x64)	Local display
Active Buzzer	Safety alerts
WiFi Network	Cloud connectivity
System Architecture
            ┌─────────────────────┐
            │     Sensors         │
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
            │ Classification      │
            └──────────┬──────────┘
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
   OLED Display    Buzzer      Blynk Cloud
                                  │
                                  ▼
                        Mobile Notifications
Pin Configuration
Device	ESP32 Pin
MQ135	GPIO 0
MQ7	GPIO 1
DHT11	GPIO 4
Buzzer	GPIO 5
OLED SDA	GPIO 21
OLED SCL	GPIO 22
BH1750 SDA	GPIO 21
BH1750 SCL	GPIO 22
Software Stack
Development Environment
Arduino IDE
Libraries
WiFi.h
BlynkSimpleEsp32.h
Wire.h
Adafruit_GFX.h
Adafruit_SSD1306.h
BH1750.h
DHT.h
Edge Impulse Inferencing SDK
Cloud Platform
Blynk IoT Cloud
AI Platform
Edge Impulse
Machine Learning Model
Input Features
The model uses the following features:

Feature
MQ135 PPM
MQ7 PPM
Lux
Temperature
Humidity
Output Classes
Label	Classification
0	GOOD
1	MODERATE
2	POOR
Inference is executed directly on the ESP32 using the exported Edge Impulse library.

Blynk Virtual Pins
Virtual Pin	Data
V0	MQ135 PPM
V1	MQ7 PPM
V2	Lux
V3	Air Quality Status
V4	Silence Alarm Control
V5	Temperature
V6	Humidity
Alert Logic
POOR Air Quality
When AI predicts POOR:

Push notification sent via Blynk
Buzzer activated
OLED status updated
Event logged in cloud
Alarm Silence
User can silence the alarm remotely using the Blynk application.

Behavior:

Buzzer turns OFF immediately
Monitoring continues
Alarm automatically re-arms after 5 minutes
Installation
1. Clone Repository
git clone https://github.com/yourusername/AIRiSense-Cloud.git
2. Install Required Libraries
Install all required libraries through Arduino Library Manager.

3. Import Edge Impulse Library
Export your trained Edge Impulse model as an Arduino library and install it in Arduino IDE.

4. Configure Credentials
Update the following values:

const char* ssid = "YOUR_WIFI";
const char* pass = "YOUR_PASSWORD";

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
5. Upload Firmware
Select:

Board: ESP32 Dev Module
Correct COM Port
Upload the sketch.

Future Improvements
MQTT integration
Air Quality Index (AQI) calculation
Historical cloud analytics
OTA firmware updates
Battery-powered operation
Additional gas sensors
Mobile dashboard enhancements
Multi-room monitoring support
Applications
Smart Homes
Industrial Safety Monitoring
Indoor Air Quality Assessment
Laboratories
Offices and Workspaces
Educational IoT Projects
Environmental Research
Author
Developed as part of the AIRiSense Cloud Smart Environmental Monitoring Project.

Combining Edge AI, IoT, and Embedded Systems for real-time environmental intelligence.

License
This project is licensed under the MIT License.

Feel free to use, modify, and distribute this project for educational and research purposes.
