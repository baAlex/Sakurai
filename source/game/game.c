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
    {0, 0, 0, 0, 0, 0, 0, 0, TYPE_HERO_A, 100, 238},
    {0, 0, 0, 0, 0, 0, 0, 0, TYPE_HERO_B, 100, 19},
    /* Enemies */
    {0, 0, 0, 0, 0, 0, 0, 0, TYPE_A, 100, 36},
    {0, 0, 0, 0, 0, 0, 0, 0, TYPE_B, 100, 75},
    {0, 0, 0, 0, 0, 0, 0, 0, TYPE_C, 100, 132},
    {0, 0, 0, 0, 0, 0, 0, 0, TYPE_D, 100, 24}};

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
    5, 1,

    1, /* Type A */
    1, /* Type B */
    2, /* Type C */
    3, /* Type D */
    4, /* Type E */
    5  /* Type F */
};


int main()
{
	union Command* com;
	uint8_t i;
	uint8_t color;
	uint8_t width;

	uint16_t clean_min_x = UINT16_MAX;
	uint16_t clean_max_x = 0;

	if (s_actor[0].type == TYPE_DEAD && s_actor[1].type == TYPE_DEAD)
	{
		NewCommand(CODE_HALT);
		CleanCommands();
		return; /* Game over */
	}

	/* Iterate actors, update them */
	for (i = 0; i < ACTORS_NUMBER; i++)
	{
		/* Save position that comes from previous frame
		   in min/max form to allow the below 'draw step'
		   clean the screen */
		clean_min_x = MIN(clean_min_x, s_actor[i].x);
		clean_max_x = MAX(clean_max_x, s_actor[i].x);

		/* Update state */
		s_actor[i].state = s_actor[i].next_state;
		s_actor[i].x = s_base_x[i] + (Sin(s_actor[i].phase) >> 5);

		/* Nothing, we ded' */
		if (s_actor[i].type == TYPE_DEAD)
			continue;

		/* Idle state or "wait some time for my turn" */
		if (s_actor[i].state == STATE_IDLE)
		{
			/* Update counter */
			if (s_actor[i].idle_time < (255 - s_idle_time[s_actor[i].type]))
			{
				if ((*(uint16_t*)FRAME_COUNTER_OFFSET % 2) == 0)
					s_actor[i].idle_time += s_idle_time[s_actor[i].type];
			}
			else
			{
				/* Change to 'charge' state */
				s_actor[i].idle_time = 255;
				s_actor[i].charge_time = 0;
				s_actor[i].next_state = STATE_CHARGE;

				/* Select our attack */
				s_actor[i].attack_type = Random() % UINT8_MAX;

			again:
				/* We are heroes, so lets chose an enemy */
				if (i < 2)
					s_actor[i].target = 2 + (Random() % (ACTORS_NUMBER - 2));

				/* The same, but from the other side */
				else
					s_actor[i].target = Random() % 2;

				/* Wait, our target is live? */
				if (s_actor[s_actor[i].target].type == TYPE_DEAD)
					goto again;
			}
		}

		/* Bounded state or "damage received, let me rest a bit" */
		else if (s_actor[i].state == STATE_BOUNDED)
		{
			/* Update counter */
			if (s_actor[i].bounded_time != 0)
				s_actor[i].bounded_time -= 1;
			else
			{
				/* Restore previous state */
				if (s_actor[i].idle_time != 255)
					s_actor[i].next_state = STATE_IDLE;
				else
					s_actor[i].next_state = STATE_CHARGE;
			}
		}

		/* Charge state or "this weapon is too heavy, game me a sec" */
		else if (s_actor[i].state == STATE_CHARGE)
		{
			/* Oscillate in position */
			s_actor[i].phase += 20;

			/* Update counter */
			if (s_actor[i].charge_time < (255 - 8))
				s_actor[i].charge_time += 8;
			else
			{
				/* Change to 'attack' state */
				s_actor[i].charge_time = 255;
				s_actor[i].next_state = STATE_ATTACK;
			}
		}

		/* Attack state or "ey! look this cool animation" */
		else if (s_actor[i].state == STATE_ATTACK)
		{
			/* Change to 'idle' state */
			s_actor[i].idle_time = 0;
			s_actor[i].next_state = STATE_IDLE;

			/* Inflict damage in our victim */
			s_actor[s_actor[i].target].health -= 5;

			/* Change they state */
			s_actor[s_actor[i].target].next_state = STATE_BOUNDED;
			s_actor[s_actor[i].target].bounded_time = 24; /* One second */

			/* Set if victim died */
			if (s_actor[s_actor[i].target].health <= 0)
				s_actor[s_actor[i].target].type = TYPE_DEAD;
		}
	}

	/* Change the background every 10 seconds,
	   is just to test this functionality */
	if ((*(uint16_t*)FRAME_COUNTER_OFFSET % 240) == 0)
	{
		if(*(uint16_t*)FRAME_COUNTER_OFFSET == 0)
		{
			LoadSprite("assets\\idle.jvn", 0);
			LoadSprite("assets\\sprite1.jvn", 1);
			LoadSprite("assets\\sprite2.jvn", 2);
		}

		switch (Random() % 8)
		{
		case 0: LoadBackground((uint16_t) "assets\\bkg1.raw"); break;
		case 1: LoadBackground((uint16_t) "assets\\bkg2.raw"); break;
		case 2: LoadBackground((uint16_t) "assets\\bkg3.raw"); break;
		case 3: LoadBackground((uint16_t) "assets\\bkg4.raw"); break;
		case 4: LoadBackground((uint16_t) "assets\\bkg5.raw"); break;
		case 5: LoadBackground((uint16_t) "assets\\bkg6.raw"); break;
		case 6: LoadBackground((uint16_t) "assets\\bkg7.raw"); break;
		case 7: LoadBackground((uint16_t) "assets\\bkg8.raw");
		}

		NewCommand(CODE_DRAW_BKG);
		goto no_clean; /* Because draw an entry background
		                  left us with the screen clean */
	}

	/* Draw step */
	{
		/* Clean space that characters occupy */
		com = NewCommand(CODE_DRAW_RECTANGLE_BKG);
		com->draw_shape.x = 0;
		com->draw_shape.y = 56; /* Minimum value in 's_base_y' in
		                           relation with following 'height' */

		com->draw_shape.width = 20;
		com->draw_shape.height = 9; /* 144 px */

	no_clean:

		for (i = 0; i < ACTORS_NUMBER; i++)
		{
			if (s_actor[i].type == TYPE_DEAD)
				continue;

			/* Draw character */
			com = NewCommand(CODE_DRAW_SPRITE);
			com->draw_sprite.x = s_actor[i].x;
			com->draw_sprite.y = s_base_y[i];

			if (i < 2)
			{
				com->draw_sprite.slot = 0;
				com->draw_sprite.frame = *(uint16_t*)FRAME_COUNTER_OFFSET + i;
			}
			else
			{
				com->draw_sprite.slot = 2;
				com->draw_sprite.frame = 0;
			}

			/* Draw time meter background */
			com = NewCommand(CODE_DRAW_RECTANGLE_PRECISE);
			com->draw_shape.color = 16;
			com->draw_shape.x = s_actor[i].x;
			com->draw_shape.y = s_base_y[i];
			com->draw_shape.width = 34;
			com->draw_shape.height = 3;

			/* Draw time meter */
			switch (s_actor[i].state)
			{
			case STATE_IDLE:
				color = 8;
				width = (s_actor[i].idle_time >> 3);
				break;
			case STATE_CHARGE:
				color = 41;
				width = (s_actor[i].charge_time >> 3);
				break;
			default: break;
			}

			if (s_actor[i].state == STATE_BOUNDED)
			{
				color = 60;

				if (s_actor[i].idle_time != 255)
					width = (s_actor[i].idle_time >> 3);
				else
					width = (s_actor[i].charge_time >> 3);
			}

			if (width == 0)
				continue;

			com = NewCommand(CODE_DRAW_RECTANGLE_PRECISE);
			com->draw_shape.color = color;
			com->draw_shape.width = width; /* 32 px */
			com->draw_shape.height = 1;
			com->draw_shape.x = s_actor[i].x + 1;
			com->draw_shape.y = s_base_y[i] + 1;
		}
	}

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return 0;
}
