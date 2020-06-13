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
	uint8_t enemies_no;
	uint16_t sum = 0;

	enemies_no = EnemiesNumber(battle_no);

	TraitsInitialize();

	/* Enemies chances for this battle */
	IntPrintText("Enemies chances:\n");

	for (i = 0; i < ENEMIES_NO; i++)
	{
		s_chances[i] = EnemyChances(i, UFixedMake(battle_no, 0));
		sum += s_chances[i]; /* To normalizate */

		IntPrintNumber(s_chances[i]);
	}

	/* Reset heroes health and magic only if is the first battle */
	if (battle_no == 0)
	{
		for (i = 0; i < HEROES_NO; i++)
		{
			Clear(&g_actor[i], sizeof(struct Actor));

			g_actor[i].state = ACTOR_STATE_IDLE;
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
	}

	/* Initialize heroes constant information */
	g_actor[0].persona = &g_heroes[HERO_KURO];
	g_actor[0].x = 45;
	g_actor[0].y = 60;
	g_actor[0].phase = (uint8_t)Random();

	g_actor[1].persona = &g_heroes[HERO_SAO];
	g_actor[1].x = 16;
	g_actor[1].y = 96;
	g_actor[1].phase = (uint8_t)Random();

	/* Initialize enemies, the remainder actors */
	{
		/* Set the state and chose a personality between the chances */
		for (i = HEROES_NO; i < ACTORS_NO; i++)
		{
			Clear(&g_actor[i], sizeof(struct Actor));

			if (i >= HEROES_NO + enemies_no) /* Exceeded number of enemies for this battle */
			{
				g_actor[i].state = ACTOR_STATE_DEAD;
				continue;
			}

			g_actor[i].state = ACTOR_STATE_IDLE;
			g_actor[i].phase = (uint8_t)Random();

		again:

			e = ((uint8_t)Random() % ENEMIES_NO);
			if ((Random() % sum) <= s_chances[e])
				g_actor[i].persona = &g_enemies[e];
			else
				goto again; /* TODO, limit this to some tries */
		}

		/* Shuffle positions, including those of enemies set as STATE_DEAD */
		for (i = HEROES_NO; i < ACTORS_NO; i++)
		{
			e = ((uint8_t)Random() % (ACTORS_NO - HEROES_NO)) + HEROES_NO;
			Copy(&g_actor[e], &s_actor_temp, sizeof(struct Actor));
			Copy(&g_actor[i], &g_actor[e], sizeof(struct Actor));
			Copy(&s_actor_temp, &g_actor[i], sizeof(struct Actor));
		}

		/* One last iteration */
		Clear(s_chances, sizeof(uint16_t) * ENEMIES_NO); /* To reuse it */

		for (i = HEROES_NO; i < ACTORS_NO; i++)
		{
			/* Set screen position */
			if (i != HEROES_NO)
			{
				g_actor[i].x = g_actor[i - 1].x + 32;
				g_actor[i].y = g_actor[i - 1].y + 12;
			}
			else
			{
				g_actor[i].x = 144;
				g_actor[i].y = 60;
			}

			if (g_actor[i].state == ACTOR_STATE_DEAD)
				continue;

			/* Ensure that difficult enemies don't appear more than twice */
			e = EnemyPersonaIndex(g_actor[i].persona);
			s_chances[e] += 1; /* Reused to count appearances */

			if ((g_actor[i].persona->tags & TAG_DIFFICULT) && s_chances[e] > 2)
			{
				IntPrintText("Replaced difficult enemy ");
				IntPrintNumber(e);
				g_actor[i].persona = &g_enemies[Random() % 2]; /* HARDCODED */
			}

			/* Finally set health and magic based on the personality */
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
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

		CmdDrawRectangle(4, 6, g_actor[i].x, g_actor[i].y, 36);
	}
}
