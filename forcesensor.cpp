#include <Arduino.h>

#include "forcesensor.h"

int getRawForceSensor() {
  return analogRead(FORCESENSOR_PIN);
}