#include <ClockGenerator.h>
#include "pinDef.h"
#include <avr/interrupt.h>

// void (*ClockGenerator::onOverflow)() = 0;
// uint8_t ClockGenerator::enabled = 0;

volatile int32_t ClockGenerator::current_line = -10;
volatile int32_t ClockGenerator::current_cycle;
volatile uint8_t ClockGenerator::step_pin_status = LOW;
int32_t ClockGenerator::max_line = 0;
int32_t ClockGenerator::cycle_per_line = 0;
int32_t ClockGenerator::cycle_per_step = 0;
ADC *ClockGenerator::adc_ptr = nullptr;

ClockGenerator::ClockGenerator(ADC *p)
{
    adc_ptr = p;
    CORE_PIN5_CONFIG = PORT_PCR_MUX(2) | PORT_PCR_DSE | PORT_PCR_SRE;
    step_pin_status = LOW;
    NVIC_SET_PRIORITY(IRQ_CMT, 0);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWriteFast(LED_BUILTIN, LOW);
}

void ClockGenerator::setPeriod(unsigned long period)
{
    uint8_t bdiv, cdiv = 0;

    if (period == 0)
        period = 1;
    period = (int)((double)period * F_BUS / 1000000000L);
    Serial.println(period);
    if (period < 65535 * 16)
    {
        bdiv = 0;
    }
    else if (period < 65535 * 2 * 16)
    {
        bdiv = 1;
    }
    else if (period < 65535 * 3 * 16)
    {
        bdiv = 2;
    }
    else if (period < 65535 * 4 * 16)
    {
        bdiv = 3;
    }
    else if (period < 65535 * 5 * 16)
    {
        bdiv = 4;
    }
    else if (period < 65535 * 6 * 16)
    {
        bdiv = 5;
    }
    else if (period < 65535 * 7 * 16)
    {
        bdiv = 6;
    }
    else if (period < 65535 * 8 * 16)
    {
        bdiv = 7;
    }
    else if (period < 65535 * 9 * 16)
    {
        bdiv = 8;
    }
    else if (period < 65535 * 10 * 16)
    {
        bdiv = 9;
    }
    else if (period < 65535 * 11 * 16)
    {
        bdiv = 10;
    }
    else if (period < 65535 * 12 * 16)
    {
        bdiv = 11;
    }
    else if (period < 65535 * 13 * 16)
    {
        bdiv = 12;
    }
    else if (period < 65535 * 14 * 16)
    {
        bdiv = 13;
    }
    else if (period < 65535 * 15 * 16)
    {
        bdiv = 14;
    }
    else if (period < 65535 * 16 * 16)
    {
        bdiv = 15;
    }
    else if (period < 65535 * 18 * 16)
    {
        bdiv = 8;
        cdiv = 1;
    }
    else if (period < 65535 * 20 * 16)
    {
        bdiv = 9;
        cdiv = 1;
    }
    else if (period < 65535 * 22 * 16)
    {
        bdiv = 10;
        cdiv = 1;
    }
    else if (period < 65535 * 24 * 16)
    {
        bdiv = 11;
        cdiv = 1;
    }
    else if (period < 65535 * 26 * 16)
    {
        bdiv = 12;
        cdiv = 1;
    }
    else if (period < 65535 * 28 * 16)
    {
        bdiv = 13;
        cdiv = 1;
    }
    else if (period < 65535 * 30 * 16)
    {
        bdiv = 14;
        cdiv = 1;
    }
    else if (period < 65535 * 32 * 16)
    {
        bdiv = 15;
        cdiv = 1;
    }
    else if (period < 65535 * 36 * 16)
    {
        bdiv = 8;
        cdiv = 2;
    }
    else if (period < 65535 * 40 * 16)
    {
        bdiv = 9;
        cdiv = 2;
    }
    else if (period < 65535 * 44 * 16)
    {
        bdiv = 10;
        cdiv = 2;
    }
    else if (period < 65535 * 48 * 16)
    {
        bdiv = 11;
        cdiv = 2;
    }
    else if (period < 65535 * 52 * 16)
    {
        bdiv = 12;
        cdiv = 2;
    }
    else if (period < 65535 * 56 * 16)
    {
        bdiv = 13;
        cdiv = 2;
    }
    else if (period < 65535 * 60 * 16)
    {
        bdiv = 14;
        cdiv = 2;
    }
    else if (period < 65535 * 64 * 16)
    {
        bdiv = 15;
        cdiv = 2;
    }
    else if (period < 65535 * 72 * 16)
    {
        bdiv = 8;
        cdiv = 3;
    }
    else if (period < 65535 * 80 * 16)
    {
        bdiv = 9;
        cdiv = 3;
    }
    else if (period < 65535 * 88 * 16)
    {
        bdiv = 10;
        cdiv = 3;
    }
    else if (period < 65535 * 96 * 16)
    {
        bdiv = 11;
        cdiv = 3;
    }
    else if (period < 65535 * 104 * 16)
    {
        bdiv = 12;
        cdiv = 3;
    }
    else if (period < 65535 * 112 * 16)
    {
        bdiv = 13;
        cdiv = 3;
    }
    else if (period < 65535 * 120 * 16)
    {
        bdiv = 14;
        cdiv = 3;
    }
    else
    {
        bdiv = 15;
        cdiv = 3;
    }
    period /= (bdiv + 1);
    period >>= (cdiv + 4);
    if (period > 65535)
        period = 65535;
    // high time = (CMD1:CMD2 + 1) ÷ (fCMTCLK ÷ 8)
    // low time  = CMD3:CMD4 ÷ (fCMTCLK ÷ 8)
    SIM_SCGC4 |= SIM_SCGC4_CMT;
    CMT_MSC = 0;
    CMT_PPS = bdiv;
    CMT_CMD1 = ((period - 1) >> 8) & 255;
    CMT_CMD2 = (period - 1) & 255;
    CMT_CMD3 = (period >> 8) & 255;
    CMT_CMD4 = period & 255;
    CMT_OC = 0x20;                // CMTPOL bit set to 0
    CMT_MSC = (cdiv << 5) | 0x0A; // baseband mode, do not set MCGEN
}

// unsigned long ClockGenerator::getPeriod()
// {
//     uint32_t period;

//     period = (CMT_CMD3 << 8) | CMT_CMD4;
//     period *= (CMT_PPS + 1);
//     period <<= ((CMT_MSC >> 5) & 3) + 4;
//     period /= (F_BUS / 1000000);
//     return period;
// }

void ClockGenerator::enable()
{
    // ClockGenerator::enabled = 1;
    current_cycle = 0;
    current_line = -10;
    step_pin_status = LOW;
    pinMode(PIN_ROG, OUTPUT);
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_SIG_OUT, INPUT);
    NVIC_ENABLE_IRQ(IRQ_CMT);
    bitSet(CMT_MSC, MCGEN);
}

void ClockGenerator::disable()
{
    // ClockGenerator::enabled = 0;
    // CORE_PIN5_CONFIG = PORT_PCR_MUX(1) | PORT_PCR_DSE | PORT_PCR_SRE;
    // digitalWriteFast(5, LOW);
    bitClear(CMT_MSC, MCGEN);
    NVIC_DISABLE_IRQ(IRQ_CMT);
}

uint8_t ClockGenerator::is_active()
{
    return bitRead(CMT_MSC, MCGEN);
}

void cmt_isr(void)
{
    // static uint8_t inHandler = 0;

    uint8_t tmp = CMT_MSC;
    tmp = CMT_CMD2;
    // if (!inHandler && ClockGenerator::onOverflow)
    // {
    //     inHandler = 1;
    //     (*ClockGenerator::onOverflow)();
    //     inHandler = 0;
    // }
    if (ClockGenerator::current_cycle == 0)
    {
        bitSet(CMT_MSC, EXSPC);
    }
    else if (ClockGenerator::current_cycle == 2)
    {
        bitClear(CMT_MSC, EXSPC);
        digitalWriteFast(PIN_ROG, LOW);
    }
    else if (ClockGenerator::current_cycle == 1)
    {
        digitalWriteFast(PIN_ROG, HIGH);
    }
    
    if (ClockGenerator::current_line >= 0 && ClockGenerator::current_cycle >= 102 && ClockGenerator::current_cycle <= 8027)
    {
        if (ClockGenerator::adc_ptr->adc0->isConverting())
        {
            digitalWriteFast(LED_BUILTIN, HIGH);
            for (;;)
                ;
        }
        ClockGenerator::adc_ptr->adc0->startSingleRead(PIN_SIG_OUT);
    }

    if (ClockGenerator::current_line >= 0 && (ClockGenerator::current_cycle + ClockGenerator::current_line * ClockGenerator::cycle_per_line) % ClockGenerator::cycle_per_step == 0)
    {
        digitalWriteFast(PIN_STEP, HIGH);
        ClockGenerator::step_pin_status = HIGH;
    }
    else if (ClockGenerator::step_pin_status)
    {
        digitalWriteFast(PIN_STEP, LOW);
        ClockGenerator::step_pin_status = HIGH;
    }

    if (ClockGenerator::current_cycle == ClockGenerator::cycle_per_line - 1)
    {

        ClockGenerator::current_line++;
        ClockGenerator::current_cycle = 0;
        if (ClockGenerator::current_line == ClockGenerator::max_line)
        {
            bitClear(CMT_MSC, MCGEN);
            NVIC_DISABLE_IRQ(IRQ_CMT);
        }
    }
    else
    {
        ClockGenerator::current_cycle++;
    }
    // Serial.printf("CMT ISR\n");
}

void ClockGenerator::config(uint32_t _line_number, uint32_t _cycle_per_line, uint32_t _cycle_per_step)
{
    max_line = _line_number;
    cycle_per_line = _cycle_per_line;
    cycle_per_step = _cycle_per_step;
}
