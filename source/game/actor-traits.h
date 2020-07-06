/*-----------------------------

 [actor-traits.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_TRAITS_H
#define ACTOR_TRAITS_H

#include "actor.h"
#include "engine.h"

#define ACTIONS_NO 7

#define ACTION_GENERIC 0
#define ACTION_SIMPLE 1
#define ACTION_COMBINED 2
#define ACTION_SHOCK 3
#define ACTION_THUNDER 4
#define ACTION_HEAL 5
#define ACTION_MEDITATE 6

#define HEROES_PERSONALITIES_NO 2
#define ENEMIES_PERSONALITIES_NO 7
#define PERSONALITY_KURO 0
#define PERSONALITY_SAO 1

#define TAG_PERSONA_NONE 0
#define TAG_PERSONA_ENEMY 2
#define TAG_PERSONA_LEVITATES 4
#define TAG_PERSONA_DIFFICULT 8

#define TAG_ACTION_NONE 0
#define TAG_ACTION_PARTY 1

struct Actor;
struct Action;

struct Action
{
	void (*callback)(struct Action*, struct Actor*);
	uint8_t charge_velocity;
	uint8_t oscillation_velocity;
	uint8_t magic_cost;
	uint8_t tags;

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
	uint8_t generic_damage; /* ACTION_GENERIC */
};

struct Action g_action[ACTIONS_NO];                 /* TraitsInitialize() */
struct Persona g_heroes[HEROES_PERSONALITIES_NO];   /* TraitsInitialize() */
struct Persona g_enemies[ENEMIES_PERSONALITIES_NO]; /* TraitsInitialize() */

void TraitsInitialize();

#define EnemyPersonaIndex(persona) ((uint8_t)((persona)-g_enemies))
#define HeroPersonaIndex(persona) ((uint8_t)((persona)-g_heroes))
#endif
