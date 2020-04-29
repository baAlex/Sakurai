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


unsigned char Layout(unsigned char i, unsigned char battle_no);

static uint8_t battle_no = 0;
static uint8_t battle_layout[ENEMIES_NO] = {0, 0, 0, 0};


static uint8_t initialized_types[TYPES_NO] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t load_screen_frame = 0;


static struct Actor actor[ACTORS_NO] =
{
	{0, 0, 0, 0, 0, 0, 0, 100, TYPE_HERO_A},
	{0, 0, 0, 0, 0, 0, 0, 100, TYPE_HERO_B},

	{0, 0, 0, 0, 0, 0, 0, 100, __NN__},
	{0, 0, 0, 0, 0, 0, 0, 100, __NN__},
	{0, 0, 0, 0, 0, 0, 0, 100, __NN__},
	{0, 0, 0, 0, 0, 0, 0, 100, __NN__}
};


static struct Information info[ACTORS_NO] =
{
	{/* BaseX */ 38, /* BaseY */ 60},
	{/* BaseX */ 8,  /* BaseY */ 100},

	{/* BaseX */ 150, /* BaseY */ 60},
	{/* BaseX */ 180, /* BaseY */ 73},
	{/* BaseX */ 220, /* BaseY */ 86},
	{/* BaseX */ 250, /* BaseY */ 100},

};


static struct Personality persona[TYPES_NO] =
{
	{"Sayori", /* Idle */ 5, /* Health */ 100},
	{"Kuro",   /* Idle */ 4, /* Health */ 100},

	{"Ferment",   /* Idle */ 1, /* Health */ 40},
	{"Wind Eye",  /* Idle */ 1, /* Health */ 50}, /* Phantasy Star */
	{"Kingpin",   /* Idle */ 2, /* Health */ 60}, /* Half-Life */
	{"Destroyer", /* Idle */ 3, /* Health */ 70},
	{"Phibia",    /* Idle */ 4, /* Health */ 80},
	{"Viridi",    /* Idle */ 5, /* Health */ 90},
	{"Ni",        /* Idle */ 6, /* Health */ 100}

	/* TODO: think better names
	- "Ferment", ok
	- "Destroyer", nope
	- "Phibia", a dragon is an amphibian?
	- "Viridi", mehhh... (viridiplantae)
	- "Ni", lovely */
};


/*-----------------------------

 sDrawActors()
 - As the name says, plus a time meter bar on top of them
-----------------------------*/
static void sDrawActors()
{
	union Command* com;
	uint8_t i = 0;
	uint8_t color = 0;
	uint8_t width = 0;

	/* Clean area */
	com = NewCommand(CODE_DRAW_RECTANGLE_BKG);
	com->draw_shape.x = 0;
	com->draw_shape.y = 60;
	com->draw_shape.width = 20; /* 320 px */
	com->draw_shape.height = 9; /* 144 px */

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


void* FieldState()
{
	void* next_state = FieldState;
	uint8_t i = 0;

	if (INPUT_X == 1)
	{
		if(battle_no > 0)
		{
			battle_no -= 1;
			next_state = GameStart;
			goto bye; /*Hack! */
		}
	}

	if (INPUT_Y == 1)
	{
		if(battle_no < UINT8_MAX)
			battle_no += 1;

		next_state = GameStart;
		goto bye; /*Hack! */
	}

	/* Logic step */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (actor[i].state == STATE_DEAD)
			continue;

		else if (actor[i].state == STATE_IDLE)
			sActorIdle(&actor[i]);
		else if (actor[i].state == STATE_CHARGE)
			sActorCharge(&actor[i]);
	}

	/* Draw step */
	sDrawActors();

bye:
	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return next_state;
}


/*===========================*/


static void sInitializeActor(uint8_t i)
{
	/* Only enemies resets they health and type */
	if (i >= HEROES_NO)
	{
		if (battle_no < 32) /* HARDCODED! */
		{
			actor[i].type = Layout(i - HEROES_NO, battle_no);
		}
		else
		{
			do
				actor[i].type = (Random() % TYPES_NO);
			while (actor[i].type < HEROES_NO);
		}

		PrintString("# Enemy: ");
		PrintNumber(actor[i].type);

		if (actor[i].type == __NN__)
		{
			actor[i].type = (TYPES_NO - 1);
			actor[i].state = STATE_DEAD;
			return;
		}
		else
		{
			actor[i].health = persona[actor[i].type].health;

			/* A random plus of health, 10 pts */
			if (actor[i].health <= 90)
				actor[i].health += (uint8_t)(Random() % 10);
		}
	}


	actor[i].phase = (uint8_t)Random();
	actor[i].state = STATE_IDLE;

	/* The rest just need to be zero */
	actor[i].target = 0;
	actor[i].attack_type = 0;
	actor[i].idle_time = 0;
	actor[i].charge_time = 0;
	actor[i].bounded_time = 0;
}


void* GameLoad()
{
	/* TODO: in the future, rather than count frames is
	going to be better to measure elapsed miliseconds
	(currently the engine didn't share them) */

	union Command* com;
	uint8_t i = 0;

	if (load_screen_frame != 0)
		goto bye;

	for (i = 0; i < TYPES_NO; i++)
		initialized_types[i] = 0; /* TODO, i need a malloc()!! */

	/* Load heroes portraits */
	LoadSprite("assets\\port-a.jvn", 22);
	LoadSprite("assets\\port-b.jvn", 23);

	/* Load sprites, whitout repeat */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (initialized_types[actor[i].type] == 0)
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

			initialized_types[actor[i].type] = 1;
		}
	}

bye:
	/* Bye! */
	load_screen_frame += 1;

	/* Before change into 'field' state we need to
	display the loading message for 1:30 seconds */
	if (load_screen_frame >= 36)
	{
		NewCommand(CODE_DRAW_BKG);

		/* Draw heroes portrait */
		com = NewCommand(CODE_DRAW_SPRITE);
		com->draw_sprite.x = 2;
		com->draw_sprite.y = 2;
		com->draw_sprite.slot = 22;

		com = NewCommand(CODE_DRAW_SPRITE);
		com->draw_sprite.x = 66;
		com->draw_sprite.y = 2;
		com->draw_sprite.slot = 23;

		NewCommand(CODE_HALT);
		CleanCommands();

		PrintString("# GameLoad() ends: ");
		PrintNumber(battle_no);
		return FieldState;
	}

	NewCommand(CODE_HALT);
	CleanCommands();
	return GameLoad;
}


void* GameStart()
{
	union Command* com;
	uint8_t i = 0;
	uint16_t text_x = 0;

	UnloadEverything(); /* TODO, not everything! */

	/* Load indispensable resources (TODO!) */
	LoadSprite("assets\\font1.jvn", 20);
	LoadSprite("assets\\font2.jvn", 21);

	/* Load an random background and draw it */
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

	/* Print "Enemies appear!" */
	com = NewCommand(CODE_DRAW_RECTANGLE);
	com->draw_shape.color = 15;
	com->draw_shape.x = 0;
	com->draw_shape.y = 100 - 16;
	com->draw_shape.width = 20; /* 320 px */
	com->draw_shape.height = 2; /* 32 px */

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = 8;
	com->draw_text.y = 100 - 16;
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "Enemies appear!";

	text_x = com->draw_text.x;

	/* Initialize actors */
	for (i = 0; i < ACTORS_NO; i++)
	{
		sInitializeActor(i);

		/* Print enemy name */
		if (i < HEROES_NO)
			continue;

		if (actor[i].state == STATE_DEAD)
			continue;

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = text_x;
		com->draw_text.y = 100 - 16 + 12;
		com->draw_text.slot = 20;
		com->draw_text.text = (uint16_t)persona[actor[i].type].name;

		text_x += 76; /* (320 - 8 - 8) / 4 */
	}

	/* Bye! */
	load_screen_frame = 0;

	NewCommand(CODE_HALT);
	CleanCommands();

	PrintString("# GameStart() ends: ");
	PrintNumber(battle_no);
	return GameLoad;
}
