#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"

int main(void)
{
    led_init();
    sei();

    while (1)
    {
        led_rgb(0, 1, 0);
        _delay_ms(500);
    }
}
