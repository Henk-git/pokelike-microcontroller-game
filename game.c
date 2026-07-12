#include "game.h"
#include "uart.h"
#include <avr/io.h>
#include <stdlib.h>

static int8_t player_hp;
static int8_t enemy_hp;

static void print_bar(int8_t hp)
{
	uint8_t leds = 0;
	if (hp > 0)
		leds = ((uint8_t)hp + 14) / 15;
	if (leds > 8)
		leds = 8;
	uart_putc('[');
	for (uint8_t i = 0; i < 8; i++)
		uart_putc(i < leds ? '#' : '.');
	uart_print("] ");
	uart_print_int(hp);
	uart_print("/120");
}

void game_print_status(void)
{
	uart_print("  Du      ");
	print_bar(player_hp);
	uart_print("\r\n  Gegner  ");
	print_bar(enemy_hp);
	uart_print("\r\n\r\n");
}

void game_init(void)
{
	player_hp = PLAYER_MAX_HP;
	enemy_hp = ENEMY_MAX_HP;
	uart_print("\r\n\r\n");
	uart_print("+=======================+\r\n");
	uart_print("|  P O K E B A T T L E  |\r\n");
	uart_print("+=======================+\r\n");
	uart_print("Joystick kippen = Angriff!\r\n\r\n");
	game_print_status();
}

void game_player_turn(void)
{
	// 10% Chance zu verfehlen
	if (rand() % 10 < 1)
	{
		uart_print("> Dein Angriff geht daneben!\r\n");
		return;
	}
	enemy_hp -= 15;
	if (enemy_hp < 0)
		enemy_hp = 0;
	uart_print("> Volltreffer! 15 Schaden!\r\n");
	if (enemy_hp == 0)
		uart_print("\r\n*** SIEG! Gegner besiegt! ***\r\n");
}

void game_enemy_turn(void)
{
	// 50% Chance zu verfehlen
	if (rand() % 2 == 0)
	{
		uart_print("> Gegner verfehlt!\r\n");
		return;
	}
	player_hp -= 15;
	if (player_hp < 0)
		player_hp = 0;
	uart_print("> Gegner trifft! 15 Schaden!\r\n");
	if (player_hp == 0)
		uart_print("\r\n*** NIEDERLAGE! Du wurdest besiegt! ***\r\n");
}

GameResult game_is_over(void)
{
	if (enemy_hp <= 0)
		return WIN;
	if (player_hp <= 0)
		return LOSE;
	return ONGOING;
}

int8_t game_player_hp(void) { return player_hp; }
int8_t game_enemy_hp(void) { return enemy_hp; }