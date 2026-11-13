/**
 * Gesture-Controlled Smart Lamp
 * Arduino + APDS9960 + WS2812B (60 LEDs)
 * Main entry point; hardware, effects and state logic live in modules.
 */
#include "Config.h"
#include "GestureSensor.h"
#include "LEDEffects.h"
#include "LampController.h"

GestureSensor gestureSensor;
LEDEffects ledEffects;
LampController lampController(ledEffects);

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  ledEffects.begin();
  if (!gestureSensor.begin()) {
    Serial.println(F("APDS INIT FAILED"));
    while (true) { delay(1000); }
  }
  Serial.println(F("=== SMART LAMP MODULAR BUILD ==="));
}

void loop() {
  const uint32_t now = millis();
  ledEffects.update(now);

  SensorReading reading;
  if (gestureSensor.read(reading)) {
    lampController.update(reading, now);
  }
  delay(Config::LOOP_INTERVAL_MS);
}
