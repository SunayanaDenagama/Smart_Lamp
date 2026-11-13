#include "LampController.h"
#include <math.h>
#include "Config.h"

bool LampController::inRange(uint8_t prox) const {
  return prox >= Config::ON_MIN && prox <= Config::ON_MAX;
}

void LampController::enterState(LampState next) {
  state_ = next;
  onHolding_ = false;
  offHolding_ = false;
  colorHoldProx_ = -1;
  pickerGraceActive_ = false;
}

uint8_t LampController::nonlinearBrightness(uint8_t prox) {
  float norm = (prox - Config::ON_MIN) /
               float(Config::ON_MAX - Config::ON_MIN);
  norm = constrain(norm, 0.0f, 1.0f);
  const float curved = powf(norm, 0.45f);
  const float value = Config::BRIGHTEST -
      curved * (Config::BRIGHTEST - Config::DIMMEST);
  return static_cast<uint8_t>(constrain(value,
      float(Config::DIMMEST), float(Config::BRIGHTEST)));
}

bool LampController::checkColorPickerEntry(uint8_t prox, uint32_t now) {
  if (prox < Config::COLOR_HOLD_MIN || prox > Config::COLOR_HOLD_MAX) {
    colorHoldProx_ = -1;
    return false;
  }
  if (colorHoldProx_ < 0 ||
      abs(static_cast<int>(prox) - colorHoldProx_) > Config::COLOR_STEADY_TOL) {
    colorHoldProx_ = prox;
    colorHoldStart_ = now;
    return false;
  }
  if (now - colorHoldStart_ >= Config::COLOR_HOLD_TIME_MS) {
    enterState(LampState::ColorPicker);
    Serial.println(F(">>> COLOR PICKER MODE"));
    return true;
  }
  return false;
}

bool LampController::checkOffGesture(uint8_t prox, uint32_t now) {
  if (prox < Config::OFF_PROX) {
    offHolding_ = false;
    return false;
  }
  if (!offHolding_) {
    offHolding_ = true;
    offHoldStart_ = now;
  }
  if (now - offHoldStart_ >= Config::OFF_HOLD_TIME_MS) {
    const uint8_t current = leds_.brightness();
    enterState(LampState::Off);
    offCooldownActive_ = true;
    offIgnoreStart_ = now;
    leds_.startFade(current, 0, Config::FADE_OUT_DURATION_MS, now);
    Serial.println(F(">>> OFF"));
    return true;
  }
  return false;
}

void LampController::handleOff(const SensorReading& reading, uint32_t now) {
  if (!inRange(reading.proximity)) {
    onHolding_ = false;
    return;
  }
  if (!onHolding_) {
    onHolding_ = true;
    holdStart_ = now;
  }
  if (now - holdStart_ >= Config::ON_HOLD_TIME_MS) {
    enterState(LampState::AutoAdjusting);
    autoStart_ = now;
    // Preserve the original map() curve and ambient range, clamping output.
    const uint8_t b = constrain(map(reading.ambient, 0, 1000,
        Config::DIMMEST, Config::BRIGHTEST),
        long(Config::DIMMEST), long(Config::BRIGHTEST));
    leds_.setColor(savedColor_);
    leds_.startFade(0, b, Config::FADE_IN_DURATION_MS, now);
    savedBrightness_ = b;
    Serial.println(F(">>> ON -> AUTO"));
  }
}

void LampController::handleAuto(uint32_t now) {
  if (now - autoStart_ >= Config::AUTO_TIME_MS) {
    enterState(LampState::ManualLocked);
    Serial.println(F(">>> AUTO DONE -> LOCKED"));
  }
}

void LampController::handleLocked(uint8_t prox, uint32_t now) {
  if (!inRange(prox)) {
    onHolding_ = false;
    return;
  }
  if (!onHolding_) {
    onHolding_ = true;
    holdStart_ = now;
  }
  if (now - holdStart_ >= Config::ON_HOLD_TIME_MS) {
    enterState(LampState::ManualUnlocked);
    Serial.println(F(">>> MANUAL UNLOCKED"));
  }
}

void LampController::handleUnlocked(uint8_t prox) {
  if (inRange(prox)) {
    savedBrightness_ = nonlinearBrightness(prox);
    leds_.setBrightness(savedBrightness_);
  } else {
    enterState(LampState::ManualLocked);
    Serial.println(F(">>> HAND REMOVED -> LOCKED (SAVED)"));
  }
}

void LampController::handleColorPicker(uint8_t prox, uint32_t now) {
  const uint8_t breatheVal = beatsin8(30, 50, 150);
  if (prox >= Config::PICKER_PROX_MIN) {
    pickerGraceActive_ = false;
    const float norm = (prox - Config::PICKER_PROX_MIN) / 140.0f;
    const int index = static_cast<int>((1.0f - norm) * Config::PICKER_COLOR_COUNT);
    pickerLastColorIndex_ = constrain(index, 0,
        int(Config::PICKER_COLOR_COUNT - 1));
    leds_.showPickerColor(pickerLastColorIndex_, breatheVal);
  } else {
    if (!pickerGraceActive_) {
      pickerGraceActive_ = true;
      pickerGraceStart_ = now;
    }
    if (now - pickerGraceStart_ >= Config::PICKER_GRACE_PERIOD_MS) {
      savedColor_ = LEDEffects::pickerColor(pickerLastColorIndex_);
      enterState(LampState::ManualLocked);
      leds_.setColor(savedColor_);
      // Original sketch commits the selected color at full brightness.
      savedBrightness_ = Config::BRIGHTEST;
      leds_.setBrightness(savedBrightness_);
      Serial.println(F(">>> COLOR SAVED -> EXIT PICKER"));
    }
  }
}

void LampController::update(const SensorReading& reading, uint32_t now) {
  if (state_ == LampState::Off && offCooldownActive_) {
    if (now - offIgnoreStart_ < Config::OFF_IGNORE_TIME_MS) return;
    offCooldownActive_ = false;
  }

  if (state_ == LampState::ManualLocked ||
      state_ == LampState::ManualUnlocked) {
    if (checkColorPickerEntry(reading.proximity, now)) return;
    if (checkOffGesture(reading.proximity, now)) return;
  }

  switch (state_) {
    case LampState::Off: handleOff(reading, now); break;
    case LampState::AutoAdjusting: handleAuto(now); break;
    case LampState::ManualLocked: handleLocked(reading.proximity, now); break;
    case LampState::ManualUnlocked: handleUnlocked(reading.proximity); break;
    case LampState::ColorPicker: handleColorPicker(reading.proximity, now); break;
  }
}
