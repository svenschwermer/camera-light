#ifndef TWI_H_
#define TWI_H_

#include <stdint.h>

#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)

void twi_init(uint8_t baud_rate);
void twi_wait(void);
void twi_write(uint8_t slave_address, const uint8_t *data, uint8_t len);

#endif
