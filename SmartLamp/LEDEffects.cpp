#define FASTLED_ALLOW_INTERRUPTS 0
#include "LEDEffects.h"

namespace {
const CRGB kPickerColors[Config::PICKER_COLOR_COUNT] = {
  CRGB::White, CRGB::DeepPink, CRGB::Gold,
  CRGB::BlueViolet, CRGB::Tomato
};
}

void LEDEffects::begin() {
  FastLED.addLeds<WS2812B, Config::LED_PIN, GRB>(leds_, Config::NUM_LEDS);
  FastLED.setBrightness(0);
  fill_solid(leds_, Config::NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void LEDEffects::render() {
  fill_solid(leds_, Config::NUM_LEDS, color_);
  FastLED.setBrightness(brightness_);
  FastLED.show();
}

void LEDEffects::setColor(const CRGB& color) {
  color_ = color;
  render();
}

void LEDEffects::setBrightness(uint8_t brightness) {
  fading_ = false;
  brightness_ = brightness;
  render();
}

void LEDEffects::startFade(uint8_t from, uint8_t to,
                           uint32_t durationMs, uint32_t now) {
  fadeFrom_ = from;
  fadeTo_ = to;
  fadeStart_ = now;
  fadeDuration_ = durationMs;
  brightness_ = from;
  fading_ = (durationMs > 0);
  if (!fading_) brightness_ = to;
  render();
}

void LEDEffects::update(uint32_t now) {
  if (!fading_) return;
  const uint32_t elapsed = now - fadeStart_;
  if (elapsed >= fadeDuration_) {
    brightness_ = fadeTo_;
    fading_ = false;
  } else {
    const int32_t delta = static_cast<int32_t>(fadeTo_) - fadeFrom_;
    brightness_ = static_cast<uint8_t>(
      static_cast<int32_t>(fadeFrom_) +
      delta * static_cast<int32_t>(elapsed) / static_cast<int32_t>(fadeDuration_));
  }
  render();
}

CRGB LEDEffects::pickerColor(uint8_t index) {
  if (index >= Config::PICKER_COLOR_COUNT) index = Config::PICKER_COLOR_COUNT - 1;
  return kPickerColors[index];
}

void LEDEffects::showPickerColor(uint8_t index, uint8_t brightness) {
  color_ = pickerColor(index);
  setBrightness(brightness);
}
