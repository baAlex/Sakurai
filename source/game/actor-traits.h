/*-----------------------------

 [actor-traits.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_TRAITS_H
#define ACTOR_TRAITS_H

#include "actor.h"
#include "engine.h"

#define ACTIONS_NO 5
#define PERSONAS_NO 6

#define PERSONA_KURO 0
#define PERSONA_SAO 1
#define PERSONA_ENEMY_A 2
#define PERSONA_ENEMY_B 3
#define PERSONA_ENEMY_C 4
#define PERSONA_ENEMY_D 5
/*#define PERSONA_ENEMY_E 6
#define PERSONA_ENEMY_F 7
#define PERSONA_ENEMY_G 8*/

#define TAG_NONE 0
#define TAG_ENEMY 2
#define TAG_LEVITATES 4

struct Actor;
struct Action;

struct Action
{
	char* name;
	void (*callback)(struct Action*, struct Actor*, struct Actor*);
	uint8_t charge_velocity;

	/* Action dependent: */
	uint8_t amount;
};

struct Persona
{
	char* name;
	uint8_t tags;

	uint8_t idle_velocity;
	uint8_t recover_velocity;
	uint8_t initial_health;
	uint8_t initial_magic;

	uint8_t actions_preference;
	struct Action* action_a;
	struct Action* action_b;
};

struct Action g_action[ACTIONS_NO];    /* TraitsInitialize() */
struct Persona g_persona[PERSONAS_NO]; /* TraitsInitialize() */

void TraitsInitialize();

#endif
