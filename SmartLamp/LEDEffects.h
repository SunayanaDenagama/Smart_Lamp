#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Config.h"

class LEDEffects {
 public:
  void begin();
  void update(uint32_t now);
  void setColor(const CRGB& color);
  void setBrightness(uint8_t brightness);
  void startFade(uint8_t from, uint8_t to, uint32_t durationMs, uint32_t now);
  void showPickerColor(uint8_t index, uint8_t brightness);
  uint8_t brightness() const { return brightness_; }
  bool isFading() const { return fading_; }
  static CRGB pickerColor(uint8_t index);

 private:
  CRGB leds_[Config::NUM_LEDS];
  CRGB color_ = CRGB::White;
  uint8_t brightness_ = 0;
  bool fading_ = false;
  uint8_t fadeFrom_ = 0;
  uint8_t fadeTo_ = 0;
  uint32_t fadeStart_ = 0;
  uint32_t fadeDuration_ = 0;
  void render();
};
