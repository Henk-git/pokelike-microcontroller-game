# main.c

```c
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "matrix.h"
#include "game.h"

#define BAUD 9600
#define MYUBRR F_CPU / 8 / BAUD - 1

static void adc_init(void) {
	ADMUX  = (1 << REFS0) | (1 << MUX1);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static uint16_t adc_read(void) {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int main(void) {
	uart_init(MYUBRR);
	matrix_init();
	adc_init();
	game_init();

	// Timer0 fr Zufall starten
	TCCR0B |= (1 << CS00);

	uint16_t debounce_counter = 0;
	uint8_t action_executed = 0;

	while (1) {
		uint16_t x_val = adc_read();

		if (x_val > 800 || x_val < 200) {
			if (!action_executed && game_is_over() == ONGOING) {
				game_player_turn();
				if (game_is_over() == ONGOING) {
					game_enemy_turn();
				}
				action_executed = 1;
				debounce_counter = 0;
			}
		}

		if (action_executed) {
			debounce_counter++;
			if (debounce_counter > 150 && x_val > 400 && x_val < 600) {
				action_executed = 0;
			}
		}

		// Matrix-Frame bauen: Zeile 0 = Gegner-HP, Zeile 7 = Spieler-HP
		uint8_t frame[8] = {0};
		frame[0] = hp_to_bitmask(game_enemy_hp());
		frame[7] = hp_to_bitmask(game_player_hp());

		matrix_render(frame);
	}
}
```

# game.h

```c
#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define PLAYER_MAX_HP 120
#define ENEMY_MAX_HP  120

typedef enum { ONGOING, WIN, LOSE } GameResult;

void        game_init(void);
void        game_player_turn(void);
void        game_enemy_turn(void);
GameResult  game_is_over(void);
int8_t      game_player_hp(void);
int8_t      game_enemy_hp(void);

#endif
```

# game.c

```c
#include "game.h"
#include "uart.h"
#include <avr/io.h>

static int8_t player_hp;
static int8_t enemy_hp;

void game_init(void) {
	player_hp = PLAYER_MAX_HP;
	enemy_hp  = ENEMY_MAX_HP;
	uart_print("\r\n=== POKEBATTLE START ===\r\n");
	uart_print("Your HP: 120 | Enemy HP: 120\r\n");
}

void game_player_turn(void) {
	// 10% Chance zu verfehlen (TCNT0 luft frei)
	if (TCNT0 % 10 < 1) {
		uart_print("> Dein Angriff verfehlt!\r\n");
		return;
	}
	enemy_hp -= 15;
	if (enemy_hp < 0) enemy_hp = 0;
	uart_print("> Treffer! 15 Schaden. Gegner HP: ");
	uart_print_int(enemy_hp);
	uart_print("\r\n");
}

void game_enemy_turn(void) {
	// 50% Chance zu verfehlen
	if ((TCNT0 & 0x01) == 0) {
		uart_print("> Gegner verfehlt!\r\n");
		return;
	}
	player_hp -= 15;
	if (player_hp < 0) player_hp = 0;
	uart_print("> Gegner trifft! 15 Schaden. Deine HP: ");
	uart_print_int(player_hp);
	uart_print("\r\n");
}

GameResult game_is_over(void) {
	if (enemy_hp  <= 0) return WIN;
	if (player_hp <= 0) return LOSE;
	return ONGOING;
}

int8_t game_player_hp(void) { return player_hp; }
int8_t game_enemy_hp(void)  { return enemy_hp;  }
```

# matrix.h

```c
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

void matrix_init(void);
void matrix_render(uint8_t frame[8]);
uint8_t hp_to_bitmask(int8_t hp);

#endif
```

# matrix.c

```c
#include "matrix.h"
#include <avr/io.h>
#include <util/delay.h>

void matrix_init(void) {
	// SPI fr 74HC595
	DDRB  |= (1 << PB3) | (1 << PB5) | (1 << PB2);
	SPCR   = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	// Zeilen-Pins (PD2-PD7, PC0-PC1)
	DDRD  |= 0xFC;
	DDRC  |= 0x03;
}

static void shift_out(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	PORTB &= ~(1 << PB2);
	PORTB |=  (1 << PB2);
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
		shift_out(frame[i]);
		activate_row(i);
		_delay_ms(1);
	}
}

uint8_t hp_to_bitmask(int8_t hp) {
	if (hp <= 0) return 0x00;
	if (hp >= 120) return 0xFF;
	uint8_t leds = (uint8_t)hp / 15;
	return (uint8_t)((1 << leds) - 1);
}
```

# uart.h

```c
#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(uint32_t baud);
void uart_putc(char c);
void uart_print(const char *s);
void uart_println(const char *s);
void uart_print_int(int16_t n);

#endif
```

# uart.c

```c
#include "uart.h"
#include <avr/io.h>

// USART initialisieren
void uart_init(uint32_t baud)
{
	UCSR0A = (1 << U2X0);
	uint16_t ubrr = F_CPU / 8 / baud - 1;
	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// ein Zeichen senden
void uart_putc(char c)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

// String senden
void uart_print(const char *s)
{
	while (*s)
		uart_putc(*s++);
}

// String mit Zeilenumbruch senden
void uart_println(const char *s)
{
	uart_print(s);
	uart_putc('\r');
	uart_putc('\n');
}

// Zahl als String senden (bis zu 3-stellig)
void uart_print_int(int16_t n)
{
	if (n < 0)
	{
		uart_putc('-');
		n = -n;
	}
	if (n >= 100)
		uart_putc('0' + n / 100);
	if (n >= 10)
		uart_putc('0' + (n / 10) % 10);
	uart_putc('0' + n % 10);
}
```

# input.h

```c
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef enum { DIR_LEFT, DIR_CENTER, DIR_RIGHT } Direction;

void input_init(void);
Direction joystick_direction(void);
uint8_t button_pressed(void);

#endif
```

# input.c

```c
#include "input.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Flag wird in ISR gesetzt
static volatile uint8_t btn_flag = 0;

// ADC und INT0 initialisieren
void input_init(void) {
	// ADC: AVcc Referenz, Kanal 0 (PC0)
	ADMUX  = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// INT0: fallende Flanke (PD2), Pull-up aktivieren
	DDRD  &= ~(1 << PD2);
	PORTD |=  (1 << PD2);
	EICRA  = (1 << ISC01);
	EIMSK  = (1 << INT0);
	sei();
}

// ADC-Wert lesen
static uint16_t adc_read(void) {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

// Joystick-Richtung aus ADC-Wert ableiten
Direction joystick_direction(void) {
	uint16_t val = adc_read();
	if (val < 300)  return DIR_LEFT;
	if (val > 700)  return DIR_RIGHT;
	return DIR_CENTER;
}

// Tastendruck abfragen und Flag lschen
uint8_t button_pressed(void) {
	if (btn_flag) { btn_flag = 0; return 1; }
	return 0;
}

// Joystick-Knopf ISR
ISR(INT0_vect) {
	btn_flag = 1;
}
```

# Makefile

```makefile
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
	$(AVRDUDE) -p m328p -c arduino -P /dev/cu.usbmodem11101 -b 115200 -U flash:w:main.hex

clean:
	rm -f $(OBJ) main.elf main.hex
```