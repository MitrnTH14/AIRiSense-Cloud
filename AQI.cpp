// =======================================================================
//                           AIRiSense Cloud
//      (V11 - Enhanced Remote Control Debugging)
// =======================================================================

// --- Your Wi-Fi and Blynk Credentials ---
const char* ssid = "RAMP";
const char* pass = "Check@401";

#define BLYNK_TEMPLATE_ID   "TMPL3-i5K-oKg"
#define BLYNK_TEMPLATE_NAME "AIRiSense Cloud"
#define BLYNK_AUTH_TOKEN    "-Of6i41qmDEkrJfkgOzq2_GGPae8VKul"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BH1750.h>
#include "time.h"

// --- Hardware Pin Configuration ---
#define MQ135_PIN 1
#define MQ7_PIN   2
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9
#define BUZZER_PIN 5

// --- SENSOR CALIBRATION & PPM CONSTANTS ---
#define RL_VALUE 5.0
#define R0_MQ135 26.84 // Your calibrated value
#define R0_MQ7   20.37 // Your calibrated value
#define PARA_MQ135 116.6020682
#define PARB_MQ135 -2.769034857
#define PARA_MQ7   99.042
#define PARB_MQ7   -1.518

// ... (Global Objects & Timers are the same)
Adafruit_SSD1306 display(128, 64, &Wire, -1);
BH1750 lightMeter;
BlynkTimer timer;
bool alarmSilenced = false;
unsigned long silenceStartTime = 0;
const long silenceDuration = 300000;
bool buzzerOn = false;
unsigned long beepStartTime = 0;
const long beepDuration = 100;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Function Declarations
void sendSensorData();
float calculateResistance(int adc_value);
float calculatePPM(float rs, float r0, float para, float parb);
String getAiClassification(float mq135_ppm, float mq7_ppm);
void updateLocalOutputs(String status);
void updateOLED(float mq135_ppm, float mq7_ppm, float lux, String status);


// ===============================================================
//         BLYNK_WRITE - Handles Remote Control Input (WITH DEBUGGING)
// ===============================================================
BLYNK_WRITE(V4) {
  // (NEW) This message will print every time you press the switch in the app
  Serial.println("--- BLYNK_WRITE(V4) TRIGGERED! ---");
  
  int switchValue = param.asInt();

  // (NEW) This will show the exact value received (should be 1)
  Serial.printf("Switch value received: %d\n", switchValue);
  
  if (switchValue == 1) {
    alarmSilenced = true;
    silenceStartTime = millis();
    Serial.println(" -> Alarm has been SILENCED for 5 minutes.");
    Blynk.virtualWrite(V4, 0); // Turn the switch back off in the app
  }
}

// ===============================================================
//                           SETUP
// ===============================================================
void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0); display.println("AIRiSense Cloud Booting..."); display.display();
  
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("Error initializing BH1750"));
  }

  display.clearDisplay(); display.setCursor(0,0); display.println("Connecting to WiFi..."); display.display();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Connected!");
  
  Blynk.config(BLYNK_AUTH_TOKEN);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  timer.setInterval(5000L, sendSensorData);
}

// ===============================================================
//                         MAIN LOOP
// ===============================================================
void loop() {
  if (!Blynk.connected()) {
    Blynk.connect();
  } else {
    Blynk.run();
  }
  
  timer.run();
  
  if (buzzerOn && (millis() - beepStartTime >= beepDuration)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOn = false;
  }

  if (alarmSilenced && (millis() - silenceStartTime >= silenceDuration)) {
    alarmSilenced = false;
    Serial.println("!!! Alarm has been RE-ARMED automatically. !!!");
  }
}

// ... (sendSensorData, calculateResistance, calculatePPM, getAiClassification are the same)
void sendSensorData() {
  float ppm_mq135 = calculatePPM(calculateResistance(analogRead(MQ135_PIN)), R0_MQ135, PARA_MQ135, PARB_MQ135);
  float ppm_mq7 = calculatePPM(calculateResistance(analogRead(MQ7_PIN)), R0_MQ7, PARA_MQ7, PARB_MQ7);
  float lux = lightMeter.readLightLevel();
  String ai_status = getAiClassification(ppm_mq135, ppm_mq7);
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, ppm_mq135); Blynk.virtualWrite(V1, ppm_mq7);
    Blynk.virtualWrite(V2, lux); Blynk.virtualWrite(V3, ai_status);
  }
  updateLocalOutputs(ai_status);
  updateOLED(ppm_mq135, ppm_mq7, lux, ai_status);
}
float calculateResistance(int adc_value) { return ((4095.0 * RL_VALUE) / adc_value) - RL_VALUE; }
float calculatePPM(float rs, float r0, float para, float parb) { return para * pow((rs / r0), parb); }
String getAiClassification(float mq135_ppm, float mq7_ppm) {
  if (mq135_ppm > 250 || mq7_ppm > 300) return "POOR";
  if (mq135_ppm > 75 || mq7_ppm > 100) return "MODERATE";
  return "GOOD";
}


// --- BUZZER LOGIC WITH DEBUGGING ---
void updateLocalOutputs(String status) {
  if (status == "POOR") {
    Serial.println("AI Status is POOR. Checking buzzer conditions...");
    if (alarmSilenced) {
      Serial.println(" -> Buzzer will not sound because ALARM IS SILENCED.");
    } else if (buzzerOn) {
      Serial.println(" -> Buzzer is already beeping. No action needed.");
    } else {
      Serial.println(" -> Conditions met! Turning buzzer ON.");
      buzzerOn = true;
      beepStartTime = millis();
      digitalWrite(BUZZER_PIN, HIGH);
    }
  }
}

// ... (updateOLED is the same)
void updateOLED(float mq135_ppm, float mq7_ppm, float lux, String status) {
  struct tm timeinfo; getLocalTime(&timeinfo); char timeString[9]; strftime(timeString, sizeof(timeString), "%T", &timeinfo);
  display.clearDisplay();
  display.setTextSize(1); display.setCursor(0, 0);
  display.printf("VOC:%.1f CO:%.1f\n", mq135_ppm, mq7_ppm);
  display.printf("Lux: %.1f\n", lux);
  display.printf("Status: %s\n", status.c_str());
  display.println("---------------------");
  display.setCursor(0, 55);
  display.print(WiFi.status() == WL_CONNECTED ? "OK" : "NO");
  display.print("/");
  display.print(Blynk.connected() ? "OK" : "NO");
  if (alarmSilenced) { display.print(" [S]"); }
  display.setCursor(80, 55); display.print(timeString);
  display.display();
}