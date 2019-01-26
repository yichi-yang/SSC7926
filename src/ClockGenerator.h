#ifndef SSC7926_CLOCK_GENERATOR
#define SSC7926_CLOCK_GENERATOR

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ADC.h"

/* // Arduino Mega
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define FREQUENCYTIMER2_PIN  10

// Teensy++
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define FREQUENCYTIMER2_PIN  24

// Teensy 3.0 & 3.1
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
#define FREQUENCYTIMER2_PIN  5

// Sanguino
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define FREQUENCYTIMER2_PIN  15

// Arduino Uno, Duemilanove, Diecimila, etc
#else
#define FREQUENCYTIMER2_PIN  11
#endif */

#define CLK_PIN 5 // teensy 3.6

// CMT_MSC bits
#define EOCF 7
#define CMTDIV 5 // 2 bits
#define EXSPC 4
#define BASE 3
#define FSK 2
#define EOCIE 1
#define MCGEN 0

// CMT_MSC bits
#define IROL 7
#define CMTPOL 6
#define IROPEN 5

// Pin ROG and STEP
#define PIN_ROG 27
#define PIN_STEP 25
#define PIN_SIG A9

class ClockGenerator
{
  // public:
  //   static void (*onOverflow)(); // not really public, but I can't work out the 'friend' for the SIGNAL

public:
  ClockGenerator(ADC* p);
  static void setPeriod(unsigned long);
  static void config(uint32_t _line_number, uint32_t _cycle_per_line, uint32_t _cycle_per_step);
  // static unsigned long getPeriod();
  // static void setOnOverflow(void (*)());
  static void enable();
  static void disable();
  static uint8_t is_active();
  volatile static uint32_t current_line, current_cycle;
  static uint8_t step_pin_status;
  static uint32_t max_line, cycle_per_line, cycle_per_step;
  static ADC *adc_ptr;
};

#endif
