#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

extern Adafruit_MPU6050 mpu;

void initializeAccelerometer();
float getX();
float getY();
float getZ();
float getRoll();
float getPitch();

#endif