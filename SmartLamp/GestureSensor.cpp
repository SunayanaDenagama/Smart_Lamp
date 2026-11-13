#include "GestureSensor.h"
#include <Wire.h>

bool GestureSensor::begin() {
  Wire.begin();
  if (!apds_.init()) return false;
  if (!apds_.enableProximitySensor(false)) return false;
  if (!apds_.enableLightSensor(false)) return false;
  apds_.setProximityGain(PGAIN_8X);
  return true;
}

bool GestureSensor::read(SensorReading& reading) {
  if (!apds_.readProximity(reading.proximity)) return false;
  uint16_t ambient;
  if (apds_.readAmbientLight(ambient)) lastAmbient_ = ambient;
  reading.ambient = lastAmbient_;
  return true;
}
