#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "led.h"
#include "encoder.h"

enum state
{
    set_red,
    set_green,
    set_blue,
    set_white,
};

static struct
{
    uint8_t r, g, b, w;
} eeprom EEMEM = {1, 1, 1, 1};

static enum state state;
static uint8_t r, g, b, w;

static inline void state_func(uint8_t *v, uint8_t *eemem, enum state next)
{
    *v += encoder_read();
    if (encoder_pushed())
    {
        eeprom_update_byte(eemem, *v);
        state = next;
    }
}

int main(void)
{
    state = set_red;
    r = eeprom_read_byte(&eeprom.r);
    g = eeprom_read_byte(&eeprom.g);
    b = eeprom_read_byte(&eeprom.b);
    w = eeprom_read_byte(&eeprom.w);

    led_init();
    encoder_init();
    sei();

    while (1)
    {
        switch (state)
        {
        case set_red:
            state_func(&r, &eeprom.r, set_green);
            break;
        case set_green:
            state_func(&g, &eeprom.g, set_blue);
            break;
        case set_blue:
            state_func(&b, &eeprom.b, set_white);
            break;
        case set_white:
            state_func(&w, &eeprom.w, set_red);
            break;
        }
        led_rgbw(r, g, b, w);
        _delay_ms(5);
    }
}
