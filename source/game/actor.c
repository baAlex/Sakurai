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
#include "utilities.h"


extern void ActorsTraitsInitialize();
extern uint8_t EnemiesNumber(uint8_t battle_no);
extern uint8_t EnemyChances(uint8_t enemy_i, uint8_t battle_no);


void ActorsInitialize(uint8_t battle_no)
{
	uint8_t i = 0;
	uint8_t enemies_no = EnemiesNumber(battle_no);

	ActorsTraitsInitialize();

	for (i = 0; i < ACTORS_NO; i++)
	{
		Clear(&g_actor[i], sizeof(struct Actor));

		g_actor[i].x = (uint16_t)i * 10;
		g_actor[i].y = (uint16_t)i * 10;

		if (i < enemies_no + HEROES_NO)
			g_actor[i].state = ACTOR_STATE_IDLE;
		else
			g_actor[i].state = ACTOR_STATE_DEAD;
	}
}


void ActorsDraw()
{
	uint8_t i = 0;

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		CmdDrawSprite(20, g_actor[i].x, g_actor[i].y, 0);
	}
}
