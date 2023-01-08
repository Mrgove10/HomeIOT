#include <Arduino.h>
#include <PubSubClient.h>
#include <wifi.h>

Wifi wifi_lib;

void setup()
{
  wifi_lib.Connect(1,2);
}

void loop()
{
  // put your main code here, to run repeatedly:
}