/*-----------------------------

 [game-private.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef GAME_PRIVATE_H
#define GAME_PRIVATE_H

#include "engine.h"

/* Two of the six actors are heroes */
#define HEROES_NO 2
#define ACTORS_NO 6

/* Like DOOM, there are seven actors types, plus the heroes */
#define TYPES_NO 9

#define TYPE_HERO_A 0 /* Heroes always at begining! */
#define TYPE_HERO_B 1
#define TYPE_A 2
#define TYPE_B 3
#define TYPE_C 4
#define TYPE_D 5
#define TYPE_E 6
#define TYPE_F 7
#define TYPE_G 8


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
	uint8_t idle_time;
	uint8_t health;
};

struct Information info[ACTORS_NO] = {
    {/* BaseX */ 38, /* BaseY */ 60},
    {/* BaseX */ 8,  /* BaseY */ 100},

    {/* BaseX */ 180, /* BaseY */ 60},
    {/* BaseX */ 202, /* BaseY */ 73},
    {/* BaseX */ 225, /* BaseY */ 86},
    {/* BaseX */ 248, /* BaseY */ 100},

};

struct Personality persona[TYPES_NO] = {
    {/* Idle */ 5, /* Health */ 100},
    {/* Idle */ 4, /* Health */ 100},

    {/* Idle */ 1, /* Health */ 40},
    {/* Idle */ 1, /* Health */ 50},
    {/* Idle */ 2, /* Health */ 60},
    {/* Idle */ 3, /* Health */ 70},
    {/* Idle */ 4, /* Health */ 80},
    {/* Idle */ 5, /* Health */ 90},
    {/* Idle */ 6, /* Health */ 100},
};


/*
 Actors state, modified at runtime
 ---------------------------------
*/
struct Actor
{
	uint16_t x;

	uint8_t target;
	uint8_t attack_type;

	uint8_t state;
	uint8_t next_state;

	uint8_t idle_time;    /* Counts from 0 to 255 */
	uint8_t charge_time;  /* " */
	uint8_t bounded_time; /* " */

	uint8_t phase;
	int8_t health;

	uint8_t type;
};

struct Actor actor[ACTORS_NO] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

#endif
