#define MQTT_MAX_PACKET_SIZE 256
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#include <Arduino.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Network details
const char *ssid = "";
const char *password = "";

//Varriables
float humidity, pressure, temperature, altitude;
unsigned int serverTime;
bool bmeValid;
float SEALEVELPRESSURE_HPA = 1005;

Adafruit_BME280 bme;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char *mqttServerAdress = "192.168.1.38";

/**
 * startup loop, only runs once
 */
void setup()
{
	Serial.begin(9600);
	Wire.begin(D2, D1);
	timeClient.begin();
	WiFi.begin(ssid, password);
	WiFi.mode(WIFI_STA);
	WiFi.hostname("WeatherStationInside");
	bmeValid = bme.begin(0x76);

	if (!bmeValid)
	{
		Serial.println("Could not find a valid BME280 sensor, check wiring!");
		while (1)
			;
	}
	else
	{
		Serial.println("BME280 sensor is valid");
	}

	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // Address 0x3D for 128x64
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			;
	}

	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setRotation(2);

	//starting connection
	Serial.print("Connecting to ");
	Serial.println(ssid);
	display.setCursor(0, 0);
	display.println("Connecting wifi");
	display.setCursor(0, 10);
	display.println(ssid);
	display.display();
	display.setCursor(0, 20);
	while (WiFi.status() != WL_CONNECTED)
	{
		//Wait for the WiFI connection completion
		Serial.println("Waiting for connection");
		display.print(".");
		display.display();
		delay(500);
	}
	display.clearDisplay();

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

	Serial.print("Connecting to MQTT");
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

/*----------------------------------------------------------------------------------*/
// Calculates Sea Level Pressure (SLP) based on altitude, temperature and non-adjusted barometric pressure
/*----------------------------------------------------------------------------------*/
//
// based on following formula
//
// H = Altitude expressed in meters
// P = Atomospheric pressure in 1/10th of a mb or hPa
// T = Temperature in 1/10th of a degree celcius
//
//         /            0.0065 * H        \ -5.257
// SLP = P |1 - __________________________|
//         \    T +  0.0065 * H + 273.15  /
/* ------------------------------------------------------------------------------------ */

float SeaLevelPressure(float Pressure, float Temperature, float Altitude)
{

	bool Trace = false;

	double SLP = 0;
	double num = 0.0065F * Altitude;
	double denum = Temperature / 10.0 + 0.0065F * Altitude + 273.15F;

	SLP = Pressure / 10.0F * pow((1 - (num / denum)), -5.257F);

	if (Trace)
	{
		Serial.print(F("Press= "));
		Serial.println(Pressure);
		Serial.print(F("Temp= "));
		Serial.println(Temperature);
		Serial.print(F("Alt= "));
		Serial.println(Altitude);
		Serial.print(F("SLP= "));
		Serial.println(SLP, 4);
	}

	return SLP * 10.0f;
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
	String payload = "{\"t_in\":" + String(temperature, 2) + "," +
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

/**
 * Gets the current sensor values 
 */
void updateSensorValue()
{
	humidity = bme.readHumidity();
	temperature = bme.readTemperature();
	altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
	pressure = SeaLevelPressure((bme.readPressure() / 100.0F), temperature, altitude);
}

/**	
 * updates the curent epoch time from the server
 */
void getCurrentTime()
{
	timeClient.update();
	serverTime = timeClient.getEpochTime();
}

/**
 * prints the time to the serial monitor
 */
void printTimeSerial()
{
	Serial.print(year(serverTime));
	Serial.print(":");
	Serial.print(month(serverTime));
	Serial.print(":");
	Serial.print(day(serverTime));
	Serial.print(":");
	Serial.print(hour(serverTime));
	Serial.print(":");
	Serial.print(minute(serverTime));
	Serial.print(":");
	Serial.print(second(serverTime));
}

/**
 * Print teh sensor values to the serial port
 */
void printValuesSerial()
{
	Serial.print("Temperature = ");
	Serial.print(temperature);
	Serial.println(" *C");

	Serial.print("Pressure = ");
	Serial.print(pressure);
	Serial.println(" hPa");

	Serial.print("Approx. Altitude = ");
	Serial.print(altitude);
	Serial.println(" m");

	Serial.print("Humidity = ");
	Serial.print(humidity);
	Serial.println(" %");

	Serial.println();
}

/**
 * Displays the sensor values on the screen
 */
void updateLCD()
{
	display.clearDisplay();
	display.setTextSize(1);

	// humidity
	display.setCursor(0, 15);
	display.print(humidity);
	display.print("%");

	// pressure
	display.setCursor(0, 25);
	display.print(pressure);
	display.print("hpa");

	// time
	display.setTextSize(2);
	display.setCursor(0, 0);
	if (hour(serverTime) < 10)
	{
		display.print("0");
	}
	display.print(hour(serverTime));
	display.print(":");
	if (minute(serverTime) < 10)
	{
		display.print("0");
	}
	display.print(minute(serverTime));

	// temperature
	display.setTextSize(5);
	display.setCursor((display.width() / 2) + 3, 0);
	display.print((int)temperature);
	display.setTextSize(2);
	display.setCursor((display.width() / 2) + 3, (display.height() - 15));
	display.print(temperature);

	//line diff
	display.drawLine(((display.width() - 2) / 2), 0, ((display.width() - 2) / 2), display.height(), WHITE);
	display.display();
}

/**
 * Main loop of teh software
 */
void loop()
{
	getCurrentTime();
	printTimeSerial();
	updateSensorValue();
	printValuesSerial();
	mqtt();
	updateLCD();
	delay(60000);
}
