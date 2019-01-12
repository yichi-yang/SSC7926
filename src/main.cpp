#include <Arduino.h>
#include "ClockGenerator.h"
#include "ADC.h"
#include "BufferDMA.h"

const int readPin = A9;
const int bufferLen = 16;

// ClockGenerator clock;

void setup()
{
    ADC *adc = new ADC();
    alignas(8) static volatile int16_t buffer[bufferLen];
    BufferDMA dmaBuffer(buffer, bufferLen, ADC_0);
    pinMode(readPin, INPUT);
    Serial.begin(9600);
    adc->setAveraging(8);
    adc->setResolution(12);
    adc->enableDMA(ADC_0);
    dmaBuffer.start();
    for (int i = 0; i < 3; i++)
    {
        memset((void *)buffer, 0, sizeof(buffer));
        for (int j = 0; j < bufferLen; j++)
        {
            // adc->adc0->startSingleRead(readPin);
            adc->analogRead(readPin, ADC_0);
            delay(1000);
            for (int k = 0; k < bufferLen; k++)
            {
                Serial.printf("%5d", buffer[k]);
            }
            Serial.printf("\n");
        }
        Serial.printf("RESET\n");
    }
    Serial.printf("END\n");
}

void loop()
{
    // put your main code here, to run repeatedly:
}