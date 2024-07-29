#ifndef PULSEOXIMETER_H
#define PULSEOXIMETER_H

#define REPORTING_PERIOD_MS 100

// #include <Arduino.h>
#include <Wire.h>
#include "MAX30102_PulseOximeter.h"

extern PulseOximeter pox;

void onBeatDetected();
void initializePulseOximeter();
void updateOximeter();
float getHeartRate();
int getSPO2();

#endif