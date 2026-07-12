#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef enum { DIR_LEFT, DIR_CENTER, DIR_RIGHT } Direction;

void input_init(void);
Direction joystick_direction(void);
uint8_t button_pressed(void);

#endif
