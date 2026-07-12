#include "matrix.h"
#include <avr/io.h>
#include <util/delay.h>

// Pins: LATCH=PB0, CLOCK=PB1, DATA=PB2
#define LATCH_PIN PB0
#define CLOCK_PIN PB1
#define DATA_PIN  PB2

// ein Byte per Schieberegister senden (MSB zuerst)
static void shift_out(uint8_t row, uint8_t data) {
    PORTB &= ~(1 << LATCH_PIN);

    // Zeile senden
    for (int8_t i = 7; i >= 0; i--) {
        PORTB &= ~(1 << CLOCK_PIN);
        if (row & (1 << i)) PORTB |= (1 << DATA_PIN);
        else                 PORTB &= ~(1 << DATA_PIN);
        PORTB |= (1 << CLOCK_PIN);
    }

    // Spaltendaten senden
    for (int8_t i = 7; i >= 0; i--) {
        PORTB &= ~(1 << CLOCK_PIN);
        if (data & (1 << i)) PORTB |= (1 << DATA_PIN);
        else                  PORTB &= ~(1 << DATA_PIN);
        PORTB |= (1 << CLOCK_PIN);
    }

    PORTB |= (1 << LATCH_PIN);
}

// Ausgangspins setzen
void matrix_init(void) {
    DDRB |= (1 << LATCH_PIN) | (1 << CLOCK_PIN) | (1 << DATA_PIN);
    matrix_clear();
}

// eine Zeile setzen (row 0-7, data = Bitmuster)
void matrix_set_row(uint8_t row, uint8_t data) {
    shift_out(1 << row, data);
}

// alles aus
void matrix_clear(void) {
    for (uint8_t i = 0; i < 8; i++) matrix_set_row(i, 0x00);
}

// kompletten Frame anzeigen (Multiplexing)
void matrix_render(uint8_t frame[8]) {
    for (uint8_t i = 0; i < 8; i++) matrix_set_row(i, frame[i]);
}

// eine Zeile blinken lassen
void matrix_blink_row(uint8_t row, uint8_t times) {
    uint8_t saved = 0xFF;
    for (uint8_t i = 0; i < times; i++) {
        matrix_set_row(row, 0x00);
        _delay_ms(120);
        matrix_set_row(row, saved);
        _delay_ms(120);
    }
}

// HP-Wert (0-8) in LED-Bitmuster umrechnen
uint8_t hp_to_bitmask(int8_t hp) {
    if (hp <= 0) return 0x00;
    if (hp >= 8) return 0xFF;
    return (uint8_t)(0xFF << (8 - hp));
}
