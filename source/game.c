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

 [game.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "game.h"


int main()
{
	union Instruction* ins;
	uint8_t i;

	uint16_t clean_min_x = UINT16_MAX;
	uint16_t clean_max_x = 0;

	/* Iterate actors, update them */
	for (i = 0; i < ACTORS_NUMBER; i++)
	{
		/* Save position that comes from previous frame
		   in min/max form to allow the below 'draw step'
		   clean the screen */
		clean_min_x = MIN(clean_min_x, s_actor[i].x);
		clean_max_x = MAX(clean_max_x, s_actor[i].x);

		/* Nothing more, we ded' */
		if (s_actor[i].type == TYPE_DEAD)
			continue;

		/* We received damage on the previous frame,
		   wait some time rather than usual conduct */
		if (s_actor[i].recovery_time != 0)
		{
			s_actor[i].recovery_time -= 1;
			continue;
		}

		/* Oscillate in position */
		s_actor[i].phase += 4;
		s_actor[i].x = s_base_x[i] + (Sin(s_actor[i].phase) >> 5);

		/* 1 - Waiting for our turn */
		if (s_actor[i].common_time < 170)
		{
			s_actor[i].common_time += s_idle_time[s_actor[i].type];

			if (s_actor[i].common_time > 170)
				s_actor[i].common_time = 170;
		}

		/* 2 - Select target and attack type */
		else if (s_actor[i].common_time == 170)
		{
			s_actor[i].common_time += 1;
			s_actor[i].attack_type = Random() % UINT8_MAX;

			/* We are heroes, so lets chose an enemy */
			if (i < 2)
				s_actor[i].target = 2 + (Random() % (ACTORS_NUMBER - 2));

			/* The same, but from the other side */
			else
				s_actor[i].target = Random() % 2;
		}

		/* 3 - Preparing attack! */
		else if (s_actor[i].common_time != 255)
		{
			s_actor[i].common_time += 2;

			if (s_actor[i].common_time < 170)
				s_actor[i].common_time = 255;
		}

		/* 4 - Attack (with a cool animation) */
		else
		{
			s_actor[i].common_time += 1;

			/* Inflict damage in our victim */
			s_actor[s_actor[i].target].health -= 5;
			s_actor[s_actor[i].target].recovery_time = 24; /* One second */

			/* Set if victim died */
			if (s_actor[s_actor[i].target].health <= 0)
				s_actor[s_actor[i].target].type = TYPE_DEAD;

			/* Draw an rectangle to see who is attacking */
			ins = NewInstruction(CODE_DRAW_RECTANGLE);
			ins->draw.color = ((i < 2) ? 36 : 58) + i;
			ins->draw.x = 129;
			ins->draw.y = 0;
			ins->draw.width = 1;  /* 16 px */
			ins->draw.height = 1; /* 16 px */

			/* And our poor victim */
			ins = NewInstruction(CODE_DRAW_RECTANGLE);
			ins->draw.color = ((s_actor[i].target < 2) ? 36 : 58) + s_actor[i].target;
			ins->draw.x = 145;
			ins->draw.y = 0;
			ins->draw.width = 1;  /* 16 px */
			ins->draw.height = 1; /* 16 px */
		}
	}

	/* Change the background every 10 seconds,
	   is just to test this functionality */
	if ((*s_frame % 240) == 0)
	{
		/* Print something */
		PrintString((uint16_t)"Something\n");
		PrintNumber(0x1234);

		/* Load and draw it */
		ins = NewInstruction(CODE_LOAD_BKG);

		switch (Random() % 8)
		{
		case 0: ins->load.filename = (uint16_t) "assets\\bkg1.dat"; break;
		case 1: ins->load.filename = (uint16_t) "assets\\bkg2.dat"; break;
		case 2: ins->load.filename = (uint16_t) "assets\\bkg3.dat"; break;
		case 3: ins->load.filename = (uint16_t) "assets\\bkg4.dat"; break;
		case 4: ins->load.filename = (uint16_t) "assets\\bkg5.dat"; break;
		case 5: ins->load.filename = (uint16_t) "assets\\bkg6.dat"; break;
		case 6: ins->load.filename = (uint16_t) "assets\\bkg7.dat"; break;
		case 7: ins->load.filename = (uint16_t) "assets\\bkg8.dat";
		}

		NewInstruction(CODE_DRAW_BKG);
		goto no_clean; /* Because draw an entry background
		                  left us with the screen clean */
	}

	/* Draw step */
	{
		/* Clean space that characters occupy */
		ins = NewInstruction(CODE_DRAW_RECTANGLE_BKG);
		ins->draw.x = 0;
		ins->draw.y = 56; /* Minimum value in 's_base_y' in
		                     relation with following 'height' */

		ins->draw.width = 20;
		ins->draw.height = 9; /* 144 px */

		/* Clean the time metter */
		ins = NewInstruction(CODE_DRAW_RECTANGLE);
		ins->draw.color = 16;
		ins->draw.x = 3;
		ins->draw.y = 3;
		ins->draw.width = 4;  /* 64 px */
		ins->draw.height = 1; /* 16 px */

	no_clean:

		for (i = 0; i < ACTORS_NUMBER; i++)
		{
			if (s_actor[i].type == TYPE_DEAD)
				continue;

			/* Draw character */
			ins = NewInstruction(CODE_DRAW_RECTANGLE);
			ins->draw.color = ((i < 2) ? 36 : 58) + i;
			ins->draw.x = s_actor[i].x;
			ins->draw.y = s_base_y[i];
			ins->draw.width = 4;  /* 64 px */
			ins->draw.height = 6; /* 96 px */

			/* Draw time metter */
			ins = NewInstruction(CODE_DRAW_PIXEL);
			ins->draw.color = ((i < 2) ? 36 : 58) + i;
			ins->draw.x = 3 + (s_actor[i].common_time >> 2);
			ins->draw.y = 3 + i + i;
		}
	}

	/* Bye! */
	NewInstruction(CODE_HALT);
	s_instructions_counter = 0;

	return 0;
}


static uint16_t Random()
{
	/* http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html */
	s_marsaglia ^= s_marsaglia << 7;
	s_marsaglia ^= s_marsaglia >> 9;
	s_marsaglia ^= s_marsaglia << 8;
	return s_marsaglia;
}


static int8_t Sin(uint8_t a)
{
	int8_t r = s_sin_table[a % 128];
	return (a > 128) ? -r : r;
}


static union Instruction* NewInstruction(uint8_t code)
{
	union Instruction* i = (union Instruction*)(INSTRUCTIONS_TABLE_OFFSET) + s_instructions_counter;
	s_instructions_counter += 1;
	i->code = code;
	return i;
}


static void PrintString(uint16_t string)
{
	uint16_t* a1 = (uint16_t*)INT_FD_ARG1;
	uint16_t* a2 = (uint16_t*)INT_FD_ARG2;
	*a1 = 0x01;
	*a2 = string;
	asm("int 0xFD");
}


static void PrintNumber(uint16_t number)
{
	uint16_t* a1 = (uint16_t*)INT_FD_ARG1;
	uint16_t* a2 = (uint16_t*)INT_FD_ARG2;
	*a1 = 0x02;
	*a2 = number;
	asm("int 0xFD");
}
