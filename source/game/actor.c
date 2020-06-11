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


uint8_t ActorsInitialize(uint8_t battle_no)
{
	uint8_t i = 0;
	uint8_t enemies_no = 0;

	uint16_t sum = 0;
	uint8_t random = 0;

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
	g_actor[0].persona = &g_persona[PERSONA_KURO];
	g_actor[1].persona = &g_persona[PERSONA_SAO];

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (i >= HEROES_NO)
			Clear(&g_actor[i], sizeof(struct Actor));

		g_actor[i].x = (uint16_t)i * 30;
		g_actor[i].y = (uint16_t)i * 30;

		g_actor[i].state = (i < enemies_no + HEROES_NO) ? ACTOR_STATE_IDLE : ACTOR_STATE_DEAD;
		g_actor[i].phase = (uint8_t)Random();

		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		if (i >= HEROES_NO)
		{
		again:
			random = ((uint8_t)Random() % ENEMIES_NO);
			if ((Random() % sum) <= s_chances[random])
				g_actor[i].persona = &g_persona[HEROES_NO + random];
			else
				goto again; /* TODO, limit this to some tries */
		}

		if (i >= HEROES_NO || battle_no == 0)
		{
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

		CmdDrawRectangle(1, 1, g_actor[i].x, g_actor[i].y, 36);
	}
}
