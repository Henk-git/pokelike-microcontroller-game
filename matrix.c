#include "matrix.h"
#include <avr/io.h>
#include <util/delay.h>

void matrix_init(void) {
	// SPI fr 74HC595: SER=PB3/MOSI (11), SRCLK=PB5/SCK (13), RCLK/Latch=PB0 (8)
	// PB2 (SS) muss Output sein, sonst faellt der ATmega in SPI-Slave-Mode
	DDRB  |= (1 << PB3) | (1 << PB5) | (1 << PB0) | (1 << PB2);
	SPCR   = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	// Zeilen-Pins (PD2-PD7, PC0-PC1)
	DDRD  |= 0xFC;
	DDRC  |= 0x03;
}

static void shift_out(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	PORTB &= ~(1 << PB0);
	PORTB |=  (1 << PB0);
}

static void activate_row(uint8_t row) {
	PORTD |= 0xFC;
	PORTC |= 0x03;
	switch (row) {
		case 0: PORTD &= ~(1 << PD2); break;
		case 1: PORTD &= ~(1 << PD3); break;
		case 2: PORTD &= ~(1 << PD4); break;
		case 3: PORTD &= ~(1 << PD5); break;
		case 4: PORTD &= ~(1 << PD6); break;
		case 5: PORTD &= ~(1 << PD7); break;
		case 6: PORTC &= ~(1 << PC0); break;
		case 7: PORTC &= ~(1 << PC1); break;
	}
}

void matrix_render(uint8_t frame[8]) {
	for (uint8_t i = 0; i < 8; i++) {
		// Blanking: alle Zeilen aus, bevor neue Spaltendaten kommen
		PORTD |= 0xFC;
		PORTC |= 0x03;
		shift_out(frame[i]);
		activate_row(i);
		_delay_ms(2);
	}
}

uint8_t hp_to_bitmask(int8_t hp) {
	if (hp <= 0) return 0x00;
	if (hp >= 120) return 0xFF;
	uint8_t leds = ((uint8_t)hp + 14) / 15;
	return (uint8_t)((1 << leds) - 1);
}