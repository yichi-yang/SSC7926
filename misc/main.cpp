#include <Arduino.h>
#include "ClockGenerator.h"
#include "ADC.h"
#include "BufferDMA.h"

const int readPin = A9;
const int bufferLen = 4096;

elapsedMillis time;

// ClockGenerator clock;

void setup()
{
    bool second_half = false;
    ADC *adc = new ADC();
    alignas(8) static volatile int16_t buffer[bufferLen];
    alignas(8) static volatile int16_t readout_buffer[bufferLen / 2];
    BufferDMA dmaBuffer(buffer, bufferLen, ADC_0);
    pinMode(readPin, INPUT);
    Serial.begin(9600);

    delay(5000);
    Serial.println("OK");
    while (!Serial.available())
    {
    }

    adc->setAveraging(8);
    adc->setResolution(12);
    adc->enableDMA(ADC_0);
    Serial.print("&buffer=");
    Serial.println((uint32_t)buffer, HEX);
    dmaBuffer.start();

    memset((void *)buffer, 0x0, sizeof(buffer));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < bufferLen; j++)
        {
            // adc->adc0->startSingleRead(readPin);
            adc->analogRead(readPin, ADC_0);
            time = 0;
            while (time < 100)
            {
                if (data_ready)
                {
                    if (data_ready > 1)
                    {
                        Serial.print("Buffer overrun");
                        for (;;)
                            ;
                    }
                    Serial.print("Read buffer: ");
                    memcpy((void *)readout_buffer, (void *)(buffer + second_half * bufferLen / 2), sizeof(readout_buffer));
                    memset((void *)(buffer + second_half * bufferLen / 2), 0x0, sizeof(readout_buffer));
                    for (int k = 0; k < bufferLen / 2; k++)
                    {
                        Serial.printf("%6hu", readout_buffer[k]);
                    }
                    Serial.println();
                    data_ready--;
                    second_half = !second_half;
                }
            }
            for (int k = 0; k < bufferLen; k++)
            {
                Serial.printf("%6hu", buffer[k]);
            }
            Serial.printf("      [%2d, %2d]\n", second_half, dmaChannel->TCD->CITER);
        }
        Serial.printf("RESET\n");
    }
    Serial.printf("END\n");
}

void loop()
{
    // put your main code here, to run repeatedly:
}