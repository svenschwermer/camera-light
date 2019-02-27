#ifndef LED_H_
#define LED_H_

#include <stdint.h>

#define LED_COUNT 24
/* #define WHITE_CHANNEL */

void led_init(void);
void led_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif
