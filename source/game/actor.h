/*-----------------------------

 [actor.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_H
#define ACTOR_H

#include "actor-traits.h"
#include "engine.h"

#define ACTORS_NO 6
#define HEROES_NO 2
#define ENEMIES_NO 7

#define ACTOR_STATE_DEAD 0
#define ACTOR_STATE_IDLE 1
#define ACTOR_STATE_CHARGE 2
#define ACTOR_STATE_ATTACK 3

struct Action;
struct Persona;

struct Actor
{
	struct Persona* persona;
	uint8_t health;
	uint8_t magic;
	uint8_t state;

	uint8_t idle_timer;
	uint8_t recover_timer;
	uint8_t charge_timer;

	struct Action* action;

	uint16_t x;
	uint16_t y;
	uint8_t phase;
};

struct Actor g_actor[ACTORS_NO]; /* ActorsInitialize() */

uint8_t ActorsInitialize(uint8_t battle_no);
void ActorsDraw();

#endif
