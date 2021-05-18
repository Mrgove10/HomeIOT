#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// Update these with values suitable for your network.

#define SOLAR_VOLT_SENS 34
#define BAT_VOLT_SENS 39
#define FIVE_VOLT_SENS 36

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */

const char *mqtt_server = "192.168.1.38";

WiFiClient espClient;
PubSubClient client(espClient);

// calculation for execution time
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

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      // client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  //get start time
  startMicros = micros();
  pinMode(SOLAR_VOLT_SENS, INPUT);
  pinMode(BAT_VOLT_SENS, INPUT);
  pinMode(FIVE_VOLT_SENS, INPUT);

  Serial.begin(115200);

  // WIFI
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);

  // OTA Updates

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Esp-Solar-Panel");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
                       Serial.printf("Error[%u]: ", error);
                       if (error == OTA_AUTH_ERROR)
                         Serial.println("Auth Failed");
                       else if (error == OTA_BEGIN_ERROR)
                         Serial.println("Begin Failed");
                       else if (error == OTA_CONNECT_ERROR)
                         Serial.println("Connect Failed");
                       else if (error == OTA_RECEIVE_ERROR)
                         Serial.println("Receive Failed");
                       else if (error == OTA_END_ERROR)
                         Serial.println("End Failed");
                     });
  ArduinoOTA.begin();
  Serial.println("Ready");
}

void loop()
{
  // DO NOT REMOVE THIS
  ArduinoOTA.handle();

  //MQTT
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

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
  client.publish("out", (char *)JSON.c_str());

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
  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
  delay(10);
  //delay(60000);
}