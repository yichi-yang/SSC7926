#include <Arduino.h>

#include "SdFs.h"

const int bufferLen = 32768;

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

void setup()
{
    alignas(16) static volatile int16_t buffer[bufferLen];
    SdFs sd;
    FsFile file;

    Serial.begin(9600);
    while (!Serial)
    {
    }
    do
    {
        delay(10);
    } while (Serial.available() && Serial.read());

    Serial.println(
        "\nType '1' for FIFO SDIO"
        "\n     '2' for DMA SDIO"
        "\n     '3' for Dedicated SPI"
        "\n     '4' for Shared SPI");

    while (!Serial.available())
    {
    }
    char c = Serial.read();

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
    if (sizeof(buffer) != file.write((void *)buffer, sizeof(buffer)))
    {
        errorHalt("write failed", &sd);
    }

    file.close();

    Serial.println("Done");
}
//-----------------------------------------------------------------------------
void loop()
{
}