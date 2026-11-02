// =======================================================================
//                           AIRiSense Cloud
//      (V20 - Instant Silence Response & Notifications)
// =======================================================================

// --- Credentials ---
const char* ssid = "YOUR_WIFI";
const char* pass = "YOUR_PASSWORD";
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "AIRiSense Cloud"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BH1750.h>
#include "time.h"
#include "DHT.h"

// --- LIBRARY INCLUDE ---
#include <aqi_inferencing.h>

// --- Hardware Pins ---
#define MQ135_PIN 0
#define MQ7_PIN   1
#define DHTPIN    4
#define BUZZER_PIN 5
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define DHTTYPE DHT11

// --- CALIBRATION ---
#define RL_VALUE 5.0
#define R0_MQ135 15.29
#define R0_MQ7   11.30

#define PARA_MQ135 116.6020682
#define PARB_MQ135 -2.769034857
#define PARA_MQ7   99.042
#define PARB_MQ7   -1.518

// --- Objects ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);
BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Variables
bool alarmSilenced = false;
unsigned long silenceStartTime = 0;
const long silenceDuration = 300000; // 5 minutes
bool notificationSent = false;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Declarations
void sendSensorData();
float calculateResistance(int adc_value);
float calculatePPM(float rs, float r0, float para, float parb);
String getAiClassification(float mq135, float mq7, float lux, float temp, float humid);
void updateLocalOutputs(String status);
void updateOLED(float mq135, float mq7, float lux, float temp, float humid, String status);

// --- REMOTE CONTROL (Instant Silence Fix) ---
BLYNK_WRITE(V4) {
  if (param.asInt() == 1) {
    alarmSilenced = true;
    silenceStartTime = millis();

    // FORCE BUZZER OFF IMMEDIATELY
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("Alarm silenced via Cloud. Buzzer Forced OFF.");
    Blynk.virtualWrite(V4, 0); // Reset switch in app
  }
}

// ===============================================================
//                           SETUP
// ===============================================================
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0); display.println("Booting AIRiSense..."); display.display();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Blynk.config(BLYNK_AUTH_TOKEN);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  timer.setInterval(5000L, sendSensorData);
}

// ===============================================================
//                         MAIN LOOP
// ===============================================================
void loop() {
  if (Blynk.connected()) Blynk.run(); else Blynk.connect();
  timer.run();

  // Re-arm alarm if silence time expires
  if (alarmSilenced && (millis() - silenceStartTime >= silenceDuration)) {
    alarmSilenced = false;
    Serial.println("Silence timer expired. Re-arming.");
  }
}

// ===============================================================
//                       CORE FUNCTIONS
// ===============================================================
void sendSensorData() {
  float ppm_mq135 = calculatePPM(calculateResistance(analogRead(MQ135_PIN)), R0_MQ135, PARA_MQ135, PARB_MQ135);
  float ppm_mq7 = calculatePPM(calculateResistance(analogRead(MQ7_PIN)), R0_MQ7, PARA_MQ7, PARB_MQ7);
  float lux = lightMeter.readLightLevel();
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(hum) || isnan(temp)) return;

  // --- AI PREDICTION ---
  String ai_status = getAiClassification(ppm_mq135, ppm_mq7, lux, temp, hum);

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, ppm_mq135); Blynk.virtualWrite(V1, ppm_mq7);
    Blynk.virtualWrite(V2, lux); Blynk.virtualWrite(V3, ai_status);
    Blynk.virtualWrite(V5, temp); Blynk.virtualWrite(V6, hum);
  }

  updateLocalOutputs(ai_status);
  updateOLED(ppm_mq135, ppm_mq7, lux, temp, hum, ai_status);
}

// --- EDGE IMPULSE CLASSIFICATION ---
String getAiClassification(float mq135, float mq7, float lux, float temp, float humid) {
  float features[] = { mq135, mq7, lux, temp, humid };
  signal_t signal;
  int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
  if (err != 0) return "ERR_SIG";

  ei_impulse_result_t result = { 0 };
  err = run_classifier(&signal, &result, false);
  if (err != EI_IMPULSE_OK) return "ERR_RUN";

  int bestIdx = 0;
  float maxVal = result.classification[0].value;
  for (size_t i = 1; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (result.classification[i].value > maxVal) {
      maxVal = result.classification[i].value;
      bestIdx = i;
    }
  }

  // Get raw label and translate "2" to "POOR" for logic
  String rawLabel = result.classification[bestIdx].label;
  if (rawLabel == "2") return "POOR";
  if (rawLabel == "1") return "MODERATE";
  if (rawLabel == "0") return "GOOD";

  rawLabel.toUpperCase();
  return rawLabel;
}

float calculateResistance(int adc_value) { return ((4095.0 * RL_VALUE) / adc_value) - RL_VALUE; }
float calculatePPM(float rs, float r0, float para, float parb) { return para * pow((rs / r0), parb); }

// --- UPDATED OUTPUT LOGIC ---
void updateLocalOutputs(String status) {
  if (status == "POOR") {
    // 1. Send Notification (Once per event)
    if (!notificationSent) {
      Serial.println("Sending Blynk Notification: Poor Air Quality!");
      Blynk.logEvent("poor_air", "⚠️ Critical Alert: Poor Air Quality Detected!");
      notificationSent = true;
    }

    // 2. Handle Buzzer (Continuous Sound)
    if (alarmSilenced) {
      digitalWrite(BUZZER_PIN, LOW); // User silenced it
    } else {
      digitalWrite(BUZZER_PIN, HIGH); // Continuous ON
    }
  }
  else {
    // Air is Good/Moderate -> Reset everything
    digitalWrite(BUZZER_PIN, LOW);
    notificationSent = false;
    alarmSilenced = false;
  }
}

void updateOLED(float mq135, float mq7, float lux, float temp, float humid, String status) {
  struct tm timeinfo; getLocalTime(&timeinfo); char timeString[9]; strftime(timeString, sizeof(timeString), "%T", &timeinfo);
  display.clearDisplay();
  display.setTextSize(1); display.setCursor(0, 0);
  display.printf("VOC:%.1f CO:%.1f\n", mq135, mq7);
  display.printf("Lux:%.1f\n", lux);
  display.printf("T:%.1fC H:%.1f%%\n", temp, humid);
  display.printf("AI Status: %s\n", status.c_str());
  display.println("---------------------");
  display.setCursor(0, 55);
  display.print(WiFi.status() == WL_CONNECTED ? "OK" : "NO"); display.print("/"); display.print(Blynk.connected() ? "OK" : "NO");
  if (alarmSilenced) display.print(" [S]");
  display.setCursor(80, 55); display.print(timeString);
  display.display();
}
