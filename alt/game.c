#include "game.h"
#include "uart.h"
#include <avr/io.h>

// zwei Attacken zur Auswahl
Attack attacks[] = {
    { "TACKLE",       15 },
    { "THUNDERSHOCK", 25 },
};
uint8_t attack_count = 2;

static int8_t player_hp;
static int8_t enemy_hp;

// Spiel zurücksetzen
void game_init(void) {
    player_hp = 8;
    enemy_hp  = 8;
    uart_println("=== POKEBATTLE START ===");
    uart_print("Your HP: 8 | Enemy HP: 8");
    uart_putc('\r'); uart_putc('\n');
}

// Spieler greift an
void game_player_attack(uint8_t idx) {
    if (idx >= attack_count) return;
    enemy_hp -= attacks[idx].damage;
    if (enemy_hp < 0) enemy_hp = 0;

    uart_print("> Du benutzt ");
    uart_print(attacks[idx].name);
    uart_print("! (");
    uart_print_int(attacks[idx].damage);
    uart_println(" Schaden)");
    uart_print("  Gegner HP: ");
    uart_print_int(enemy_hp);
    uart_putc('\r'); uart_putc('\n');
}

// Gegner greift mit festem Schaden zurück
void game_enemy_attack(void) {
    // TCNT1 als einfacher Zufallswert
    uint8_t idx = (TCNT1 & 0x01);
    int8_t dmg = attacks[idx].damage - 5;

    player_hp -= dmg;
    if (player_hp < 0) player_hp = 0;

    uart_print("> Gegner benutzt ");
    uart_print(attacks[idx].name);
    uart_print("! (");
    uart_print_int(dmg);
    uart_println(" Schaden)");
    uart_print("  Deine HP: ");
    uart_print_int(player_hp);
    uart_putc('\r'); uart_putc('\n');
}

// Spielstand prüfen
GameResult game_is_over(void) {
    if (enemy_hp  <= 0) return WIN;
    if (player_hp <= 0) return LOSE;
    return ONGOING;
}

int8_t game_player_hp(void) { return player_hp; }
int8_t game_enemy_hp(void)  { return enemy_hp;  }
