#include <Arduino.h>
#include "SerialCom.h"
#include "Types.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

particleSensorState_t state;

uint32_t statusPublishPreviousMillis = 0;
const uint16_t statusPublishInterval = 60000; // 30 seconds = 30000 milliseconds

// Network details
const char *ssid = "";
const char *password = "";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char *mqttServerAdress = "192.168.1.38";

// Mqtt reconnect function
void reconnect()
{
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect("ESP8266 Client"))
        {
            Serial.println("connected");
            // ... and subscribe to topic
            mqttClient.subscribe("testTopic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

// mqtt send message
void mqtt()
{
    if (!mqttClient.connected())
    {
        reconnect();
    }
    // Define
    String payload = "{\"qual\":" + String(state.avgPM25) + "}";
    Serial.println(payload);

    // This is needed at the top of the loop!
    mqttClient.loop();

    // publish the message
    if (mqttClient.publish("govie-air-qual", (char *)payload.c_str()))
    {
        Serial.println("Publish message success");
    }
    else
    {
        Serial.println("Could not send message :(");
    }
}

void setup()
{
    Serial.begin(115200);
    SerialCom::setup();

    // WIFI
    WiFi.begin(ssid, password);
    WiFi.mode(WIFI_STA);
    WiFi.hostname("WeatherStationInside");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Wait for the WiFI connection completion
        Serial.println("Waiting for connection");
        delay(500);
    }
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP()); // Printing the ESP IP address

    // mqtt
    mqttClient.setServer(mqttServerAdress, 1883);
}

void loop()
{
    SerialCom::handleUart(state);

    const uint32_t currentMillis = millis();
    if (currentMillis - statusPublishPreviousMillis >= statusPublishInterval)
    {
        statusPublishPreviousMillis = currentMillis;

        if (state.valid)
        {
            Serial.print(state.avgPM25);
            mqtt();
        }
    }
}