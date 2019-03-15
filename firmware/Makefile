target ?= camera-light.elf
src = main.c led.c encoder.c twi.c ssd1306.c
obj = $(src:%c=%o)

CC = avr-gcc
CFLAGS = -mmcu=attiny817 -gdwarf -Os -DF_CPU=16000000ul -std=gnu11 -Wall -Werror
LDFLAGS = -mmcu=attiny817 -gdwarf -Os

$(target): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^
	avr-objdump -P mem-usage $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(obj) $(target)

flash: $(target)
	avrdude -p t817 -c xplainedmini_updi -e -U flash:w:$< -U eeprom:w:$<

dis: $(target)
	avr-objdump -S $< | vim -R -

.PHONY: clean flash dis
