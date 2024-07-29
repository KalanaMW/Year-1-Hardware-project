#include <Arduino.h>

#include "Accelerometer.h"

Adafruit_MPU6050 mpu;

//This function initializes the MPU6050 sensor by calling the begin() method of the mpu object.

void initializeAccelerometer() {
  mpu.begin();
}

float getX() {
  /* Get new sensor events with the readings*/
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return a.acceleration.x;
}

float getY() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return a.acceleration.y;
}

float getZ() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  return a.acceleration.z;
}

float getRoll() {
  float accX = getX();
  float accY = getY();
  float accZ = getZ(); 

  // Serial.print("X: ");
  // Serial.print(accX);
  // Serial.print("Y: ");
  // Serial.print(accY);
  // Serial.print("Z: ");
  // Serial.println(accZ);
  
  return (atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI);
}

float getPitch() {
  float accX = getX();
  float accY = getY();
  float accZ = getZ(); 
  
  return (atan(-1 * accX / sqrt(pow(accY, 2) + pow(accZ, 2))) * 180 / PI);
}




