#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient mqttClient;

extern int subscribedAngleOffset;
extern bool subscribedAngleOffsetChanged;
extern float subscribedGammaFactor;
extern bool subscribedGammaFactorChanged;

void setupMQTT();
void connectToBroker();
void recieveCallback(char *, byte *, unsigned int);
void mqtt_publish(char *, float);

#endif