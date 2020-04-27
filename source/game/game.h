/*-----------------------------

 [game.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef GAME_H
#define GAME_H

#include "engine.h"

#define TYPE_A 2
#define TYPE_B 3
#define TYPE_C 4
#define TYPE_D 5
#define TYPE_E 6
#define TYPE_F 7
#define TYPE_G 8
#define __NN__ 99

struct Battle
{
	uint8_t layout_types[4];
};

struct Battle current_battle;

void* GameStart();

#endif
