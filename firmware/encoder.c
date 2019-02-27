#include <avr/io.h>
#include <avr/interrupt.h>
#include "encoder.h"

static volatile int8_t enc_delta;
static int8_t last;

static inline uint8_t phase_a(void)
{
    return VPORTC.IN & PIN2_bm;
}

static inline uint8_t phase_b(void)
{
    return VPORTC.IN & PIN3_bm;
}

void encoder_init(void)
{
    enc_delta = 0;
    last = 0;

    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    if (phase_a())
        last = 3;
    if (phase_b())
        last ^= 1; // convert gray to binary

    TCA0.SINGLE.PER = F_CPU / 16 / 1000 - 1; // 1 kHz
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;
}

int8_t encoder_read(void)
{
    int8_t val;
    cli();
    val = enc_delta;
    enc_delta = val & ((1 << STEPS_PER_INDENT) - 1);
    sei();
    return val >> STEPS_PER_INDENT;
}

ISR(TCA0_OVF_vect)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;

    int8_t diff;
    int8_t new = 0;
    if (phase_a())
        new = 3;
    if (phase_b())
        new ^= 1;      // convert gray to binary
    diff = last - new; // difference last - new
    if (diff & 1)
    {                                // bit 0 = value (1)
        last = new;                  // store new as next last
        enc_delta += (diff & 2) - 1; // bit 1 = direction (+/-)
    }
}
