/*-----------------------------

MIT License

Copyright (c) 2020 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [actor.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "actor.h"
#include "attacks.h"
#include "utilities.h"

struct Actor g_actor[ACTORS_NO] =
{
	{ACTOR_STATE_IDLE, 0, 0, 0, 0, 0, 0, 0, 100, 0, TYPE_HERO_B},
	{ACTOR_STATE_IDLE, 0, 0, 0, 0, 0, 0, 0, 100, 30, TYPE_HERO_A},

	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, __NN__},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, __NN__},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, __NN__},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, __NN__}
};

struct Information g_info[ACTORS_NO] =
{
	{/* BaseX */ 45, /* BaseY */ 60},
	{/* BaseX */ 8,  /* BaseY */ 100},

	{/* BaseX */ 150, /* BaseY */ 60},
	{/* BaseX */ 180, /* BaseY */ 73},
	{/* BaseX */ 220, /* BaseY */ 86},
	{/* BaseX */ 250, /* BaseY */ 100}
};

#define SHOCK_BOUNDED_TIME 240

struct Personality g_persona[TYPES_NO] =
{
	{"Sayori", /* Idle */ 5, /* Bounded */ 60, /* Health in g_actor[] */ 0, /* Default damage */ 20, /* Charge */ 6},
	{"Kuro",   /* Idle */ 3, /* Bounded */ 60, /* Health in g_actor[] */ 0, /* Default damage */ 30, /* Charge */ 7},

	/* Well balanced */
	{"Ferment",   /* Idle */ 5, /* Bounded */ 60, /* Health */ 50, /* Damage */ 15, /* Charge */ 3},
	{"Wind Eye",  /* Idle */ 4, /* Bounded */ 30, /* Health */ 30, /* Damage */ 10, /* Charge */ 4}, /* Phantasy Star */

	/* Slow bullet sponges */
	{"Kingpin",   /* Idle */ 2, /* Bounded */ 90, /* Health */ 80, /* Damage */ 50, /* Charge */ 2}, /* Half-Life */
	{"Destroyer", /* Idle */ 2, /* Bounded */ 80, /* Health */ 100, /* Damage */ 30, /* Charge */ 3},

	/* Fast and delicate one */
	{"Phibia",    /* Idle */ 8, /* Bounded */ 15, /* Health */ 70, /* Damage */ 5, /* Charge */ 3},

	/* Well balanced, at this point of the game the enemies
	   number makes the actual challenge */
	{"Viridi",    /* Idle */ 4, /* Bounded */ 50, /* Health */ 90, /* Damage */ 15, /* Charge */ 5},
	{"Ni",        /* Idle */ 5, /* Bounded */ 50, /* Health */ 100, /* Damage */ 15, /* Charge */ 5}

	/* TODO: think better names
	- "Ferment", ok
	- "Destroyer", nope
	- "Phibia", a dragon is an amphibian?
	- "Viridi", mehhh... (viridiplantae)
	- "Ni", lovely */
};


void InitializeActor(uint8_t i, uint8_t* layout)
{
	/* Only enemies resets they health and type */
	if (i >= HEROES_NO)
	{
		g_actor[i].state = ACTOR_STATE_IDLE;

		if (layout != NULL)
			g_actor[i].type = layout[i - HEROES_NO];
		else
		{
			do
				g_actor[i].type = (Random() % TYPES_NO);
			while (g_actor[i].type < TYPE_A);
		}

		IntPrintText("# Enemy: ");
		IntPrintNumber(g_actor[i].type);

		if (g_actor[i].type == __NN__)
		{
			g_actor[i].state = ACTOR_STATE_DEAD;
			return;
		}
		else
		{
			g_actor[i].health = g_persona[g_actor[i].type].health;

			/* A random plus of health, 10 pts */
			if (g_actor[i].health <= 90)
				g_actor[i].health += (uint8_t)(Random() % 10);
		}
	}

	g_actor[i].phase = (uint8_t)Random();
	g_actor[i].charge_vel = g_persona[g_actor[i].type].charge_vel;

	if (i < HEROES_NO)
	{
		g_actor[i].idle_time = (uint8_t)(Random() % 64);

		if (g_actor[i].state != ACTOR_STATE_DEAD)
			g_actor[i].state = ACTOR_STATE_IDLE;
	}
	else
		g_actor[i].idle_time = 0;

	/* The rest just need to be zero */
	g_actor[i].target = 0;
	g_actor[i].attack_type = 0;
	g_actor[i].charge_time = 0;
	g_actor[i].bounded_time = 0;
}


void DrawActors()
{
	uint8_t i = 0;
	uint8_t color = 0;
	uint8_t width = 0;
	uint8_t frame = 0;
	uint16_t sin = 0;

	/* Clean area */
	CmdDrawRectangleBkg(20 /* 320 px */, 9 /* 144 px */, 0, 60);

	/* Draw actors */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		/* Sprite */
		sin = 0;
		frame = (g_actor[i].bounded_time == 0) ? 0 : 1;

		if (g_actor[i].state == ACTOR_STATE_CHARGE && g_actor[i].attack_type != ATTACK_HOLD)
			sin = Sin(g_actor[i].phase) >> 5;

		CmdDrawSprite(g_actor[i].type, g_info[i].base_x + sin, g_info[i].base_y, frame);

		/* Time meter background */
		CmdDrawRectanglePrecise(34, 3, g_info[i].base_x, g_info[i].base_y, 16);

		/* Time meter */
		if (g_actor[i].state == ACTOR_STATE_IDLE)
		{
			color = 8;
			width = (g_actor[i].idle_time >> 3); /* Max of 32 px */
		}
		else
		{
			color = 41;
			width = (g_actor[i].charge_time >> 3); /* Max of 32 px */
		}

		if (width == 0) /* Too tiny to draw it */
			continue;

		if (g_actor[i].bounded_time > 0)
			color = 60;

		CmdDrawRectanglePrecise(width, 1, g_info[i].base_x + 1, g_info[i].base_y + 1, color);
	}
}


void ActorIdle(uint8_t index)
{
	struct Actor* actor = &g_actor[index];

	/* Bounded!, lets wait some time */
	if (actor->bounded_time > 0)
	{
		actor->bounded_time -= 1;
		return;
	}

	/* Update idle timer */
	if (actor->idle_time < (255 - g_persona[actor->type].idle_time))
	{
		if ((CURRENT_FRAME % 2) == 0)
			actor->idle_time += g_persona[actor->type].idle_time;
	}

	/* Our turn!, time to change into 'charge' state */
	else
	{
		actor->charge_time = 0;
		actor->state = ACTOR_STATE_CHARGE;

		/* Select our attack */
		actor->attack_type = ATTACK_SIMPLE;

		/* Select our target... */
		{
		again:
			/* As an enemy */
			if (actor->type >= TYPE_A)
				actor->target = Random() % HEROES_NO;

			/* As an hero */
			else
				actor->target = HEROES_NO + (Random() % (ENEMIES_NO));

			/* Wait, the target is alive? */
			if (g_actor[actor->target].state == ACTOR_STATE_DEAD)
				goto again;
		}
	}
}


void ActorCharge(uint8_t index)
{
	struct Actor* actor = &g_actor[index];

	/* Bounded!, lets wait some time */
	if (actor->bounded_time > 0)
	{
		actor->bounded_time -= 1;
		return;
	}

	/* Oscillate in position */
	if (actor->attack_type != ATTACK_HOLD)
		actor->phase += 20;

	/* Update charge timer */
	if (actor->charge_time < (255 - actor->charge_vel))
		actor->charge_time += actor->charge_vel;

	/* Attack!!! */
	else
	{
		actor->state = ACTOR_STATE_ATTACK;
	}
}


void ActorAttack(uint8_t index)
{
	struct Actor* actor = &g_actor[index];
	struct Actor* target = &g_actor[actor->target];

	uint8_t damage = 0;

	actor->idle_time = 0;
	actor->state = ACTOR_STATE_IDLE;

	/* TODO, check if the target is still alive */

	if (target->attack_type == ATTACK_HOLD)
		return;

	target->bounded_time = g_persona[actor->target].bounded_time;

	if (actor->attack_type == ATTACK_SIMPLE) /* Monsters only use this one */
		damage = g_persona[actor->type].damage;
	else if (actor->attack_type == ATTACK_COMBINED)
		damage = g_persona[actor->type].damage << 1;
	else if (actor->attack_type == ATTACK_SHOCK)
	{
		damage = g_persona[actor->type].damage >> 2;
		target->bounded_time = SHOCK_BOUNDED_TIME;
	}
	else if (actor->attack_type == ATTACK_THUNDER)
		damage = 255;

	if (target->health > damage)
		target->health -= damage;
	else
	{
		target->health = 0;
		target->state = ACTOR_STATE_DEAD;
	}
}
