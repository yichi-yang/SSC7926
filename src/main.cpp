#include <Arduino.h>
#include "ClockGenerator.h"
#include "ADC.h"
#include "BufferDMA.h"
#include "SdFs.h"

const int readPin = A9;
const int bufferLen = 16384;

void errorHalt(const char *msg, SdFs *sd);

void setup()
{
    bool second_half = false;
    ADC *adc = new ADC();
    alignas(8) static volatile int16_t buffer[bufferLen];
    memset((void *)buffer, 0x0, sizeof(buffer));
    BufferDMA dmaBuffer(buffer, bufferLen, ADC_0);
    ClockGenerator clock(adc);
    SdFs sd;
    FsFile file;

    Serial.begin(115200);
    while (!Serial)
    {
    }
    do
    {
        delay(10);
    } while (Serial.available() && Serial.read());
    Serial.println("OK");
    while (!Serial.available())
    {
    }

    clock.setPeriod(3600);
    clock.config(64, bufferLen / 2, 16);

    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc->setAveraging(1);
    adc->setResolution(12);

    adc->enableDMA(ADC_0);
    Serial.print("&buffer=");
    Serial.println((uint32_t)buffer, HEX);
    dmaBuffer.start();

    if (!sd.begin(SdioConfig(FIFO_SDIO)))
    {
        errorHalt("begin failed", &sd);
    }
    Serial.println("\nFIFO SDIO mode.");

    if (!file.open("TeensyDemo.bin", O_RDWR | O_CREAT))
    {
        errorHalt("open failed", &sd);
    }
    if (!file.truncate(0))
    {
        errorHalt("truncate failed", &sd);
    }

    clock.enable();

    while (clock.is_active())
    {
        if (BufferDMA::data_ready)
        {
            if (BufferDMA::data_ready > 1)
            {
                Serial.print("Buffer overrun");
                for (;;)
                    ;
            }
            Serial.println("Data");
            if ((sizeof(buffer) / 2) != file.write((void *)(buffer + second_half * bufferLen / 2), sizeof(buffer) / 2))
            {
                errorHalt("write failed", &sd);
            }
            // Serial.println("SD");
            // Serial.print("Read buffer: ");
            // memcpy((void *)readout_buffer, (void *)(buffer + second_half * bufferLen / 2), sizeof(readout_buffer));
            // // memset((void *)(buffer + second_half * bufferLen / 2), 0x0, sizeof(readout_buffer));
            // for (int k = 0; k < bufferLen / 2; k++)
            // {
            //     Serial.printf("%6hu", readout_buffer[k]);
            // }
            // Serial.println();
            BufferDMA::data_ready--;
            second_half = !second_half;
        }
    }

    delay(100);

    if (BufferDMA::data_ready)
    {
        if (BufferDMA::data_ready > 1)
        {
            Serial.print("Buffer overrun");
            for (;;)
                ;
        }
        Serial.println("Data+");
        if ((sizeof(buffer) / 2) != file.write((void *)(buffer + second_half * bufferLen / 2), sizeof(buffer) / 2))
        {
            errorHalt("write failed", &sd);
        }
        // Serial.println("SD");
        // Serial.print("Read buffer: ");
        // memcpy((void *)readout_buffer, (void *)(buffer + second_half * bufferLen / 2), sizeof(readout_buffer));
        // // memset((void *)(buffer + second_half * bufferLen / 2), 0x0, sizeof(readout_buffer));
        // for (int k = 0; k < bufferLen / 2; k++)
        // {
        //     Serial.printf("%6hu", readout_buffer[k]);
        // }
        // Serial.println();
        BufferDMA::data_ready--;
        second_half = !second_half;
    }

    file.close();
    Serial.println("finished");
}

void loop()
{
    // put your main code here, to run repeatedly:
}

void errorHalt(const char *msg, SdFs *sd)
{
    Serial.print("Error: ");
    Serial.println(msg);
    if (sd->sdErrorCode())
    {
        if (sd->sdErrorCode() == SD_CARD_ERROR_ACMD41)
        {
            Serial.println("Try power cycling the SD card.");
        }
        printSdErrorSymbol(&Serial, sd->sdErrorCode());
        Serial.print(", ErrorData: 0X");
        Serial.println(sd->sdErrorData(), HEX);
    }
    while (true)
    {
    }
}