target ?= camera-light.elf
src = main.c led.c encoder.c twi.c ssd1306.c
obj = $(src:%c=%o)

CC = avr-gcc
CFLAGS = -mmcu=attiny1614 -gdwarf -Os -DF_CPU=16000000ul -std=gnu11 -Wall -Werror
LDFLAGS = -mmcu=attiny1614 -gdwarf -Os

$(target): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^
	avr-objdump -P mem-usage $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(obj) $(target)

flash: $(target)
	avrdude -p t1614 -c jtag3updi -e -U flash:w:$< -U eeprom:w:$<

fuse:
	avrdude -p t1614 -c jtag3updi -U fuse2:w:0x1:m

dis: $(target)
	avr-objdump -S $< | less

dwarf: $(target)
	avr-objdump --dwarf=info $< | less

.PHONY: clean flash fuse dis dwarf
