#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "matrix.h"
#include "input.h"
#include "game.h"

// Frame aufbauen: Reihe 0 = Gegner HP, Reihe 7 = Spieler HP, Reihe 6 = Cursor
static void draw_frame(uint8_t cursor) {
    uint8_t frame[8] = {0};
    frame[0] = hp_to_bitmask(game_enemy_hp());
    frame[7] = hp_to_bitmask(game_player_hp());
    // Cursor: Position 0 = linke LED, Position 1 = rechte LED
    frame[6] = (cursor == 0) ? 0b10000000 : 0b00001000;
    matrix_render(frame);
}

int main(void) {
    uart_init(115200);
    matrix_init();
    input_init();
    game_init();

    uint8_t cursor = 0; // 0 = TACKLE, 1 = THUNDERSHOCK

    while (1) {
        // Joystick auslesen
        Direction dir = joystick_direction();
        if      (dir == DIR_LEFT  && cursor != 0) { cursor = 0; _delay_ms(200); }
        else if (dir == DIR_RIGHT && cursor != 1) { cursor = 1; _delay_ms(200); }

        draw_frame(cursor);

        // Taster gedrückt → angreifen
        if (button_pressed()) {
            game_player_attack(cursor);
            draw_frame(cursor);
            _delay_ms(600);

            // Gegner antwortet
            if (game_is_over() == ONGOING) {
                game_enemy_attack();
                matrix_blink_row(7, 3); // Spieler-HP-Reihe blinkt
                draw_frame(cursor);
                _delay_ms(400);
            }

            // Ergebnis prüfen
            GameResult result = game_is_over();
            if (result == WIN) {
                uart_println("*** DU GEWINNST! ***");
                // Sieges-Animation: alle Reihen nacheinander
                for (uint8_t i = 0; i < 3; i++) {
                    uint8_t f[8];
                    for (uint8_t r = 0; r < 8; r++) f[r] = 0xFF;
                    matrix_render(f);
                    _delay_ms(300);
                    matrix_clear();
                    _delay_ms(300);
                }
                // Reset
                game_init();
                cursor = 0;
            } else if (result == LOSE) {
                uart_println("*** DU VERLIERST! ***");
                // Niederlage-Animation: alle Reihen aus
                for (uint8_t i = 0; i < 3; i++) {
                    matrix_clear();
                    _delay_ms(300);
                    uint8_t f[8];
                    for (uint8_t r = 0; r < 8; r++) f[r] = 0xFF;
                    matrix_render(f);
                    _delay_ms(300);
                }
                matrix_clear();
                _delay_ms(2000);
                game_init();
                cursor = 0;
            }
        }
    }
}
