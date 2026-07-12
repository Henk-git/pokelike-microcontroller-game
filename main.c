#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "uart.h"
#include "matrix.h"
#include "game.h"

#define BAUD 9600

static void adc_init(void)
{
	// ADC2 (PC2) - Joystick X-Achse (VRx laut Verkabelung)
	ADMUX = (1 << REFS0) | (1 << MUX1);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static uint16_t adc_read(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
		;
	return ADC;
}

int main(void)
{
	// FIX 1: Direkte Übergabe der Baudrate anstatt des vorberechneten UBRR-Wertes
	uart_init(BAUD);
	matrix_init();
	adc_init();

	// Initialer Seed für rand() aus analogem Rauschen
	srand(adc_read());
	game_init();

	uint16_t debounce_counter = 0;
	uint8_t action_executed = 0;

	while (1)
	{
		uint16_t x_val = adc_read();

		if (x_val > 800 || x_val < 200)
		{
			if (!action_executed && game_is_over() == ONGOING)
			{
				game_player_turn();
				if (game_is_over() == ONGOING)
				{
					game_enemy_turn();
				}
				game_print_status();
				action_executed = 1;
				debounce_counter = 0;
			}
		}

		if (action_executed)
		{
			debounce_counter++;
			if (debounce_counter > 150 && x_val > 400 && x_val < 600)
			{
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