/*-----------------------------

 [game-private.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef GAME_PRIVATE_H
#define GAME_PRIVATE_H

#include "engine.h"

/* How many actors in screen */
#define ACTORS_NO 6
#define ENEMIES_NO 4

/* Two of the six actors being heroes */
#define HEROES_NO 2

/* With seven actors type, plus the heroes */
#define TYPES_NO 9

#define TYPE_HERO_B 0 /* Heroes always at begining, as it let us */
#define TYPE_HERO_A 1 /* do a cheap 'if less than' to identity them */

#define TYPE_A 2
#define TYPE_B 3
#define TYPE_C 4
#define TYPE_D 5
#define TYPE_E 6
#define TYPE_F 7
#define TYPE_G 8
#define __NN__ 0xFF

/* States for our actors */
#define STATE_DEAD 0
#define STATE_IDLE 1
#define STATE_CHARGE 2


/*
 Constant data
 -------------
*/
struct Information
{
	uint16_t base_x;
	uint16_t base_y;
};

struct Personality
{
	char name[12];
	uint8_t idle_time;
	uint8_t health;
};


/*
 Actors state, modified at runtime
 ---------------------------------
*/
struct Actor
{
	uint8_t state;

	uint8_t target;
	uint8_t attack_type;

	uint8_t idle_time;    /* Counts from 0 to 255 */
	uint8_t charge_time;  /* " */
	uint8_t bounded_time; /* " */

	uint8_t phase;
	int8_t health;

	uint8_t type;
};

#endif
