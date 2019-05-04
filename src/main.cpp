#include <Arduino.h>
#include "ClockGenerator.h"
#include "ADC.h"
#include "BufferDMA.h"
#include "SdFs.h"

#define PGM_HEADER "P5 7926 7926 4095\n"

const int bufferLen = 16384;

void errorHalt(const char *msg, SdFs *sd);

void setup()
{
    ADC *adc = new ADC();
    alignas(8) static volatile int16_t buffer[bufferLen];
    memset((void *)buffer, 0x0, sizeof(buffer));
    BufferDMA dmaBuffer(buffer, bufferLen, ADC_0);
    ClockGenerator clock(adc);
    SdFs sd;
    FsFile file;

    pinMode(PIN_STEP_BTN, INPUT_PULLUP);
    pinMode(PIN_DIR_BTN, INPUT_PULLUP);
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWriteFast(PIN_STEP, LOW);
    digitalWriteFast(PIN_DIR, LOW);
    digitalWriteFast(LED_BUILTIN, LOW);
    delayMicroseconds(200);

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
        while (digitalReadFast(PIN_STEP_BTN) == LOW)
        {
            digitalWriteFast(LED_BUILTIN, HIGH);
            digitalWriteFast(PIN_DIR, digitalReadFast(PIN_DIR_BTN));
            delayMicroseconds(500);
            digitalWriteFast(PIN_STEP, HIGH);
            delayMicroseconds(3);
            digitalWriteFast(PIN_STEP, LOW);
            delayMicroseconds(500);
        }
        digitalWriteFast(LED_BUILTIN, LOW);
    }

    digitalWriteFast(PIN_DIR, LOW);

    clock.setPeriod(3200);
    clock.config(7926, 8050, 1812);

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

    if (!file.open("TeensyDemo01.pgm", O_RDWR | O_CREAT))
    {
        errorHalt("open failed", &sd);
    }
    if (!file.truncate(0))
    {
        errorHalt("truncate failed", &sd);
    }

    if (sizeof(PGM_HEADER) != file.write((void *)(PGM_HEADER), sizeof(PGM_HEADER)))
    {
        errorHalt("write failed", &sd);
    }

    clock.enable();

    while (clock.is_active())
    {
        if (digitalReadFast(PIN_STEP_BTN) == LOW)
        {
            clock.disable();
            file.close();
            Serial.println("halt");
            while (true)
                ;
        }
        if (dmaBuffer.ready() == READY)
        {
            Serial.print(".");
            if ((sizeof(buffer) / 2) != file.write((void *)(dmaBuffer.read_half()), sizeof(buffer) / 2))
            {
                errorHalt("write failed", &sd);
            }
        }
        else if (dmaBuffer.ready() == OVERRUN)
        {
            Serial.println("buffer overrun");
            while (true)
                ;
        }
    }

    delay(100);

    if (dmaBuffer.ready() == READY)
    {
        Serial.print(".");
        if ((sizeof(buffer) / 2) != file.write((void *)(dmaBuffer.read_half()), sizeof(buffer) / 2))
        {
            errorHalt("write failed", &sd);
        }
    }
    else if (dmaBuffer.ready() == OVERRUN)
    {
        Serial.println("buffer overrun");
        while (true)
            ;
    }

    if (sizeof(int16_t) * dmaBuffer.remain() != file.write((void *)(dmaBuffer.read_half()), sizeof(int16_t) * dmaBuffer.remain()))
    {
        errorHalt("write failed", &sd);
    }

    Serial.println((int)dmaBuffer.remain());

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