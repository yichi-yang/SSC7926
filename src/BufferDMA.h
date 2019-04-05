#ifndef SSC7926_BUFFERDMA_H
#define SSC7926_BUFFERDMA_H

#include <Arduino.h>
#include "DMAChannel.h"

#define READY 1
#define OVERRUN -1

void dma_half_complete_isr();

/** Class BufferDMA implements a DMA ping-pong buffer of fixed size
 * 
 *  While one half of the buffer is being filled up by DMA transfers, the
 *  other half can be safely accessed and processed.
 * 
 *  Unfortunately since member functions cannot be used as ISRs,
 *  it is not possible to use multiple instances of BufferDMA
*/
class BufferDMA
{
  public:
    //! Constructor, buffer has a size len and stores the conversions of ADC number ADC_num
    BufferDMA(volatile int16_t *buffer, uint32_t len, uint8_t ADC_num = 0);

    //! Destructor
    ~BufferDMA();

    //! Start DMA operation
    static void start();

    //! Stop DMA operation
    static void stop();

    //! return 1 when half-buffer is ready to be read, 0 otherwise. return -1 when buffer is overrun
    static int8_t ready();

    //! return address of the half buffer to be read and mark it as read.
    static volatile int16_t *read_half();

    //! return number of remaining elements to be read.
    static volatile size_t remain();

  private:
    static DMAChannel *dmaChannel;
    static volatile int16_t *p_elems;
    static uint16_t b_size;
    static uint8_t adc_number;
    static volatile uint32_t *adc_RA;
    static volatile bool second_half;
    static volatile int8_t data_ready;

    //! Half complete ISR is called everytime the buffer is half full
    friend void dma_half_complete_isr();
};

#endif
