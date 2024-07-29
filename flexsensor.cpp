#include <Arduino.h>
#include "flexsensor.h"

int getRawFlexSensor() {
  return analogRead(FLEXSENSOR_PIN);
}