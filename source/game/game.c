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
#include "attacks.h"
#include "ui.h"
#include "utilities.h"

/*#define DEV*/


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
	AnimationState_frame += 1;

	if ((g_actor[AnimationState_actor].attack_type == ATTACK_SIMPLE && AnimationState_frame >= 6) ||
	    AnimationState_frame >= 24)
	{
		if (AnimationState_actor >= HEROES_NO)
			DrawHUD(22); /* To update damages */

		NewCommand(CODE_HALT);
		CleanCommands();
		return FieldState;
	}

	NewCommand(CODE_HALT);
	CleanCommands();
	return AnimationState;
}


/*===========================*/


static uint8_t UIState_actor = 0;
static uint16_t UIState_screen = 0;

static uint8_t UIState_action = 0;
static uint8_t UIState_target = 0;

#define SCREEN_ACTION 0
#define SCREEN_ACTION_DONE 1
#define SCREEN_TARGET 2
#define SCREEN_TARGET_DONE 3


void* UIState()
{
	void* next_state = UIState;
	union Command* com;

	/* Only draw static elements on the first frame */
	if (UIState_screen == SCREEN_ACTION)
	{
		DrawActionUI_static(22, UIState_actor);
		UIState_screen = SCREEN_ACTION_DONE;
	}
	else if (UIState_screen == SCREEN_TARGET)
	{
		DrawTargetUI_static(22);
		UIState_screen = SCREEN_TARGET_DONE;
	}

	/* Player press ENTER */
	if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1)
	{
		/* Return to the field */
		if (UIState_screen == SCREEN_TARGET_DONE)
		{
			g_actor[UIState_actor].target = UIState_target;

			CleanUI();
			DrawHUD(22);
			next_state = FieldState;
			goto bye;
		}

		/* Advance to the target screen, if necessary or possible*/
		{
			g_actor[UIState_actor].attack_type = UIState_action;

			if (UIState_action == ATTACK_SIMPLE)
			{
				g_actor[UIState_actor].charge_vel = g_persona[g_actor[UIState_actor].type].charge_vel;

				/* Advance to target screen */
				UIState_screen = SCREEN_TARGET;
				goto bye;
			}

			if (UIState_action == ATTACK_COMBINED && g_actor[UIState_actor].magic >= 20)
			{
				g_actor[UIState_actor].charge_vel = g_persona[g_actor[UIState_actor].type].charge_vel >> 1;

				/* Check magic first, then to the target screen */
				g_actor[UIState_actor].magic -= 20;
				UIState_screen = SCREEN_TARGET;
				goto bye;
			}

			if (UIState_action == ATTACK_HOLD)
			{
				g_actor[UIState_actor].charge_vel = 8;

				/* Skip directly to the field */
				CleanUI();
				DrawHUD(22);
				next_state = FieldState;
				goto bye;
			}

			/* Kuro */
			if (UIState_actor == 0)
			{
				if (UIState_action == ATTACK_HEAL || UIState_action == ATTACK_MEDITATE)
				{
					g_actor[0].charge_vel = 24;

					/* Skip directly to the field */
					CleanUI();
					DrawHUD(22);
					next_state = FieldState;
					goto bye;
				}
			}

			/* Sayori */
			else if (UIState_actor == 1)
			{
				if (UIState_action == ATTACK_SHOCK && g_actor[1].magic >= 30)
				{
					g_actor[1].charge_vel = g_persona[g_actor[1].type].charge_vel;

					/* Check magic first, then to the target screen */
					g_actor[1].magic -= 30;
					UIState_screen = SCREEN_TARGET;
					goto bye;
				}

				if (UIState_action == ATTACK_THUNDER && g_actor[1].magic >= 60)
				{
					g_actor[1].charge_vel = 2;

					/* Check magic first, then to the target screen */
					g_actor[1].magic -= 60;
					UIState_screen = SCREEN_TARGET;
					goto bye;
				}
			}
		}
	}

	/* Draw dynamic elements every single frame */
	if (UIState_screen == SCREEN_ACTION_DONE)
	{
		if (INPUT_UP == 1)
			UIState_action -= 2;
		if (INPUT_DOWN == 1)
			UIState_action += 2;
		if (INPUT_LEFT == 1)
			UIState_action -= 1;
		if (INPUT_RIGHT == 1)
			UIState_action += 1;

		UIState_action = DrawActionUI_dynamic(UIState_action, 26, UIState_actor);
	}
	else
	{
		if (INPUT_LEFT == 1 || INPUT_UP == 1)
			UIState_target -= 1;
		if (INPUT_RIGHT == 1 || INPUT_DOWN == 1)
			UIState_target += 1;

		UIState_target = DrawTargetUI_dynamic(UIState_target, 26);
	}

bye:
	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return next_state;
}


/*===========================*/


void* FieldState()
{
	void* next_state = FieldState;
	uint8_t i = 0;

	/* Developers, Developers, Developers
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
	}*/

	/* HACKKKK!, before any logic calculation we
	need to be 100% sure that there are some enemy */
	for (i = HEROES_NO; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state != ACTOR_STATE_DEAD)
			goto logic; /* Fantastic! */
	}

	/* Lest asume that we win */
	{
		if (s_battle_no < UINT8_MAX)
			s_battle_no += 1;

		next_state = GameStart;
		goto bye;
	}

logic:
	/* Logic step */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;
		else if (g_actor[i].state == ACTOR_STATE_ATTACK)
		{

			/* Wait, is Kuro doing magic */
			if (i == 0 && (g_actor[0].attack_type == ATTACK_HEAL || g_actor[0].attack_type == ATTACK_MEDITATE))
			{
				if (g_actor[0].attack_type == ATTACK_HEAL)
				{
					g_actor[0].health += 50;

					if (g_actor[0].health > 100)
						g_actor[0].health = 100;

					if (g_actor[1].state != ACTOR_STATE_DEAD)
						g_actor[1].health += 50;

					if (g_actor[1].health > 100)
						g_actor[1].health = 100;
				}

				if (g_actor[0].attack_type == ATTACK_MEDITATE)
				{
					g_actor[0].magic += 50;

					if (g_actor[0].magic > 100)
						g_actor[0].magic = 100;

					if (g_actor[1].state != ACTOR_STATE_DEAD)
						g_actor[1].magic += 50;

					if (g_actor[1].magic > 100)
						g_actor[1].magic = 100;
				}

				DrawHUD(22);

				/* Back to idle */
				g_actor[i].idle_time = 0;
				g_actor[i].state = ACTOR_STATE_IDLE;
				continue;
			}

			/* Wait, the actor is holding position */
			else if (g_actor[i].attack_type == ATTACK_HOLD)
			{
				/* Back to idle */
				g_actor[i].idle_time = 0;
				g_actor[i].state = ACTOR_STATE_IDLE;
				continue;
			}

			ActorAttack(i);

			AnimationState_actor = i;
			AnimationState_frame = 0;
			next_state = AnimationState;
			goto bye;
		}
		else if (g_actor[i].state == ACTOR_STATE_CHARGE)
			ActorCharge(i);
		else
		{
			ActorIdle(i);

			if (i < HEROES_NO)
				if (g_actor[i].state != ACTOR_STATE_IDLE)
				{
					UIState_actor = i;
					UIState_screen = 0;
					UIState_action = 0;
					UIState_target = 0;
					next_state = UIState;
					goto bye;
				}
		}
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
	LoadSprite("assets\\ui-ports.jvn", 22);
	LoadSprite("assets\\ui-items.jvn", 26);
	LoadSprite("assets\\fx1.jvn", 24);
	LoadSprite("assets\\fx2.jvn", 25);

	/* Load sprites, whitout repeat */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (GameLoad_initialized[g_actor[i].type] == 0)
		{
			if (g_actor[i].type == TYPE_HERO_A)
				LoadSprite("assets\\sayori.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_HERO_B)
				LoadSprite("assets\\kuro.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_A)
				LoadSprite("assets\\enemy-a.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_B)
				LoadSprite("assets\\enemy-b.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_C)
				LoadSprite("assets\\enemy-c.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_D)
				LoadSprite("assets\\enemy-d.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_E)
				LoadSprite("assets\\enemy-e.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_F)
				LoadSprite("assets\\enemy-f.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_G)
				LoadSprite("assets\\enemy-g.jvn", g_actor[i].type);

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
		DrawHUD(22);

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
	switch (Random() % 4)
	{
	case 0: LoadBackground((uint16_t) "assets\\bkg1.raw"); break;
	case 1: LoadBackground((uint16_t) "assets\\bkg2.raw"); break;
	case 2: LoadBackground((uint16_t) "assets\\bkg3.raw"); break;
	case 3: LoadBackground((uint16_t) "assets\\bkg4.raw"); break;
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
