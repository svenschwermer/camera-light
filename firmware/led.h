#ifndef LED_H_
#define LED_H_

#include <stdint.h>

#define LED_COUNT 24
#define WHITE_CHANNEL

void led_init(void);

#ifdef WHITE_CHANNEL
void led_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
#else
void led_rgb(uint8_t r, uint8_t g, uint8_t b);
#endif

#endif
