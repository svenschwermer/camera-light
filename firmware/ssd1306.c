#include <avr/pgmspace.h>
#include "twi.h"
#include "ssd1306.h"

// static const uint8_t WIDTH = 128;
static const uint8_t HEIGHT = 32;
static const uint8_t address = 0x3c << 1;

// Init sequence
static const uint8_t init[] PROGMEM = {
    0x00, // Control Byte; Co = 0, D/C = 0
    0xae, // Set Display OFF
    0xd5, // Set Display Clock Divide Ratio / Oscillator Frequency
    0x80, // RESET value
    0xa8, // Set Multiplex Ratio
    HEIGHT - 1,
    0xd3,        // Set Display Offset
    0x0,         // no offset
    0x40 | 0x00, // Set Display Start Line = 0
    0x8d,        // Charge Pump Setting
    0x14,        // Enable charge pump during display on
    0x20,        // Set Memory Addressing Mode
    0x00,        // Horizontal Addressing Mode
    0xa1,        // Set Segment Re-map: column address 127 is mapped to SEG0
    0xc8,        // Set COM Output Scan Direction: remapped mode; scan from COM[N-1] to COM0
    0xda,        // Set COM Pins Hardware Configuration
    0x02,        // Sequential COM pin configuration, Disable COM Left/Right remap
    0x81,        // Set Contrast Control
    0x8F,
    0xd9, // Set Pre-charge Period
    0xf1, // Phase 2 period: 15 DCLK; Phase 1 period: 1 DCLK
    0xdb, // Set V_COMH Deselect Level
    0x40, // ?
    0xa4, // Entire Display ON: Resume to RAM content display
    0xa6, // Set Normal/Inverse Display: Normal display
    0x2e, // Deactivate scroll
    0xaf, // Set Display ON
};

void ssd1306_init(void)
{
    twi_init(TWI_BAUD(F_CPU, 100000ul));
    twi_write(address, init, sizeof(init));
}
