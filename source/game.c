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

#include "shared.h"


static uint16_t* s_frame = (uint16_t*)FRAME_COUNTER_OFFSET;


/* BCC is somewhat stupid initializing static variables */
static struct Actor s_actor[ACTORS_NUMBER] = {
    /* Our heroes */
    {0, 0, 0, 0, 0, TYPE_HERO_A, 100, 238},
    {0, 0, 0, 0, 0, TYPE_HERO_B, 100, 19},
    /* Enemies */
    {0, 0, 0, 0, 0, TYPE_A, 100, 36},
    {0, 0, 0, 0, 0, TYPE_B, 100, 75},
    {0, 0, 0, 0, 0, TYPE_C, 100, 132},
    {0, 0, 0, 0, 0, TYPE_D, 100, 24}};

static uint16_t s_base_x[ACTORS_NUMBER] = {
    /* Our heroes */
    38, 8,
    /* Enemies */
    180, 202, 225, 248};

static uint8_t s_base_y[ACTORS_NUMBER] = {
    /* Our heroes */
    60, 100,
    /* Enemies */
    60, 73, 86, 100};

static uint8_t s_idle_time[TYPES_NUMBER] = {
    /* Our heroes */
    6, 2,

    1, /* Type A */
    2, /* Type B */
    3, /* Type C */
    4, /* Type D */
    5, /* Type E */
    6  /* Type F */
};


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
		}
	}

	/* Change the background every 10 seconds,
	   is just to test this functionality */
	if ((*(uint16_t*)FRAME_COUNTER_OFFSET % 240) == 0)
	{
		/* Print something */
		PrintString((uint16_t)"Something\n");
		PrintNumber(0x1234);

		/* Load and draw it */
		ins = NewInstruction(CODE_LOAD_BKG);

		switch (Random() % 8)
		{
		case 0: ins->load.filename = (uint16_t) "assets\\bkg1.raw"; break;
		case 1: ins->load.filename = (uint16_t) "assets\\bkg2.raw"; break;
		case 2: ins->load.filename = (uint16_t) "assets\\bkg3.raw"; break;
		case 3: ins->load.filename = (uint16_t) "assets\\bkg4.raw"; break;
		case 4: ins->load.filename = (uint16_t) "assets\\bkg5.raw"; break;
		case 5: ins->load.filename = (uint16_t) "assets\\bkg6.raw"; break;
		case 6: ins->load.filename = (uint16_t) "assets\\bkg7.raw"; break;
		case 7: ins->load.filename = (uint16_t) "assets\\bkg8.raw";
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
	CleanInstructions();

	return 0;
}
