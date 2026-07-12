#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define PLAYER_MAX_HP 120
#define ENEMY_MAX_HP  120

typedef enum { ONGOING, WIN, LOSE } GameResult;

void        game_init(void);
void        game_player_turn(void);
void        game_enemy_turn(void);
void        game_print_status(void);
GameResult  game_is_over(void);
int8_t      game_player_hp(void);
int8_t      game_enemy_hp(void);

#endif