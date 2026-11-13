#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint8_t LED_PIN = 6;
constexpr uint16_t NUM_LEDS = 60;
constexpr uint32_t SERIAL_BAUD = 115200;

constexpr uint8_t ON_MIN = 50;
constexpr uint8_t ON_MAX = 255;
constexpr uint8_t OFF_PROX = 254;
constexpr uint8_t COLOR_HOLD_MIN = 50;
constexpr uint8_t COLOR_HOLD_MAX = 200;
constexpr uint8_t COLOR_STEADY_TOL = 20;
constexpr uint8_t PICKER_PROX_MIN = 60;

constexpr uint32_t COLOR_HOLD_TIME_MS = 3000;
constexpr uint32_t ON_HOLD_TIME_MS = 1000;
constexpr uint32_t OFF_HOLD_TIME_MS = 1200;
constexpr uint32_t AUTO_TIME_MS = 3000;
constexpr uint32_t FADE_IN_DURATION_MS = 120;
constexpr uint32_t FADE_OUT_DURATION_MS = 120;
constexpr uint32_t PICKER_GRACE_PERIOD_MS = 2000;
constexpr uint32_t OFF_IGNORE_TIME_MS = 2000;
constexpr uint32_t LOOP_INTERVAL_MS = 40;

constexpr uint8_t DIMMEST = 10;
constexpr uint8_t BRIGHTEST = 255;
constexpr uint8_t PICKER_COLOR_COUNT = 5;
}  // namespace Config
