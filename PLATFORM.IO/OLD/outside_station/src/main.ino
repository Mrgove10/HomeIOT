#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char *mqttServerAdress = "192.168.1.38";

const char *ssid = "";
const char *password = "";

void setup(void)
{
  Serial.begin(115200);
  sensors.begin();

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("WeatherStationOutside");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    //Wait for the WiFI connection completion
    Serial.println("Waiting for connection");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); // Printing the ESP IP address
  delay(2500);

  //Mqtt
  mqttClient.setServer(mqttServerAdress, 1883);

  // Attempt to connect to the server with the ID "myClientID"
  if (mqttClient.connect("govie-weather-station-1344"))
  {
    Serial.println("Connection has been established, well done");

    // Establish the subscribe event
    //	mqttClient.setCallback(subscribeReceive);
  }
  else
  {
    Serial.println("Looks like the server connection failed...");
  }
}
//Mqtt reconnect function
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

//mqtt send message
void mqtt()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  // Define
  String payload = "{\"t_out\":" + String(Celcius, 2) + "}";

  //publish the message
  if (mqttClient.publish("govie-weather-station-1344-out", (char *)payload.c_str()))
  {
    //	Serial.println("Publish message success");
  }
  /*else
	{
		Serial.println("Could not send message :(");
	}*/
}

void loop(void)
{
  // This is needed at the top of the loop!
  mqttClient.loop();
  sensors.requestTemperatures();
  Celcius = sensors.getTempCByIndex(0);
  Serial.println(Celcius);
  mqtt();
  delay(60000);
}