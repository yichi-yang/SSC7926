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
    memset((void *)buffer, 0x0, sizeof(buffer));
    BufferDMA dmaBuffer(buffer, bufferLen, ADC_0);
    ClockGenerator clock(adc);
    clock.setPeriod(13);
    clock.config(128, bufferLen / 2, 16);
    Serial.begin(115200);
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc->setAveraging(1);
    adc->setResolution(16);

    delay(5000);
    Serial.println("OK");
    while (!Serial.available())
    {
    }

    adc->enableDMA(ADC_0);
    Serial.print("&buffer=");
    Serial.println((uint32_t)buffer, HEX);
    dmaBuffer.start();
    clock.enable();

    time = 0;
    while (true)
    {
        if (BufferDMA::data_ready)
        {
            if (BufferDMA::data_ready > 1)
            {
                Serial.print("Buffer overrun");
                for (;;)
                    ;
            }
            Serial.print("Read buffer: ");
            memcpy((void *)readout_buffer, (void *)(buffer + second_half * bufferLen / 2), sizeof(readout_buffer));
            // memset((void *)(buffer + second_half * bufferLen / 2), 0x0, sizeof(readout_buffer));
            for (int k = 0; k < bufferLen / 2; k++)
            {
                Serial.printf("%6hu", readout_buffer[k]);
            }
            Serial.println();
            BufferDMA::data_ready--;
            second_half = !second_half;
        }
    }

    // for (int i = 0; i < 3; i++)
    // {
    //     for (int j = 0; j < bufferLen; j++)
    //     {
    //         // adc->adc0->startSingleRead(readPin);
    //         adc->analogRead(readPin, ADC_0);
    //         time = 0;
    //         while (time < 100)
    //         {
    //             if (data_ready)
    //             {
    //                 if (data_ready > 1)
    //                 {
    //                     Serial.print("Buffer overrun");
    //                     for (;;)
    //                         ;
    //                 }
    //                 Serial.print("Read buffer: ");
    //                 memcpy((void *)readout_buffer, (void *)(buffer + second_half * bufferLen / 2), sizeof(readout_buffer));
    //                 memset((void *)(buffer + second_half * bufferLen / 2), 0x0, sizeof(readout_buffer));
    //                 for (int k = 0; k < bufferLen / 2; k++)
    //                 {
    //                     Serial.printf("%6hu", readout_buffer[k]);
    //                 }
    //                 Serial.println();
    //                 data_ready--;
    //                 second_half = !second_half;
    //             }
    //         }
    //         for (int k = 0; k < bufferLen; k++)
    //         {
    //             Serial.printf("%6hu", buffer[k]);
    //         }
    //         Serial.printf("      [%2d, %2d]\n", second_half, dmaChannel->TCD->CITER);
    //     }
    //     Serial.printf("RESET\n");
    // }
    // Serial.printf("END\n");
}

void loop()
{
    // put your main code here, to run repeatedly:
}