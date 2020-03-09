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


/* Clean actors from screen once with a big
   rectangle or twice with two small ones?

   Also, as I'm actually cleaning half the screen
   rather that use rectangles, an instruction some
   like 'DRAW_SCANLINE_BKG' (cleaning after an
   specified scanline) is going to be a lot better */
#define ONE_CLEAN_RECTANGLE


int main()
{
	union Instruction* ins;
	uint8_t i;

	uint16_t heroes_min_x = 999;
	uint16_t heroes_max_x = 0;
#ifndef ONE_CLEAN_RECTANGLE
	uint16_t enemies_min_x = 999;
	uint16_t enemies_max_x = 0;
#endif

	/* Iterate actors, update them */
	for (i = 0; i < ACTORS_NUMBER; i++)
	{
		s_actor[i].previous_x = s_actor[i].x;
		s_actor[i].previous_frame = s_actor[i].frame;

		s_actor[i].x = s_base_x[i] + (Sin(s_frame + s_frame + s_phase[i]) >> 5);

		/* Save previous position in min/max form to
		   alow the below 'draw step' clean the screen */

#ifndef ONE_CLEAN_RECTANGLE
		if (i >= 2)
		{
			enemies_min_x = MIN(enemies_min_x, s_actor[i].previous_x);
			enemies_max_x = MAX(enemies_max_x, s_actor[i].previous_x);
		}
		else
#endif
		{
			heroes_min_x = MIN(heroes_min_x, s_actor[i].previous_x);
			heroes_max_x = MAX(heroes_max_x, s_actor[i].previous_x);
		}
	}

	/* Change the background every 10 seconds,
	   is just to test this functionality */
	if ((s_frame % 240) == 0)
	{
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

		/* Draw two portraits in the top-left corner,
		   this needs to be done every time that the entry
		   screens changes (in this case the background) */
		ins = NewInstruction(CODE_DRAW_RECTANGLE);
		ins->draw.color = 10;
		ins->draw.x = 2;
		ins->draw.y = 2;
		ins->draw.width = 3;
		ins->draw.height = 3;

		ins = NewInstruction(CODE_DRAW_RECTANGLE);
		ins->draw.color = 10;
		ins->draw.x = 52;
		ins->draw.y = 2;
		ins->draw.width = 3;
		ins->draw.height = 3;

		goto no_clean; /* Because draw an entry background
		                  left us with the screen clean */
	}

	/* Draw step */
	{
		/* Before draw we need to clean the space that actors occupy */
		ins = NewInstruction(CODE_DRAW_RECTANGLE_BKG);
		ins->draw.x = heroes_min_x;
		ins->draw.y = 56; /* Minimum value in 's_base_y' in
		                     relation with following 'height' */

		ins->draw.width = (heroes_max_x + 64 + 16 - heroes_min_x) >> 4;
		ins->draw.height = 9; /* 144 px */

#ifndef ONE_CLEAN_RECTANGLE
		ins = NewInstruction(CODE_DRAW_RECTANGLE_BKG);
		ins->draw.x = enemies_min_x;
		ins->draw.y = 56;
		ins->draw.width = (enemies_max_x + 64 + 16 - enemies_min_x) >> 4;
		ins->draw.height = 9;
#endif

	no_clean:

		/* Draw! */
		for (i = 0; i < ACTORS_NUMBER; i++)
		{
			ins = NewInstruction(CODE_DRAW_RECTANGLE);
			ins->draw.color = 30 + i;
			ins->draw.x = s_actor[i].x;
			ins->draw.y = s_base_y[i];
			ins->draw.width = 4;  /* 64 px */
			ins->draw.height = 6; /* 96 px */
		}
	}

	/* Bye! */
	NewInstruction(CODE_HALT);
	s_instructions_counter = 0;
	s_frame++;

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
	union Instruction* i = (union Instruction*)(INSTRUCTIONS_TABLE_START) + s_instructions_counter;
	s_instructions_counter += 1;
	i->code = code;
	return i;
}
