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
	com->draw_text.slot = 1;
	com->draw_text.text = (uint16_t) "Sakurai, alpha build";
#endif
}


static void sResetActors()
{
	/* TODO: is not the idea to have entirely random enemies */
	uint8_t i = 0;

	for (i = 0; i < ACTORS_NO; i++)
	{
		/* Only enemies resets they health and type */
		if (i >= HEROES_NO)
		{
			do
				actor[i].type = (Random() % TYPES_NO);
			while (actor[i].type < HEROES_NO);

			actor[i].health = persona[actor[i].type].health;

			if (actor[i].health <= 90)
				actor[i].health += (uint8_t)(Random() % 10);
		}

		actor[i].phase = (uint8_t)Random();
		actor[i].x = info[i].base_x;

		/* The rest just need to be zero */
		actor[i].target = 0;
		actor[i].attack_type = 0;
		actor[i].state = 0;
		actor[i].next_state = 0;
		actor[i].idle_time = 0;
		actor[i].charge_time = 0;
		actor[i].bounded_time = 0;
	}
}


static void sDrawActors()
{
	union Command* com;
	uint8_t i = 0;

	for (i = 0; i < ACTORS_NO; i++)
	{
		com = NewCommand(CODE_DRAW_SPRITE);
		com->draw_sprite.x = actor[i].x;
		com->draw_sprite.y = info[i].base_y;

		if (i >= HEROES_NO)
			com->draw_sprite.slot = 5;
		else
			com->draw_sprite.slot = 6;
	}
}


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

	/* Update logic */
	for (i = 0; i < ACTORS_NO; i++) {}

	/* Draw */
	sDrawActors();

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return FieldState;
}


void* GameStart()
{
	LoadSprite("assets\\font1.jvn", 1);
	LoadSprite("assets\\font2.jvn", 2);

	LoadSprite("assets\\sprite1.jvn", 4);
	LoadSprite("assets\\sprite2.jvn", 5);
	LoadSprite("assets\\idle.jvn", 6);

	sResetActors();
	sResetBackground();

	FieldState();
	return FieldState;
}
