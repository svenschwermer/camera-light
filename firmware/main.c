#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "encoder.h"
#include "ssd1306.h"
#include "led.h"

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
    int8_t delta = encoder_read();
    if (delta != 0)
    {
        *v += delta;
        ssd1306_num(*v);
    }
    if (encoder_pushed())
    {
        eeprom_update_byte(eemem, *v);
        state = next;
    }
}

static inline void display_state(enum state current, void (*disp_current)(bool),
                                 void (*disp_next)(bool), uint8_t v_next)
{
    if (state != current)
    {
        disp_current(false);
        disp_next(true);
        ssd1306_num(v_next);
    }
}

int main(void)
{
    state = set_white;
    r = eeprom_read_byte(&eeprom.r);
    g = eeprom_read_byte(&eeprom.g);
    b = eeprom_read_byte(&eeprom.b);
    w = eeprom_read_byte(&eeprom.w);

    led_init();
    encoder_init();
    sei();
    ssd1306_init();

    ssd1306_w(true);
    ssd1306_num(w);

    while (1)
    {
        switch (state)
        {
        case set_red:
            state_func(&r, &eeprom.r, set_green);
            display_state(set_red, ssd1306_r, ssd1306_g, g);
            break;
        case set_green:
            state_func(&g, &eeprom.g, set_blue);
            display_state(set_green, ssd1306_g, ssd1306_b, b);
            break;
        case set_blue:
            state_func(&b, &eeprom.b, set_white);
            display_state(set_blue, ssd1306_b, ssd1306_w, w);
            break;
        case set_white:
            state_func(&w, &eeprom.w, set_red);
            display_state(set_white, ssd1306_w, ssd1306_r, r);
            break;
        }
        led_rgbw(r, g, b, w);
        _delay_ms(50);
    }
}
