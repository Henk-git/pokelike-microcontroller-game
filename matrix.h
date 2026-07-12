#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

void matrix_init(void);
void matrix_render(uint8_t frame[8]);
uint8_t hp_to_bitmask(int8_t hp);

#endif