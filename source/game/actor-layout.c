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

 [actor-layout.c]
 - Alexander Brandt 2020

 $ clang -DLAYOUT_STANDALONE -std=c90 -Wall -Wextra -Wconversion -pedantic ./source/game/actor-layout.c
./source/game/utilities.c ./source/game/fixed.c -o layout

 $ bcc -0 -ansi -Md -DLAYOUT_STANDALONE ./source/game/actor-layout.c ./source/game/utilities.c ./source/game/fixed.c -o
layout.exe
-----------------------------*/

#include "actor.h"
#include "fixed.h"
#include "utilities.h"


#define NOISE_GATE 40
#define NOISE_MIN 2
#define NOISE_MAX 2


uint8_t EnemiesNumber(uint8_t battle_no)
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
	return (uint8_t)(CLAMP((triangle + sawtooth) >> 1, 1, ENEMIES_NO));
}


#ifndef LAYOUT_STANDALONE
#define BATTLES_NO 32
#else
#define BATTLES_NO 16 /* To fit on a DOS screen :( */
#endif

#define CHANCES_ATTACK 3 /* In number of battles*/
#define CHANCES_DECAY 6  /* Same */
#define BATTLES_DIV_ENEMIES 8 /* (BATTLES_NO / ENEMIES_NO) */

ufixed_t sImaginaryLine(ufixed_t battle_no)
{
	ufixed_t chances;

	chances = UFixedStep(UFixedMake(0, 0), UFixedMake(BATTLES_NO, 0), battle_no);
	chances = UFixedDivide(chances, UFixedMake(ENEMIES_NO, 0));

	return chances;
}

uint8_t EnemyChances(uint8_t enemy_i, ufixed_t battle_no)
{
	ufixed_t chances = UFixedMake(0, 0);

	ufixed_t attack_start = UFixedMultiply(UFixedMake(BATTLES_DIV_ENEMIES, 0), UFixedMake(enemy_i, 0));
	ufixed_t attack_end = attack_start + UFixedMake(CHANCES_ATTACK, 0);
	ufixed_t decay_end = attack_start + UFixedMake(CHANCES_ATTACK + CHANCES_DECAY, 0);

	battle_no += UFixedMake(1, 0);

	if (battle_no >= attack_start)
	{
		/* 1 - Attack */
		if (battle_no <= attack_end)
			chances = UFixedStep(attack_start, attack_end, battle_no);

		/* 2 - Decay */
		else
		{
			chances = UFixedMake(1, 0) - UFixedStep(attack_end, decay_end, battle_no);

			/* Keep it over an imaginary line! */
			if (chances < sImaginaryLine(battle_no))
				chances = sImaginaryLine(battle_no);
		}
	}

	/* Bye! */
	return FixedWhole(UFixedMultiply(chances, UFixedMake(100, 0)));
}


#ifdef LAYOUT_STANDALONE
#include <stdio.h>

int main()
{
	uint8_t enemy = 0;
	uint8_t battle = 0;

	for (enemy = 0; enemy < ENEMIES_NO; enemy++)
	{
		for (battle = 0; battle < (BATTLES_NO + CHANCES_DECAY); battle++)
		{
			if (battle == 0)
				printf("Enemy %i\n", enemy);

			printf("%i\n", EnemyChances(enemy, UFixedMake(battle, 0)));
		}

		getc(stdin);
	}

	return 0;
}

#endif
