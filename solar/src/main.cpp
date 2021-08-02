#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Voltage input pins
#define SOLAR_VOLT_SENS 32
#define BAT_VOLT_SENS 34
#define FIVE_VOLT_SENS 35

// switch pins
#define SW1 33
#define SW2 25
#define SW3 26

// temperatuer sensor
#define ONE_WIRE_BUS 23

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//boolean to say if we boot with screen on or off
bool bootScreen = false;

const char *mqtt_server = "192.168.1.38";

// wifi & mqtt
WiFiClient espClient;
PubSubClient client(espClient);

// screen
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  if (bootScreen == true)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("connecting wifi");
    display.display();
    delay(2000);
  }

  int retries = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (bootScreen == true)
    {
      display.print(".");
      display.display();
    }
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

  if (bootScreen == true)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi connected");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);
  }
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

      if (bootScreen == true)
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("mqtt ERROR");
        display.println("failed, rc=");
        display.println(client.state());
        display.display();
      }

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

  // voltage pins
  pinMode(SOLAR_VOLT_SENS, INPUT);
  pinMode(BAT_VOLT_SENS, INPUT);
  pinMode(FIVE_VOLT_SENS, INPUT);

  // button pins
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  // Serial
  Serial.begin(115200);

  Serial.println(digitalRead(SW1));
  Serial.println(digitalRead(SW2));
  Serial.println(digitalRead(SW3));

  // boot in screen mode if SW1 is on when boot
  if (true) //digitalRead(SW1) == LOW)
  {
    Serial.println("Booting with screen on");
    bootScreen = true;

    // check if we have the display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;)
        ; // Don't proceed, loop forever
    }
    else
    {
      Serial.println("Found LCD");
    }

    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.print("booting");
    display.display();
  }
  else
  {
    Serial.println("Booting without screen off");
  }

  // WIFI
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);
  if (client.connect("Esp-Solar-Panel"))
  {
    Serial.println("Mqtt Connected");
    if (bootScreen == true)
    {
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("mqtt connected");
      display.display();
      delay(2000);
    }
  }
  else
  {
    Serial.println("Mqtt ERROR");
    if (bootScreen == true)
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("mqtt ERROR");
      display.display();
      delay(2000);
    }
  }
  /*
  // OTA Updates

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Esp-Solar-Panel");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() { Serial.println("Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error) {
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

  // DO NOT REMOVE THIS
  ArduinoOTA.handle();
*/
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

  // if (bootScreen)
  // {
  //   display.clearDisplay();
  //   display.setTextColor(SSD1306_WHITE);
  //   display.setCursor(0, 0);
  //   display.println(solar);
  //   display.println(battery);
  //   display.println(five);
  //   display.println(sleepTime);
  //   display.display();
  // }

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