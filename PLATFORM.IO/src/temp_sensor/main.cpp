#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

// #include <wifi.h>

// Wifi wifi_lib;

// Cangeable values
char *WIFI_SSID = "Livebox-EC80";
char *WIFI_PASSWORD = "gWbj6RNW6n3mCwwD3A";
char *HOSTNAME = "AIOT_tempsensor";
char *MQTT_BROKER_IP = "192.168.1.30";
char *MQTT_TOPIC = "my_topic";

// Wifi
WiFiClient espClient;

// MQTT
PubSubClient mqttClient;

// calculation for execution time
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
long startMicros = 0;
long endMicros = 0;

// sensor
Adafruit_AHTX0 aht;

void generic_setup()
{
  // GENERIC METHODES
  // Serial methodes
  Serial.begin(115200);

  // I2C methodes
  Wire.begin(D7, D6); // TODO :change this if needed

  // Wifi setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  while (WiFi.status() != WL_CONNECTED)
  {
    // Wait for the WiFI connection completion
    Serial.println("Waiting for connection");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); // Printing the ESP IP address

  // mqtt setup
  mqttClient.setServer(MQTT_BROKER_IP, 1883);
  mqttClient.setClient(espClient);
}

void mqtt(String payload_string)
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(HOSTNAME))
    {
      Serial.println("connected");
      // ... and subscribe to topic
      // mqttClient.subscribe("testTopic");
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

  // This is needed at the top of the loop!
  mqttClient.loop();

  Serial.println("Message :");
  Serial.println(payload_string);

  Serial.print("Attempting MQTT publish...");

  // publish the message
  if (mqttClient.publish(MQTT_TOPIC, (char *)payload_string.c_str()))
  {
    Serial.print("publish message success");
  }
  else
  {
    Serial.print("could not send message :(");
  }
}

void inteligence()
{
  sensors_event_t humidity, temp;
  aht.begin();
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degrees C");
  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println("% rH");
  String payload_string = "{}";
  mqtt(payload_string);
}

void generic_end()
{
  // disconnect before deep sleep
  // client.disconnect();
  WiFi.disconnect();

  Serial.println("Going to sleep");
  Serial.println(30e6);
  // set time to sleep then go to sleep
  ESP.deepSleep(30e6);
  // esp_sleep_enable_timer_wakeup();
  // esp_deep_sleep_start();
  delay(10);
}

void setup()
{
  generic_setup();
  inteligence();
  generic_end();
}

void loop()
{
  // Not used
}