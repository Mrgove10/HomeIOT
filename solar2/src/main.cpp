#include <Wire.h>
#include "SDL_Arduino_INA3221.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//temperature sensor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char *ssid = "";
const char *password = "";
const char *mqtt_server = "192.168.1.38";

// wifi & mqtt
WiFiClient espClient;
PubSubClient client(espClient);

// current sensor
SDL_Arduino_INA3221 ina3221;

// the three channels of the INA3221 
#define TEST_CHANNEL 1

// calculation for execution time
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
long startMicros = 0;
long endMicros = 0;


void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.hostname("Esp-Solar-Panel");


  int retries = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    retries++;
    if (retries > 20)
    {
      ESP.restart();
    }
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(10);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Esp-Solar-Panel"))
    {
      Serial.println("Mqtt Connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

     

      // Wait 5 seconds before retrying
      delay(5000);
      ESP.restart();
    }
  }
}

void solarMQTTSend()
{
  //Getting Values
  int solar = analogRead(SOLAR_VOLT_SENS);
  int battery = analogRead(BAT_VOLT_SENS);
  int five = analogRead(FIVE_VOLT_SENS);

  Serial.print("Solar Voltage: ");
  Serial.println(solar);
  Serial.print("Battery Voltage: ");
  Serial.println(battery);
  Serial.print("5V Voltage: ");
  Serial.println(five);
  Serial.println("----");

  // create json string
  String JSON = "{\"solar\":" + String(solar) + ",\"battery\":" + String(battery) + ",\"five\":" + String(five) + "}";

  // public string to the mqtt topic
  Serial.println("publishing to mqtt");
  client.publish("solar", (char *)JSON.c_str());
}

void tempMQTTSend()
{
  //Getting Values
  sensors.begin();
  sensors.requestTemperatures();
  float Celcius = sensors.getTempCByIndex(0);

  Serial.print("Celcius : ");
  Serial.println(Celcius);

  // create json string
  String payload = "{\"t_out\":" + String(Celcius, 2) + "}";

  // public string to the mqtt topic
  Serial.println("publishing to mqtt");
  client.publish("govie-weather-station-1344-out", (char *)payload.c_str());
}

void setup()
{
  //get start time
  startMicros = micros();

  // Serial
  Serial.begin(115200);

  Serial.println(digitalRead(SW1));
  Serial.println(digitalRead(SW2));
  Serial.println(digitalRead(SW3));

  }


  // WIFI
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);
  if (client.connect("Esp-Solar-Panel"))
  {
    Serial.println("Mqtt Connected");
  }
  else
  {
    Serial.println("Mqtt ERROR");
  }

  //MQTT
  if (!client.connected())
  {
    Serial.print(" : ");
    Serial.print("mqtt_reconnect");
    reconnect();
  }
  client.loop();
  delay(100);

  solarMQTTSend();
  tempMQTTSend();

  // disconnect before deep sleep
  client.disconnect();
  WiFi.disconnect();

  //get end time
  endMicros = micros();

  // get the total execution time
  long executionTime = (endMicros - startMicros);
  // Serial.println("execution time :");
  // Serial.println(executionTime);
  // Serial.println(executionTime / uS_TO_S_FACTOR);
  // get the time the esp will sleep
  long sleepTime = (60 * uS_TO_S_FACTOR) - executionTime;
  Serial.println("Going to sleep for");
  Serial.println(sleepTime);
  // Serial.println(sleepTime / uS_TO_S_FACTOR);



  // set time to sleep then go to sleep
  // sleepTime = 2500;
  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
  delay(10);

  // delay(2500);
}

void loop()
{
  // Empty
}






void setup(void) 
{
    
  Serial.begin(9600);
  Serial.println("SDA_Arduino_INA3221_Test");
  
  Serial.println("Measuring voltage and current with ina3221 ...");
  ina3221.begin();

  Serial.print("Manufactures ID=0x");
  int MID;
  MID = ina3221.getManufID();
  Serial.println(MID,HEX);
}

void loop(void) 
{
  
  Serial.println("------------------------------");
  float shuntvoltage1 = 0;
  float busvoltage1 = 0;
  float current_mA1 = 0;
  float loadvoltage1 = 0;


  busvoltage1 = ina3221.getBusVoltage_V(TEST_CHANNEL);
  shuntvoltage1 = ina3221.getShuntVoltage_mV(TEST_CHANNEL);
  current_mA1 = ina3221.getCurrent_mA(TEST_CHANNEL);  
  loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage1); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage1); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage1); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA1); Serial.println(" mA");
  Serial.println("");

  delay(1000);
}