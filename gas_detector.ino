#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL27zJazjr"
#define BLYNK_TEMPLATE_NAME "Gas Detector"
#define BLYNK_AUTH_TOKEN "E7gqwxX7D8eRvSsUAQ6jW6thvRfb82I"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char ssid[] = "Snyman";
char pass[] = "********";

// Sensor pins
const int mq5Pin = 39;     // MQ‑5 analog output to GPIO39
const int buzzerPin = 14;  // Active‑low buzzer on GPIO14

// Threshold
const float mq5Threshold = 1.0;

// Voltage conversion factors
const float espMaxVoltage = (5.0 / 16800.0) * 10000.0;
const float voltageDividerMultiplier = (10000.0 + 6800.0) / 10000.0;

// Blynk virtual pins
#define VPIN_MQ5 V1
#define VPIN_BUZZER_SWITCH V2

// Buzzer control variable
bool buzzerEnabled = true;

// Blynk app switch = enable/disable buzzer
BLYNK_WRITE(VPIN_BUZZER_SWITCH) {
  buzzerEnabled = param.asInt();  // 1 = enabled, 0 = disabled
  if (buzzerEnabled) {
    digitalWrite(buzzerPin, HIGH);  // OFF
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // ESP32 ADC is 12‑bit (0–4095)

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);  // buzzer off (active LOW)

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  // Read MQ‑5 analog value and convert to voltage
  float mq5RawVoltage = (analogRead(mq5Pin) / 4095.0) * espMaxVoltage;
  float mq5AoVoltage = mq5RawVoltage * voltageDividerMultiplier;

  // Send to Blynk
  Blynk.virtualWrite(VPIN_MQ5, mq5AoVoltage);

  // Buzzer alarm logic
  if (buzzerEnabled && (mq5AoVoltage > mq5Threshold)) {
    digitalWrite(buzzerPin, LOW);  // ON
    Blynk.logEvent("gas_alert", "MQ‑5: Gas level exceeded!");
  } else {
    digitalWrite(buzzerPin, HIGH);  // OFF
  }

  delay(1000);  // update every second
}