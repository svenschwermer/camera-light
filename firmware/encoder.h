#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdbool.h>
#include <stdint.h>

// Should be a power of 2
#define STEPS_PER_INDENT 2

void encoder_init(void);
int8_t encoder_read(void);
bool encoder_pushed(void);

#endif
