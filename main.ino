#include <Arduino.h>
#include <EEPROM.h>
#include "Accelerometer.h"
#include "flexsensor.h"
#include "forcesensor.h"
#include "pulseoximeter.h"
#include "connectivity.h"

#define SSID "Galaxy A12 F797"
#define PASSWORD "kalana123"

#define EEPROM_SIZE 8

float CRITICAL_ANGLE = 70;
float CRITICAL_FORCE = 300;
float CRITICAL_BEND_LOW = 1600;
float CRITICAL_BEND_HIGH = 1750;

float ANGLE_TIME = 10;
float FORCE_TIME = 10;
float BEND_TIME = 10;
unsigned long long int angle_time, force_time, bend_time;

#define MOTOR_PIN 2
#define LED_PIN 15
#define PUSH_BUTTON_PIN 36

unsigned long long int vibration_time;
bool vibrating = false;
bool angle_alert = false, bend_alert = false, force_alert = false;
unsigned int updatedTime;

int mode = 0; // 0 - Normal (Posture Corrector), 1 - SPO2, Heart Rate

void giveAlert();
void pulseOximeterMode();

void setup() {
  // put your setup code here, to run once:
  // delay(1000);
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(PUSH_BUTTON_PIN, INPUT);
  pinMode(FLEXSENSOR_PIN, INPUT);
  pinMode(FORCESENSOR_PIN, INPUT);

  digitalWrite(LED_PIN, HIGH);

  WiFi.begin(SSID, PASSWORD, 6);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected");

  // Get stored mode in the EEPROM
  EEPROM.get(0, mode);

  Serial.print("Initial mode: ");
  Serial.println(mode);
  delay(2000);

  digitalWrite(LED_PIN, LOW);
  
  // Check mode, and if 1, go to pulseOximeter mode,
  // Once done change the mode again, and restart ESP.
  if (mode == 1) {
    Serial.print("In setup mode: ");
    Serial.println(mode);
    delay(1000);
    pulseOximeterMode();

    EEPROM.put(0, (int) 0);
    EEPROM.commit();

    delay(100);
    ESP.restart();
  }

  setupMQTT();

  initializeAccelerometer();

  angle_time = millis();
  force_time = millis();
  bend_time = millis();
  vibration_time = millis();
}

void loop() {
  // Check for push button press for Pulse Oximeter mode.
  int button_status = digitalRead(PUSH_BUTTON_PIN);

  if (button_status == HIGH) {
    delay(200); // Bounce-back

    Serial.print("Button stauts: ");
    Serial.println(button_status);

    EEPROM.put(0, (int) 1); // Set mode to 1.
    EEPROM.commit();

    delay(100);
    ESP.restart();
  }
  
  // int mode2;
  // EEPROM.get(0, mode2);
  // Serial.print("Currently in EEPROM: ");
  // Serial.println(mode2);

  // Get angle from accelerometer.
  float angle = getRoll();
  if (angle >= CRITICAL_ANGLE) {
    angle_time = millis();
  }
  
  // If angle is lower the limit for given time, give alerts.
  if ((millis() - angle_time) / 1000 >= ANGLE_TIME) {
    angle_alert = true;
  }
  else {
    angle_alert = false;
  }

  // Get angle from flex sensor.
  float bend = getRawFlexSensor();
  if (bend <= CRITICAL_BEND_HIGH && bend >= CRITICAL_BEND_LOW) {
    bend_time = millis();
  }

  // If bent amount is higher than a given amount, give alerts.
  if ((millis() - bend_time) / 1000 >= BEND_TIME) {
    bend_alert = true;
  }
  else {
    bend_alert = false;
  }

  // Get force from force sensor.
  float force = getRawForceSensor();
  if (force <= CRITICAL_FORCE) {
    force_time = millis();
  }

  // If bent amount is higher than a given amount, give alerts.
  if ((millis() - force_time) / 1000 >= FORCE_TIME) {
    force_alert = true;
  }
  else {
    force_alert = false;
  }

  if (angle_alert || bend_alert || force_alert) {
    giveAlert();
  } else {
    digitalWrite(MOTOR_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }  

  if (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker...");
    connectToBroker();
  }

  mqttClient.loop();

  mqtt_publish("HWP_4312_ROLL", angle);
  // mqtt_publish("HWP_4312_TIME", (millis() - angle_time) / 1000);
  mqtt_publish("HWP_4312_BEND", bend);
  mqtt_publish("HWP_4312_FORCE", force);
} 

void giveAlert() {
  // Turn the motor.
  if (millis() - vibration_time >= 1000) {
    if (!vibrating) {
      // digitalWrite(MOTOR_PIN, HIGH);
      vibrating = true;
    } else if (vibrating) {
      // digitalWrite(MOTOR_PIN, LOW);
      vibrating = false;
    }
    digitalWrite(MOTOR_PIN, vibrating);
    digitalWrite(LED_PIN, vibrating==0);
    
    vibration_time = millis();
  }
}

void pulseOximeterMode() {
  // int mode2;
  // EEPROM.get(0, mode2);
  // Serial.print("Currently in EEPROM: ");
  // Serial.println(mode2);

  float heart_rate = 0;
  int spo2 = 0;

  const int num_samples = 100;
  int i = 0;

  float heart_rates[num_samples];
  int spo2s[num_samples];

  initializePulseOximeter();
  unsigned long startTime = millis();

  digitalWrite(LED_PIN, HIGH);
  // delay(1000);
  // For 10 seconds, this will collect oximeter values.
  while (millis() - startTime <= 10000 && i < num_samples) {
    // Serial.println("In the loop....");
    updateOximeter();
    heart_rate = getHeartRate();
    spo2 = getSPO2();

    if (millis() % 200 == 0) { // Only retain value in every 50ms.
      heart_rates[i] = heart_rate;
      spo2s[i] = spo2;
      // Serial.print("Heart rate: ");
      // Serial.print(heart_rate);
      // Serial.print("\t SPO2: ");
      // Serial.println(spo2);
      i++;
    }
  }

  float sum_heart_rate = 0;
  int sum_spo2 = 0;
  for (int j = 0; j < i; j++) {
    sum_heart_rate += heart_rates[j];
    sum_spo2 += spo2s[j];
  }

  heart_rate = sum_heart_rate / i;
  spo2 = sum_spo2 / i;

  Serial.print("THIS IS THE AVERAGE VALUE :::::::::::::::::::::::::::::::::: \t\t\t Heart rate: ");
  Serial.print(heart_rate);
  Serial.print("\t SPO2: ");
  Serial.println(spo2);

  setupMQTT();

  if (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker...");
    connectToBroker();
  }
  
  mqttClient.loop();

  mqtt_publish("HWP_4312_HR", heart_rate);
  mqtt_publish("HWP_4312_SPO2", spo2);

  digitalWrite(LED_PIN, LOW);
  delay(1000);
}


