#ifndef SSD1306_H_
#define SSD1306_H_

#include <stdbool.h>
#include <stdint.h>

void ssd1306_init(void);
void ssd1306_r(bool selected);
void ssd1306_g(bool selected);
void ssd1306_b(bool selected);
void ssd1306_w(bool selected);
void ssd1306_num(uint8_t val);

#endif
