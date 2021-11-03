#include "Arduino.h"
#include "SerialCom.h"
#include "Types.h"

particleSensorState_t state;

uint32_t statusPublishPreviousMillis = 0;
const uint16_t statusPublishInterval = 2000; // 30 seconds = 30000 milliseconds

void setup()
{
    Serial.begin(115200);
    SerialCom::setup();
}

void loop()
{
    SerialCom::handleUart(state);

    const uint32_t currentMillis = millis();
    if (currentMillis - statusPublishPreviousMillis >= statusPublishInterval)
    {
        statusPublishPreviousMillis = currentMillis;

        if (state.valid)
        {
            printf("Publishing state\n");
            // publishState();
        }
    }
}