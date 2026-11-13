#pragma once

#include <Arduino.h>
#include "GestureSensor.h"
#include "LEDEffects.h"

// Five states retained from the original single-file sketch.
enum class LampState : uint8_t {
  Off,
  AutoAdjusting,
  ManualLocked,
  ManualUnlocked,
  ColorPicker
};

class LampController {
 public:
  explicit LampController(LEDEffects& leds) : leds_(leds) {}
  void update(const SensorReading& reading, uint32_t now);
  LampState state() const { return state_; }

 private:
  LEDEffects& leds_;
  LampState state_ = LampState::Off;
  CRGB savedColor_ = CRGB::White;
  uint8_t savedBrightness_ = 0;
  uint8_t pickerLastColorIndex_ = 0;
  int16_t colorHoldProx_ = -1;

  uint32_t holdStart_ = 0;
  uint32_t offHoldStart_ = 0;
  uint32_t colorHoldStart_ = 0;
  uint32_t autoStart_ = 0;
  uint32_t pickerGraceStart_ = 0;
  uint32_t offIgnoreStart_ = 0;
  bool onHolding_ = false;
  bool offHolding_ = false;
  bool pickerGraceActive_ = false;
  bool offCooldownActive_ = false;

  bool inRange(uint8_t prox) const;
  void enterState(LampState next);
  bool checkColorPickerEntry(uint8_t prox, uint32_t now);
  bool checkOffGesture(uint8_t prox, uint32_t now);
  void handleOff(const SensorReading& reading, uint32_t now);
  void handleAuto(uint32_t now);
  void handleLocked(uint8_t prox, uint32_t now);
  void handleUnlocked(uint8_t prox);
  void handleColorPicker(uint8_t prox, uint32_t now);
  static uint8_t nonlinearBrightness(uint8_t prox);
};
