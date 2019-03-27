#include "Arduino.h"
#include "BufferDMA.h"
#include "ADC.h"

volatile int8_t BufferDMA::data_ready = 0;
DMAChannel *BufferDMA::dmaChannel = nullptr;
volatile int16_t *BufferDMA::p_elems = nullptr;
uint16_t BufferDMA::b_size = 0;
uint8_t BufferDMA::ADC_number = 0;
volatile uint32_t *BufferDMA::ADC_RA = nullptr;

// Constructor
BufferDMA::BufferDMA(volatile int16_t *buffer, uint32_t len, uint8_t ADC_num)
{
    p_elems = buffer;
    b_size = len;
    ADC_number = ADC_num;
    ADC_RA = &ADC0_RA + (uint32_t)0x20000 * ADC_number;
    if (dmaChannel == nullptr)
        dmaChannel = new DMAChannel(); // reserve a DMA channel
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWriteFast(LED_BUILTIN, LOW);
}

void BufferDMA::start()
{

    dmaChannel->source(*ADC_RA);

    dmaChannel->destinationBuffer((uint16_t *)p_elems, sizeof(uint16_t) * b_size);

    dmaChannel->transferSize(sizeof(uint16_t)); // both SRC and DST size

    // dmaChannel->transferCount(b_size); // transfer b_size values

    dmaChannel->interruptAtHalf(); //interruptAtHalf or interruptAtCompletion
    dmaChannel->interruptAtCompletion();

    uint8_t DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC0;

#if ADC_NUM_ADCS >= 2
    if (ADC_number == 1)
    {
        DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC1;
    }
#endif // ADC_NUM_ADCS

    dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_ADC); // start DMA channel when ADC finishes a conversion
    dmaChannel->attachInterrupt(dma_half_complete_isr);

    uint32_t temp = *ADC_RA; // clear ADC completion bit    

    Serial.print("DADDR=");
    Serial.println((uint32_t)dmaChannel->TCD->DADDR, HEX);
    Serial.print("CITER=");
    Serial.println((uint32_t)dmaChannel->TCD->CITER);

    dmaChannel->enable();

    Serial.print("DADDR=");
    Serial.println((uint32_t)dmaChannel->TCD->DADDR, HEX);
    Serial.print("CITER=");
    Serial.println((uint32_t)dmaChannel->TCD->CITER);

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}

BufferDMA::~BufferDMA()
{

    dmaChannel->detachInterrupt();
    dmaChannel->disable();
    delete dmaChannel;
    dmaChannel = nullptr;
}

void dma_half_complete_isr()
{
    BufferDMA::dmaChannel->clearInterrupt();
    digitalWriteFast(LED_BUILTIN, HIGH);
    // if (half_complete != -1)
    // {
    //     // ERROR
    //     Serial.println("Buffer overrun");
    //     noInterrupts();
    //     for (;;)
    //         ;
    //     return;
    // }
    BufferDMA::data_ready++;
    digitalWriteFast(LED_BUILTIN, LOW);
}
