#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFi.h>
// #include <wifi.h>

// Wifi wifi_lib;

PubSubClient mqttClient;
// calculation for execution time
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
long startMicros = 0;
long endMicros = 0;

String WIFI_SSID = "";
String WIFI_PASSWORD = "";
String HOSTNAME = "AIOT_tempsensor";
String MQTT_BROKER_IP = "192.168.1.30";
String MQTT_TOPIC = "my_topic";

void generic_setup()
{
  // GENERIC METHODES
  // Serial methodes
  Serial.begin(115200);
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
  mqttClient.setServer((char *)MQTT_BROKER_IP.c_str(), 1883);
}
void mqtt(String payload_string)
{
  // if not connected
  if (!mqttClient.connected())
  {
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (mqttClient.connect((char *)HOSTNAME.c_str()))
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
  }

  // when connected
  Serial.println(payload_string);

  // This is needed at the top of the loop!
  mqttClient.loop();

  // publish the message
  if (mqttClient.publish((char *)MQTT_TOPIC.c_str(), (char *)payload_string.c_str()))
  {
    Serial.println("Publish message success");
  }
  else
  {
    Serial.println("Could not send message :(");
  }
}

void inteligence()
{
  // TODO : get values
  String payload_string = "{}";
  mqtt(payload_string);
}

void generic_setup_end()
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
  generic_setup_end();
}

void loop()
{
  // Not used
}