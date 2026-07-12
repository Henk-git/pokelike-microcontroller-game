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
	// 10% Chance zu verfehlen (TCNT0 l�uft frei)
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