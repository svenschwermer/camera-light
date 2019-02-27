target ?= camera-light.elf
src = main.c led.c encoder.c
obj = $(src:%c=%o)

CC = avr-gcc
CFLAGS = -mmcu=attiny817 -g -Os -DF_CPU=16000000ul -std=gnu11 -Wall -Werror
LDFLAGS = -mmcu=attiny817 -g -Os

$(target): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(obj) $(target)

flash: $(target)
	avrdude -p t817 -c xplainedmini_updi -e -U flash:w:$<

.PHONY: clean flash
