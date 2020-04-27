/*-----------------------------

 [game-private.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef GAME_PRIVATE_H
#define GAME_PRIVATE_H

#include "game.h"

/* How many actors in screen */
#define ACTORS_NO 6

/* Two of the six actors being heroes */
#define HEROES_NO 2

/* With seven actors type, plus the heroes */
#define TYPES_NO 9
#define TYPE_HERO_B 0 /* Heroes always at begining, as it let us */
#define TYPE_HERO_A 1 /* do a cheap 'if less than' to identity them */

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

struct Information info[ACTORS_NO] =
{
	{/* BaseX */ 38, /* BaseY */ 60},
	{/* BaseX */ 8,  /* BaseY */ 100},

	{/* BaseX */ 150, /* BaseY */ 60},
	{/* BaseX */ 180, /* BaseY */ 73},
	{/* BaseX */ 220, /* BaseY */ 86},
	{/* BaseX */ 250, /* BaseY */ 100},

};

struct Personality persona[TYPES_NO] =
{
	{"Sayori", /* Idle */ 5, /* Health */ 100},
	{"Kuro",   /* Idle */ 4, /* Health */ 100},

	{"Ferment",   /* Idle */ 1, /* Health */ 40},
	{"Wind Eye",  /* Idle */ 1, /* Health */ 50}, /* Phantasy Star */
	{"Kingpin",   /* Idle */ 2, /* Health */ 60}, /* Half-Life */
	{"Destroyer", /* Idle */ 3, /* Health */ 70},
	{"Phibia",    /* Idle */ 4, /* Health */ 80},
	{"Viridi",    /* Idle */ 5, /* Health */ 90},
	{"Ni",        /* Idle */ 6, /* Health */ 100}

	/* TODO: think better names
	- "Ferment", ok
	- "Destroyer", nope
	- "Phibia", a dragon is an amphibian?
	- "Viridi", mehhh... (viridiplantae)
	- "Ni", lovely */
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

struct Actor actor[ACTORS_NO] =
{
	{0, 0, 0, 0, 0, 0, 0, 100, TYPE_HERO_A},
	{0, 0, 0, 0, 0, 0, 0, 100, TYPE_HERO_B}
};

#endif
