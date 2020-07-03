/*-----------------------------

 [actor-traits.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_TRAITS_H
#define ACTOR_TRAITS_H

#include "actor.h"
#include "engine.h"

#define ACTIONS_NO 1

#define HEROES_PERSONALITIES_NO 2
#define ENEMIES_PERSONALITIES_NO 7
#define PERSONALITY_KURO 0
#define PERSONALITY_SAO 1

#define TAG_NONE 0
#define TAG_ENEMY 2
#define TAG_LEVITATES 4
#define TAG_DIFFICULT 8

struct Actor;
struct Action;

struct Action
{
	char* name;
	void (*callback)(struct Action*, struct Actor*);
	uint8_t charge_velocity;
	uint8_t oscillation_velocity;

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

	uint8_t sprite;
};

struct Action g_action[ACTIONS_NO];                 /* TraitsInitialize() */
struct Persona g_heroes[HEROES_PERSONALITIES_NO];   /* TraitsInitialize() */
struct Persona g_enemies[ENEMIES_PERSONALITIES_NO]; /* TraitsInitialize() */

void TraitsInitialize();

#define EnemyPersonaIndex(persona) ((uint8_t)((persona)-g_enemies))
#define HeroPersonaIndex(persona) ((uint8_t)((persona)-g_heroes))
#endif
