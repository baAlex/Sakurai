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


#define NOISE_GATE 40
#define NOISE_MIN 2
#define NOISE_MAX 2


extern void ActorsTraitsInitialize();


static uint8_t sEnemiesNumber(uint8_t battle_no)
{
	int16_t sawtooth = 0;
	int16_t triangle = 0;

	/* First battle always has one enemy */
	if (battle_no == 0)
		return 1;

	/* Sawtooth */
	sawtooth = ((int16_t)battle_no) >> 1;
	sawtooth = sawtooth % (ENEMIES_NO);
	sawtooth += 1;

	/* Triangle */
	triangle = ((int16_t)battle_no) >> 1;

	if (triangle % ((ENEMIES_NO - 1) << 1) < (ENEMIES_NO - 1))
		triangle = triangle % (ENEMIES_NO - 1);
	else
		triangle = (ENEMIES_NO - 1) - triangle % (ENEMIES_NO - 1);

	triangle += 1;

	/* Add noise */
	if ((Random() % 100) < NOISE_GATE)
	{
		triangle -= Random() % NOISE_MIN;
		triangle += Random() % NOISE_MAX;
		sawtooth -= Random() % NOISE_MIN;
		sawtooth += Random() % NOISE_MAX;
	}

	/* Yay! */
	return (uint8_t)CLAMP((triangle + sawtooth) >> 1, 1, ENEMIES_NO);
}


void ActorsInitialize(uint8_t battle_no)
{
	uint8_t i = 0;
	uint8_t enemies_no = sEnemiesNumber(battle_no);

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
