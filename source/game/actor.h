/*-----------------------------

 [actor.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_H
#define ACTOR_H

#include "actor-traits.h"
#include "engine.h"

#define ACTORS_NO 6

#define ACTOR_STATE_DEAD 0
#define ACTOR_STATE_IDLE 1
#define ACTOR_STATE_CHARGE 2
#define ACTOR_STATE_ATTACK 3
#define ACTOR_STATE_VICTORY 4

#define ACTOR_KURO 0
#define ACTOR_SAO 1

struct Action;
struct Persona;

struct Actor
{
	struct Persona* persona;
	struct Action* action;
	uint16_t x;
	uint16_t y;

	struct Actor* target;
	uint8_t state;

	uint8_t health;
	uint8_t magic;
	uint8_t idle_timer;    /* From 0 to 255 */
	uint8_t charge_timer;  /* From 0 to 255 */
	uint8_t recover_timer; /* From 255 to 0 */

	uint8_t phase;
};

uint8_t g_live_heroes;           /* ActorsInitialize() */
uint8_t g_live_enemies;          /* ActorsInitialize() */
struct Actor g_actor[ACTORS_NO]; /* ActorsInitialize() */

void ActorsInitialize(uint8_t battle_no);
void ActorsInitializeSprites(); /* TODO, temporary until I write a 'resources' module */

void ActorLogic(struct Actor* actor);
void ActorsDraw();

#endif
