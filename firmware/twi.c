#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "twi.h"

static volatile enum { idle,
                       busy } state;
static const uint8_t *write_data;
static uint8_t bytes_to_write;

void twi_init(uint8_t baud_rate)
{
    // PA2 (SCL); PA1 (SDA)
    PORTA.OUTSET = PIN2_bm | PIN1_bm;
    PORTA.DIRSET = PIN2_bm | PIN1_bm;

    PORTMUX.CTRLB = PORTMUX_TWI0_ALTERNATE_gc;

    TWI0.MCTRLA = TWI_WIEN_bm | TWI_ENABLE_bm;
    TWI0.MBAUD = baud_rate;
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;

    state = idle;
}

void twi_wait(void)
{
    while (state != idle)
        ;
}

void twi_write(uint8_t slave_address, const uint8_t *data, uint8_t len)
{
    twi_wait();
    state = busy;

    write_data = data;
    bytes_to_write = len;
    TWI0.MADDR = slave_address & ~0x01; // R/!W = 0
}

ISR(TWI0_TWIM_vect)
{
    uint8_t status = TWI0.MSTATUS;

    if (status & (TWI_ARBLOST_bm | TWI_BUSERR_bm))
    {
        // Master Arbitration Lost Bus Error
        state = idle;
    }
    else if (status & TWI_WIF_bm)
    {
        if (status & TWI_RXACK_bm)
        {
            // If NOT acknowledged (NACK) by slave, cancel the transaction
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            state = idle;
        }
        else if (bytes_to_write > 0)
        {
            uintptr_t addr = (uintptr_t)write_data;
            if (addr >= INTERNAL_SRAM_START && addr <= INTERNAL_SRAM_END)
                TWI0.MDATA = *write_data;
            else
                TWI0.MDATA = pgm_read_byte(write_data);
            ++write_data;
            --bytes_to_write;
        }
        else
        {
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            state = idle;
        }
    }
}
