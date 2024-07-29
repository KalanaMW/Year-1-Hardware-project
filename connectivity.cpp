#include "connectivity.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

int subscribedAngleOffset = 0;
bool subscribedAngleOffsetChanged = false;
int subscribedForce = 0;
bool subscribedForceChanged = false;

void setupMQTT()
{
    mqttClient.setServer("test.mosquitto.org", 1883);
    mqttClient.setCallback(recieveCallback);
}

void connectToBroker()
{
    while (!mqttClient.connected())
    {
        Serial.println("Attempting MQTT connection...");
        if (mqttClient.connect("ESP32Client-542598752107583"))
        {
            Serial.println("MQTT Connected");
            mqttClient.subscribe("HWP_4312_ANGLE");
            mqttClient.subscribe("HWP_4312_Xyz");
            mqttClient.subscribe("HWP_4312_");
        }
        else
        {
            Serial.print("Failed To connect to MQTT Broker");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
}

void recieveCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char payloadCharArr[length];
    Serial.println("Message Received: ");

    for (int i = 0; i < length; i++)
    {
        // Serial.print((char)payload[i]);
        payloadCharArr[i] = (char)payload[i];
    }
    // Serial.println();

    if (strcmp(topic, "HWP_4312_") == 0)
    {
        subscribedAngleOffset = atoi(payloadCharArr);
        subscribedAngleOffsetChanged = true;
    }
    else if (strcmp(topic, "HWP_4312_") == 0)
    {
        subscribedForce = atoi(payloadCharArr);
        subscribedForceChanged = true;
    }
    else if (strcmp(topic, "HWP_4312_") == 0)
    {
        // Received data should be handled appropriately.
        // Detect which alarm, and assign it.
    }
}

void mqtt_publish(char *topic, float value)
{
    if ((int)value == value)
    {
        const int length = String(int(value)).length();
        char valueArr[length + 1];
        String(int(value)).toCharArray(valueArr, length + 1);
        mqttClient.publish(topic, valueArr);
    }
    else
    {
        const int length = String(value).length();
        char valueArr[length + 1];
        String(value).toCharArray(valueArr, length + 1);
        mqttClient.publish(topic, valueArr);
    }
}