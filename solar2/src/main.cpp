#include <Wire.h>
#include "SDL_Arduino_INA3221.h"

SDL_Arduino_INA3221 ina3221;

// the three channels of the INA3221 
#define TEST_CHANNEL 1

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