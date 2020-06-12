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
#include "fixed.h"
#include "utilities.h"


extern uint8_t EnemiesNumber(uint8_t battle_no);
extern uint8_t EnemyChances(uint8_t enemy_i, ufixed_t battle_no);


static uint16_t s_chances[ENEMIES_NO];
struct Actor s_actor_temp;


uint8_t ActorsInitialize(uint8_t battle_no)
{
	uint8_t i = 0;
	uint8_t e = 0;
	uint8_t enemies_no = 0;
	uint16_t sum = 0;

	TraitsInitialize();

	/* Before the initialization we need to know, what kind and
	number of actors we actually need, this depending on the battle */
	enemies_no = EnemiesNumber(battle_no);

	IntPrintText("Enemies chances:\n");

	for (i = 0; i < ENEMIES_NO; i++)
	{
		s_chances[i] = EnemyChances(i, UFixedMake(battle_no, 0));
		sum += s_chances[i]; /* To normalizate */

		IntPrintNumber(s_chances[i]);
	}

	/* Initialize actors */
	Clear(g_actor, sizeof(struct Actor) * ACTORS_NO);

	g_actor[0].persona = &g_heroes[HERO_KURO]; /* For now, the first two actors are the heroes, */
	g_actor[1].persona = &g_heroes[HERO_SAO];  /* be caution of this trick on following 'for' and 'if' */

	for (i = 0; i < ACTORS_NO; i++)
	{
		g_actor[i].x = (uint16_t)i * 30;
		g_actor[i].y = (uint16_t)i * 30;

		g_actor[i].state = (i < HEROES_NO + enemies_no) ? ACTOR_STATE_IDLE : ACTOR_STATE_DEAD;
		g_actor[i].phase = (uint8_t)Random();

		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		/* If not an hero, chose a personality */
		if (i >= HEROES_NO)
		{
		again:
			e = ((uint8_t)Random() % ENEMIES_NO);
			if ((Random() % sum) <= s_chances[e])
				g_actor[i].persona = &g_enemies[e];
			else
				goto again; /* TODO, limit this to some tries */
		}

		/* If not a hero or if the battle zero (including the case of a hero)
		lets initialize actor health and magic based on their personality */
		if (i >= HEROES_NO || battle_no == 0)
		{
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
	}

	/* Ensure that difficult enemies don't appear more than twice */
	Clear(s_chances, sizeof(uint16_t) * ENEMIES_NO); /* To reuse it */

	for (i = HEROES_NO; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		e = EnemyPersonaIndex(g_actor[i].persona);
		s_chances[e] += 1;

		/* The two first enemies are to easy to consider them */
		if (e < 2)
			continue;

		/* Difficult enemy more than twice!!!, lets replace it
		with one of the two first enemies */
		if (s_chances[e] > 2)
		{
			IntPrintText("Replaced difficult enemy ");
			IntPrintNumber(e);

			g_actor[i].persona = &g_enemies[Random() % 2];
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
	}

	/* Shuffle positions, this invalidate the 'heroes first' trick */
	for (i = 0; i < ACTORS_NO; i++)
	{
		e = ((uint8_t)Random() % ACTORS_NO);
		Copy(&g_actor[e], &s_actor_temp, sizeof(struct Actor));
		Copy(&g_actor[i], &g_actor[e], sizeof(struct Actor));
		Copy(&s_actor_temp, &g_actor[i], sizeof(struct Actor));
	}

	return enemies_no;
}


void ActorsDraw()
{
	uint8_t i = 0;

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		CmdDrawRectangle(1, 1, g_actor[i].x, g_actor[i].y, 36);
	}
}
