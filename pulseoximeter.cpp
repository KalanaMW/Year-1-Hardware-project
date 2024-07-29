#include <Arduino.h>

#include "pulseoximeter.h"

PulseOximeter pox;
uint32_t tsLastReport = 0;
float heart_rate = 0;
float spo2 = 0;

void onBeatDetected()
{
  Serial.println("Beat!");
}

void initializePulseOximeter() {
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("PULSE OXIMETER FAILED");
        for(;;);
    } else {
        Serial.println("PULSE OXIMETER SUCCESS");
    }
    pox.setIRLedCurrent(MAX30102_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void updateOximeter() {
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      Serial.print("Heart rate:");
      heart_rate =  pox.getHeartRate();
      Serial.print(heart_rate);
      Serial.print("bpm / SpO2:");
      spo2 = pox.getSpO2();
      Serial.print(spo2);
      Serial.println("%");

      tsLastReport = millis();
  }
}

float getHeartRate() {
    return heart_rate;
}

int getSPO2() {
    return spo2;
}