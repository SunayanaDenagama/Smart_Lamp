#pragma once

#include <Arduino.h>
#include <SparkFun_APDS9960.h>

struct SensorReading {
  uint8_t proximity = 0;
  uint16_t ambient = 0;
};

class GestureSensor {
 public:
  bool begin();
  bool read(SensorReading& reading);

 private:
  SparkFun_APDS9960 apds_;
  uint16_t lastAmbient_ = 0;
};
