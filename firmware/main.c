#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"
#include "encoder.h"

int main(void)
{
    led_init();
    encoder_init();
    sei();

    uint8_t x = 2;
    uint8_t state = 0;
    while (1)
    {
        x += encoder_read();
        if (encoder_pushed())
            state = (state + 1) % 3;
        switch (state)
        {
        case 0:
            led_rgb(x, 0, 0);
            break;
        case 1:
            led_rgb(0, x, 0);
            break;
        case 2:
            led_rgb(0, 0, x);
            break;
        }
        _delay_ms(1);
    }
}
