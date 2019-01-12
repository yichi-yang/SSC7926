#ifndef SSC7926_BUFFERDMA_H
#define SSC7926_BUFFERDMA_H

#include <Arduino.h> // for digitalWrite
#include "DMAChannel.h"

void dma_half_complete_isr();

extern volatile int8_t half_complete;
extern DMAChannel *dmaChannel;
extern uint16_t b_size;

/** Class RingBufferDMA implements a DMA ping-pong buffer of fixed size
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

    //! Length of the buffer
    // static uint16_t size() { return b_size; }

    //! Pointer to the data
    // static volatile int16_t *const buffer() { return p_elems; }

    //! DMAChannel to handle all low level DMA code.
    // static DMAChannel *dmaChannel;

    // the buffer needs to be aligned, so use malloc instead of new
    // see http://stackoverflow.com/questions/227897/solve-the-memory-alignment-in-c-interview-question-that-stumped-me/
    //uint8_t alignment;
    //void *p_mem;

    //! Start pointer: Read here
    // uint16_t b_start;
    //! End pointer: Write here
    // uint16_t b_end;

    //! Pointer to the elements of the buffer
    // static volatile int16_t *p_elems;

    // static volatile int8_t half_complete;

    //! Size of buffer
    // static uint16_t b_size;

  protected:
  private:
    //! ADC module of the instance
    // static uint8_t ADC_number;

    // //! Increases the pointer modulo 2*size-1
    // uint16_t increase(uint16_t p);

    // static volatile uint32_t *ADC_RA;
};

#endif // RINGBUFFERDMA_H
