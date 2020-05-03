/*-----------------------------

 [actor.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_H
#define ACTOR_H

#include "engine.h"

#define ACTORS_NO 6
#define ENEMIES_NO 4
#define HEROES_NO 2

#define TYPES_NO 9

#define TYPE_HERO_B 0
#define TYPE_HERO_A 1
#define TYPE_A 2
#define TYPE_B 3
#define TYPE_C 4
#define TYPE_D 5
#define TYPE_E 6
#define TYPE_F 7
#define TYPE_G 8
#define __NN__ 0xFF

#define ACTOR_STATE_DEAD 0
#define ACTOR_STATE_IDLE 1
#define ACTOR_STATE_CHARGE 2
#define ACTOR_STATE_ATTACK 3

struct Information
{
	uint16_t base_x;
	uint16_t base_y;
};

struct Personality
{
	char name[12];
	uint8_t idle_time;
	uint8_t bounded_time;
	uint8_t health;
};

struct Actor
{
	uint8_t state;

	uint8_t target;
	uint8_t attack_type;

	uint8_t idle_time;   /* Counts from 0 to 255 */
	uint8_t charge_time; /* " */

	uint8_t bounded_time; /* Counts towards 0 */

	uint8_t phase;
	int8_t health;

	uint8_t type;
};

extern struct Actor g_actor[ACTORS_NO];
extern struct Information g_info[ACTORS_NO];
extern struct Personality g_persona[TYPES_NO];

void InitializeActor(uint8_t index, uint8_t* layout);
void DrawActors();
void ActorIdle(uint8_t index);
void ActorCharge(uint8_t index);
void ActorAttack(uint8_t index);

#endif
