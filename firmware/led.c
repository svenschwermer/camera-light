/*
 * Copyright 2017 Thomas F. (thomas_fr)
 * https://www.mikrocontroller.net/topic/437709#5183860
 *
 * This is a driver for driving WS2812B LEDs. It makes use of the USART0, TCB0
 * and CCL peripheral. It furthermore makes use of the asynchronous event
 * channel 0 and the event user 0. During init, the main clock prescaler is
 * switched off and it's required to set the system clock to 16 MHz.
 *
 * Pinout:
 *   PB4: WS2812B output
 *   PB0: internal use (XDIR)
 *   PB1: internal use (XCK)
 *   PB2: internal use (TxD)
 *   PB3: internal use (RxD)
 *
 *        ┌───┐   ┌───┐
 *   XCK  │   │   │   │
 *        ┘   └───┘   └────
 *                ┌───────┐
 *   TxD          │       │
 *        ────────┘       └
 *          ┌──┐    ┌──┐
 *   TCB    │  │    │  │
 *        ──┘  └────┘  └───
 *        ┌─┐     ┌────┐
 *   WS   │ │     │    │
 *        ┘ └─────┘    └───
 *   ns   312/938 875/375
 *
 *   IN2  IN1  IN0 │ OUT
 *   TCB  TxD  XCK │ WS
 *   ──────────────┼────
 *    0    0    0  │  0
 *    0    0    1  │  1
 *    0    1    0  │  0
 *    0    1    1  │  1
 *    1    0    0  │  0
 *    1    0    1  │  0
 *    1    1    0  │  1
 *    1    1    1  │  1
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/xmega.h>
#include "led.h"

static volatile struct
{
    uint8_t led_index : 6;
    uint8_t sub_index : 2;
    uint8_t r, g, b;
#ifdef WHITE_CHANNEL
    uint8_t w;
#endif
} led_state;

void led_init(void)
{
    VPORTB.DIR |= PIN4_bm; // WS2812B output

    // Full speed, prescaler off (16 MHz)
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, !CLKCTRL_PEN_bm);

    // USART0
    VPORTB.DIR |= PIN1_bm;                               // XCK
    USART0.CTRLC = USART_CMODE_MSPI_gc | USART_UCPHA_bm; // Master SPI mode, CPHA=1 (SPI Mode 1)
    USART0.BAUD = (F_CPU / 2 / 800000) << 6;             // 800 kHz, 1.25 µs
    USART0.CTRLB = USART_TXEN_bm;

    // TCB0
    TCB0.CTRLB = TCB_CNTMODE_SINGLE_gc;
    TCB0.EVCTRL = TCB_CAPTEI_bm;
    TCB0.CCMP = 8; // 875ns high at bit == 1  (312.5ns + 9*62.5ns)
    TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm;

    // EVENT XCK->TCB0 single shot (timer start 312.5ns(5) after XCK pos. edge)
    EVSYS.ASYNCCH1 = EVSYS_ASYNCCH1_PORTB_PIN1_gc;   // USART XCK (producer)
    EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_ASYNCCH1_gc; // TCB0 single shot (consumer)

    // CCL
    CCL.LUT0CTRLB = CCL_INSEL0_USART0_gc | CCL_INSEL1_USART0_gc;
    CCL.LUT0CTRLC = CCL_INSEL2_TCB0_gc;
    CCL.TRUTH0 = 0xca;                   // 1100_1010
    CCL.LUT0CTRLA = CCL_OUTEN_bm | 0x01; // LUT_ENABLE
    CCL.CTRLA = CCL_ENABLE_bm;

    PORTMUX.CTRLA |= PORTMUX_LUT0_ALTERNATE_gc; // LUT0-OUT => PB4
}

void led_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    led_state.led_index = 0;
    led_state.sub_index = 1;
    led_state.r = r;
    led_state.g = g;
    led_state.b = b;
    USART0.CTRLA = USART_DREIE_bm; // Enable Data Register Empty interrupt
    USART0.TXDATAL = g;
}

ISR(USART0_DRE_vect)
{
    switch (led_state.sub_index)
    {
    case 0:
        USART0.TXDATAL = led_state.g;
        led_state.sub_index = 1;
        break;

    case 1:
        USART0.TXDATAL = led_state.r;
        led_state.sub_index = 2;
        break;

    case 2:
        USART0.TXDATAL = led_state.b;
#ifdef WHITE_CHANNEL
        led_state.sub_index = 3;
        break;

    case 3:
        USART0.TXDATAL = led_state.w;
#endif
        led_state.sub_index = 0;
        ++led_state.led_index;
        break;
    }

    if (led_state.led_index == LED_COUNT)
        USART0.CTRLA = 0; // Disable Data Register Empty interrupt
}
