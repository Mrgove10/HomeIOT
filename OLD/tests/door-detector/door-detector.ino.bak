#pragma region defines
#define MQTT_MAX_PACKET_SIZE 256
#define BUTTON_PIN D5
#pragma endregion

#pragma region includes
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#pragma endregion

// Network details
const char *ssid = "Livebox-EC80";
const char *password = "gWbj6RNW6n3mCwwD3A";

PubSubClient mqttClient(espClient);
const char *mqttServerAdress = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char *mqttServerAdress = "test.mosquitto.org";

volatile bool buttonPressed = false;

void setup()
{
    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(BUTTON_PIN, ButtonPressed, RISING);

    Serial.begin(9600);
    WiFi.begin(ssid, password);
    WiFi.mode(WIFI_STA);
    WiFi.hostname("DoorSensor");

    //Wifi connected
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP()); // Printing the ESP IP address
    display.setCursor(0, 0);
    display.println("WiFi connected");
    display.setCursor(0, 10);
    display.println(WiFi.localIP());
    display.display();
    delay(2500);
    display.display();

    //Mqtt
    mqttClient.setServer(mqttServerAdress, 1883);

    // Attempt to connect to the server with the ID "myClientID"
    if (mqttClient.connect("govie-door-1344"))
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

void mqtt()
{
    if (!mqttClient.connected())
    {
        reconnect();
    }
    // Define
    String payload = "{\"dp\":" + String(temperature, 2) + "," +
                     "\"p\":" + String(pressure, 2) + "," +
                     "\"h_in\":" + String(humidity, 2) + "}";

    // This is needed at the top of the loop!
    mqttClient.loop();

    //publish the message
    if (mqttClient.publish("govie-weather-station-1344-in", (char *)payload.c_str()))
    {
        //	Serial.println("Publish message success");
    }
    /*else
	{
		Serial.println("Could not send message :(");
	}*/
}

void loop()
{
    mqtt() if (buttonPressed)
    {
        sendTelegramMessage();
    }
}

void ButtonPressed()
{
    Serial.println("ButtonPressed");
    int button = digitalRead(BUTTON_PIN);
    if (button == HIGH)
    {
        buttonPressed = true;
    }
    return;
}