#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

void matrix_init(void);
void matrix_set_row(uint8_t row, uint8_t data);
void matrix_clear(void);
void matrix_render(uint8_t frame[8]);
void matrix_blink_row(uint8_t row, uint8_t times);
uint8_t hp_to_bitmask(int8_t hp);

#endif
