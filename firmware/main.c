#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"
#include "encoder.h"

int main(void)
{
    led_init();
    encoder_init();
    sei();

    uint8_t x = 0;
    while (1)
    {
        x += encoder_read();
        led_rgb(x, x, x);
        _delay_ms(1);
    }
}
