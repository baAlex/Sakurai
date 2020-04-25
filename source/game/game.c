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
#include "game-private.h"
#include "utilities.h"

#define DEVELOPER


/*-----------------------------

 sResetBackground()
 - Loads a new random background and draws it
 - Draws a legend on the top ("alpha build")
 - Draws part of the ui
-----------------------------*/
static void sResetBackground()
{
	union Command* com;

	switch (Random() % 8)
	{
	case 0: LoadBackground((uint16_t) "assets\\bkg1.raw"); break;
	case 1: LoadBackground((uint16_t) "assets\\bkg2.raw"); break;
	case 2: LoadBackground((uint16_t) "assets\\bkg3.raw"); break;
	case 3: LoadBackground((uint16_t) "assets\\bkg4.raw"); break;
	case 4: LoadBackground((uint16_t) "assets\\bkg8.raw"); break;
	case 5: LoadBackground((uint16_t) "assets\\bkg7.raw"); break;
	case 6: LoadBackground((uint16_t) "assets\\bkg6.raw"); break;
	case 7: LoadBackground((uint16_t) "assets\\bkg5.raw");
	}

	NewCommand(CODE_DRAW_BKG);

#ifdef DEVELOPER
	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = 1;
	com->draw_text.y = 0;
	com->draw_text.slot = 31;
	com->draw_text.text = (uint16_t) "Sakurai, alpha build";
#endif
}


/*-----------------------------

 sResetActors()
 - Iterate the actors array and reset they values
 - Enemies are chosen and set in a random fashion
 - Heroes reset all values except for health and type
 - Finally, unloads all sprites in the engine to then
   load only the necessary ones (TODO)
-----------------------------*/
static void sResetActors()
{
	/* TODO: is not the idea to have entirely random enemies */
	uint8_t loaded[TYPES_NO];
	uint8_t i = 0;

	for (i = 0; i < TYPES_NO; i++)
		loaded[i] = 0; /* TODO, i need a malloc()!! */

	UnloadEverything();
	LoadSprite("assets\\font1.jvn", 31); /* TODO, not everything :/ (good case to use pool_b) */

	for (i = 0; i < ACTORS_NO; i++)
	{
		/* Only enemies resets they health and type */
		if (i >= HEROES_NO)
		{
			do
				actor[i].type = (Random() % TYPES_NO);
			while (actor[i].type < HEROES_NO);

			actor[i].health = persona[actor[i].type].health;

			/* A random plus of health, 10 pts */
			if (actor[i].health <= 90)
				actor[i].health += (uint8_t)(Random() % 10);
		}

		actor[i].phase = (uint8_t)Random();
		actor[i].state = STATE_IDLE;

		/* The rest just need to be zero */
		actor[i].target = 0;
		actor[i].attack_type = 0;
		actor[i].idle_time = 0;
		actor[i].charge_time = 0;
		actor[i].bounded_time = 0;

		/* Load sprites, whitout repeat */
		if (loaded[actor[i].type] == 0)
		{
			if (actor[i].type == TYPE_HERO_A)
				LoadSprite("assets\\player.jvn", actor[i].type);
			else if (actor[i].type == TYPE_HERO_B)
				LoadSprite("assets\\player.jvn", actor[i].type);
			else if (actor[i].type == TYPE_A)
				LoadSprite("assets\\type-a.jvn", actor[i].type);
			else if (actor[i].type == TYPE_B)
				LoadSprite("assets\\type-b.jvn", actor[i].type);
			else if (actor[i].type == TYPE_C)
				LoadSprite("assets\\type-c.jvn", actor[i].type);
			else if (actor[i].type == TYPE_D)
				LoadSprite("assets\\type-d.jvn", actor[i].type);
			else if (actor[i].type == TYPE_E)
				LoadSprite("assets\\type-e.jvn", actor[i].type);
			else if (actor[i].type == TYPE_F)
				LoadSprite("assets\\type-f.jvn", actor[i].type);
			else if (actor[i].type == TYPE_G)
				LoadSprite("assets\\type-g.jvn", actor[i].type);

			loaded[actor[i].type] = 1;
		}
	}
}


/*-----------------------------

 sDrawActors()
 - As the name says, plus a time meter bar on top of them
-----------------------------*/
static void sDrawActors(uint8_t clean_area)
{
	union Command* com;
	uint8_t i = 0;
	uint8_t color = 0;
	uint8_t width = 0;

	/* Clean area */
	if (clean_area == 1)
	{
		com = NewCommand(CODE_DRAW_RECTANGLE_BKG);
		com->draw_shape.x = 0;
		com->draw_shape.y = 60;
		com->draw_shape.width = 20; /* 320 px */
		com->draw_shape.height = 9; /* 144 px */
	}

	/* Iterate actors */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (actor[i].state == STATE_DEAD)
			continue;

		/* Sprite */
		com = NewCommand(CODE_DRAW_SPRITE);
		com->draw_sprite.x = info[i].base_x;
		com->draw_sprite.y = info[i].base_y;
		com->draw_sprite.slot = actor[i].type;

		if (actor[i].state == STATE_CHARGE)
			com->draw_sprite.x += Sin(actor[i].phase) >> 5;

		/* Time meter background */
		com = NewCommand(CODE_DRAW_RECTANGLE_PRECISE);
		com->draw_shape.color = 16;
		com->draw_shape.x = info[i].base_x;
		com->draw_shape.y = info[i].base_y;
		com->draw_shape.width = 34;
		com->draw_shape.height = 3;

		/* Time meter */
		if (actor[i].state == STATE_IDLE)
		{
			color = 8;
			width = (actor[i].idle_time >> 3); /* Max of 32 px */
		}
		else
		{
			color = 41;
			width = (actor[i].charge_time >> 3);
		}

		if (width == 0) /* Too tiny to draw it */
			continue;

		com = NewCommand(CODE_DRAW_RECTANGLE_PRECISE);
		com->draw_shape.color = color;
		com->draw_shape.width = width; /* Max of 32 px */
		com->draw_shape.height = 1;
		com->draw_shape.x = info[i].base_x + 1;
		com->draw_shape.y = info[i].base_y + 1;
	}
}


static void sActorIdle(struct Actor* actor)
{
	/* Update idle timer */
	if (actor->idle_time < (255 - persona[actor->type].idle_time))
	{
		if ((CURRENT_FRAME % 2) == 0)
			actor->idle_time += persona[actor->type].idle_time;
	}

	/* Our turn!, time to change into 'charge' state */
	else
	{
		actor->charge_time = 0;
		actor->state = STATE_CHARGE;

		/* Select our attack */
		actor->attack_type = Random() % UINT8_MAX;

		/* Select our target... */
		{
		again:
			/* As an enemy */
			if (actor->type >= HEROES_NO)
				actor->target = HEROES_NO + (Random() % (ACTORS_NO - HEROES_NO));

			/* As an hero */
			else
				actor->target = Random() % HEROES_NO;

			/* Wait, the target is alive? */
			if (actor[actor->target].state == STATE_DEAD)
				goto again;
		}
	}
}


static void sActorCharge(struct Actor* actor)
{
	/* Oscillate in position */
	actor->phase += 20;

	/* Update charge timer */
	if (actor->charge_time < (255 - 8))
		actor->charge_time += 8;

	/* Attack!!! (TODO) */
	else
	{
		actor->idle_time = 0;
		actor->state = STATE_IDLE;
	}
}


/*===========================*/


void* MenuState()
{
	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return MenuState;
}


void* FieldState()
{
	uint8_t i = 0;

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (actor[i].state == STATE_DEAD)
			continue;

		else if (actor[i].state == STATE_IDLE)
			sActorIdle(&actor[i]);
		else if (actor[i].state == STATE_CHARGE)
			sActorCharge(&actor[i]);
	}

	if (INPUT_X != 0)
	{
		sResetActors();
		sResetBackground();
		sDrawActors(0);

		INPUT_X = 0;
	}
	else
		sDrawActors(1);

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return FieldState;
}


void* GameStart()
{
	uint8_t i = 0;

	/* TODO, there is no seed() function */
	for (i = 0; i < 48; i++)
		Random();

	LoadSprite("assets\\font1.jvn", 31);

	sResetActors();
	sResetBackground();

	FieldState();
	return FieldState;
}
