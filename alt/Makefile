MCU     = atmega328p
F_CPU   = 16000000UL
CC      = avr-gcc
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c11
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

SRC = main.c uart.c matrix.c input.c game.c
OBJ = $(SRC:.c=.o)

all: main.hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

main.hex: main.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: main.hex
	$(AVRDUDE) -p m328p -c usbasp -U flash:w:main.hex

clean:
	rm -f $(OBJ) main.elf main.hex
