#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum { ONGOING, WIN, LOSE } GameResult;

typedef struct {
    const char *name;
    int8_t      damage;
} Attack;

void        game_init(void);
void        game_player_attack(uint8_t attack_idx);
void        game_enemy_attack(void);
GameResult  game_is_over(void);
int8_t      game_player_hp(void);
int8_t      game_enemy_hp(void);

extern Attack attacks[];
extern uint8_t attack_count;

#endif
