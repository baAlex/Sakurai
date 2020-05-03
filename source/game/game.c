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
#include "actor.h"
#include "utilities.h"


#define DEV


uint8_t Layout(uint8_t battle_no, uint8_t* out);

void* AnimationState();
void* FieldState();

static uint8_t s_battle_no = 0;
static uint8_t s_battle_layout[ENEMIES_NO] = {0, 0, 0, 0};


/*===========================*/


static uint8_t AnimationState_frame = 0;
static uint8_t AnimationState_actor = 0;

void* AnimationState()
{
	union Command* com = NULL;
	uint8_t target = g_actor[AnimationState_actor].target;

	/* Draw actors normally */
	DrawActors();

	/* Draw some fancy animation */
	com = NewCommand(CODE_DRAW_SPRITE);
	com->draw_sprite.x = g_info[target].base_x;
	com->draw_sprite.y = g_info[target].base_y;
	com->draw_sprite.frame = AnimationState_frame;

	if (AnimationState_actor < HEROES_NO)
		com->draw_sprite.slot = 24;
	else
		com->draw_sprite.slot = 25;

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	AnimationState_frame += 1;

	if (AnimationState_frame >= 6)
		return FieldState;

	return AnimationState;
}


/*===========================*/


void* FieldState()
{
	void* next_state = FieldState;
	uint8_t i = 0;

	/* Developers, Developers, Developers */
	if (INPUT_X == 1)
	{
		if (s_battle_no > 0)
		{
			s_battle_no -= 1;
			next_state = GameStart;
			goto bye;
		}
	}

	if (INPUT_Y == 1)
	{
		if (s_battle_no < UINT8_MAX)
			s_battle_no += 1;

		next_state = GameStart;
		goto bye;
	}

	/* Logic step */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		else if (g_actor[i].state == ACTOR_STATE_ATTACK)
		{
			ActorAttack(i);

			AnimationState_actor = i;
			AnimationState_frame = 0;
			next_state = AnimationState;
			goto bye;
		}
		else if (g_actor[i].state == ACTOR_STATE_CHARGE)
			ActorCharge(i);
		else
			ActorIdle(i);
	}

	/* Draw step */
	DrawActors();

bye:
	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return next_state;
}


/*===========================*/


static uint8_t GameLoad_frame = 0;
static uint8_t GameLoad_initialized[TYPES_NO] = {0, 0, 0, 0, 0, 0, 0, 0, 0};


void* GameLoad()
{
	/* TODO: in the future, rather than count frames is
	going to be better to measure miliseconds
	(currently the engine didn't share them) */

	union Command* com;
	uint8_t i = 0;

	if (GameLoad_frame != 0)
		goto bye;

	for (i = 0; i < TYPES_NO; i++)
		GameLoad_initialized[i] = 0; /* TODO, I need a malloc()!! */

	/* Load remaining resources */
	LoadSprite("assets\\port-a.jvn", 22);
	LoadSprite("assets\\port-b.jvn", 23);
	LoadSprite("assets\\fx1.jvn", 24);
	LoadSprite("assets\\fx2.jvn", 25);

	/* Load sprites, whitout repeat */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (GameLoad_initialized[g_actor[i].type] == 0)
		{
			if (g_actor[i].type == TYPE_HERO_A)
				LoadSprite("assets\\player.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_HERO_B)
				LoadSprite("assets\\player.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_A)
				LoadSprite("assets\\type-a.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_B)
				LoadSprite("assets\\type-b.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_C)
				LoadSprite("assets\\type-c.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_D)
				LoadSprite("assets\\type-d.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_E)
				LoadSprite("assets\\type-e.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_F)
				LoadSprite("assets\\type-f.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_G)
				LoadSprite("assets\\type-g.jvn", g_actor[i].type);

			GameLoad_initialized[g_actor[i].type] = 1;
		}
	}

bye:
	/* Bye! */
	GameLoad_frame += 1;

	/* Before change into FieldState state we need to
	display the loading message for 1,5 seconds */
#ifdef DEV
	if (GameLoad_frame >= 0)
#else
	if (GameLoad_frame >= 36)
#endif
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
		PrintNumber(s_battle_no);
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

	/* Load indispensable resources (TODO!) */
	UnloadEverything(); /* TODO, not everything! */

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

	if (Layout(s_battle_no, s_battle_layout) != 1)
		com->draw_text.text = (uint16_t) "Monsters appear!";
	else
		com->draw_text.text = (uint16_t) "Monster appears!";

	text_x = com->draw_text.x;

	/* Initialize actors */
	for (i = 0; i < ACTORS_NO; i++)
	{
		InitializeActor(i, (s_battle_no < 32) ? s_battle_layout : (uint8_t*)NULL);

		/* Print enemy name */
		if (i < HEROES_NO)
			continue;

		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = text_x;
		com->draw_text.y = 100 - 16 + 12;
		com->draw_text.slot = 20;
		com->draw_text.text = (uint16_t)g_persona[g_actor[i].type].name;

		text_x += 76; /* (320 - 8 - 8) / 4 */
	}

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	PrintString("# GameStart() ends: ");
	PrintNumber(s_battle_no);

	GameLoad_frame = 0;
	return GameLoad;
}
