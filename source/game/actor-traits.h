/*-----------------------------

 [actor-traits.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_TRAITS_H
#define ACTOR_TRAITS_H

#include "actor.h"
#include "engine.h"

#define ACTIONS_NO 5
#define HEROES_NO 2
#define ENEMIES_NO 7

#define HERO_KURO 0
#define HERO_SAO 1

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
	char* sprite_filename;

	struct Action* action_a;
	struct Action* action_b;

	uint8_t tags;
	uint8_t actions_preference;

	uint8_t idle_velocity;
	uint8_t recover_velocity;

	uint8_t initial_health;
	uint8_t initial_magic;
};

struct Action g_action[ACTIONS_NO];   /* TraitsInitialize() */
struct Persona g_heroes[HEROES_NO];   /* TraitsInitialize() */
struct Persona g_enemies[ENEMIES_NO]; /* TraitsInitialize() */

void TraitsInitialize();

#define EnemyPersonaIndex(persona) ((uint8_t)((persona) - g_enemies))
#endif
