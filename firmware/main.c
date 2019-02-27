/*
 * main.c
 *
 * Created: 05.01.2017 16:11:55
 * Author : Thomas
 * https://www.mikrocontroller.net/topic/437709#5183860
 */
#include <avr/io.h>
#include <avr/xmega.h>
#include <util/delay.h>

static void rgb(uint8_t r, uint8_t g, uint8_t b)
{
	for (char i = 0; i < 3; ++i)
	{
		USART0.TXDATAL = g;
		while (!(USART0.STATUS & USART_DREIF_bm))
			;
		USART0.TXDATAL = r;
		while (!(USART0.STATUS & USART_DREIF_bm))
			;
		USART0.TXDATAL = b;
		while (!(USART0.STATUS & USART_DREIF_bm))
			;
	}
}

int main(void)
{
	VPORTB.DIR |= PIN4_bm; // LED

	// Full speed, prescaler off (16MHz)
	_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, !CLKCTRL_PEN_bm);

	// USART in master SPI mode
	VPORTB.DIR |= PIN1_bm; // XCK

	USART0.BAUD = (F_CPU / 2 / 800000) << 6; // 800kHz
	USART0.CTRLC = USART_UCPHA_bm | !USART_UDORD_bm | USART_CMODE_MSPI_gc;
	USART0.CTRLB = USART_TXEN_bm;

	// TCB0
	TCB0.CTRLB = TCB_CNTMODE_SINGLE_gc | !TCB_ASYNC_bm | TCB_CCMPEN_bm; // TCB_CCMPEN_bm Pin Output Enable not necessary for debugging (PORTA PIN5)
	TCB0.EVCTRL = !TCB_FILTER_bm | TCB_CAPTEI_bm;
	TCB0.CCMP = 8; // 875µs high at bit == 1  (312.5 + 9*62.5)
	TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm;

	// EVENT XCL->TCB0 single shot (timer start 312.5µs(5) after XCK pos. edge)
	EVSYS.ASYNCCH1 = EVSYS_ASYNCCH1_PORTB_PIN1_gc;   // USART XCK (producer)
	EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_ASYNCCH1_gc; // TCB0 single shot (consumer)

	// CCL

	//      +---+   +---+
	// XCK  |   |   |   |
	//      +   +---+   +----
	//              +-------+
	// TxD          |       |
	//      --------+       +
	//        +--+    +--+
	// TCB    |  |    |  |
	//      --+  +----+  +---
	//      +-+     +----+
	// WS   | |     |    |
	//      + +-----+    +---
	// µs   312/938 875/375

	// TCB  TxD  XCK    WS
	//  0    0    0     0
	//  0    0    1     1
	//  0    1    0     0
	//  0    1    1     1
	//  1    0    0     0
	//  1    0    1     0
	//  1    1    0     1
	//  1    1    1     1

	// (!TCB && XCK) || (TCB && TXD)

	// INSEL0 = USART0 XCK
	// INSEL1 = USART0 TxD
	// INSEL2 = TCB0 WO0
	CCL.LUT0CTRLB = CCL_INSEL0_USART0_gc | CCL_INSEL1_USART0_gc;
	CCL.LUT0CTRLC = CCL_INSEL2_TCB0_gc;
	CCL.TRUTH0 = 0b11001010;
	CCL.LUT0CTRLA = CCL_OUTEN_bm | 1; // | CCL_LUTEN_bm;
	CCL.CTRLA = CCL_ENABLE_bm;

	PORTMUX.CTRLA |= PORTMUX_LUT0_ALTERNATE_gc; // LUT0-OUT => PB4

	while (1)
	{
		rgb(255, 0, 0);
		_delay_ms(500);
		rgb(0, 255, 0);
		_delay_ms(500);
		rgb(0, 0, 255);
		_delay_ms(500);
	}
}
