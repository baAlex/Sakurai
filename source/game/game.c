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
#include "engine.h"
#include "ui.h"
#include "utilities.h"

/*#define DEV*/

uint8_t Layout(uint8_t battle_no, uint8_t* out);

void* AnimationState();
void* FieldState();

static uint8_t s_battle_no = 0;
static uint8_t s_battle_layout[ENEMIES_NO] = {0, 0, 0, 0};


/*===========================*/


uint16_t GameOver_frame = 0;

void* GameOver()
{
	CmdDrawRectangle(20 /* 320 px */, 2 /* 32 px */, 0, 100 - 16, 15);
	CmdDrawText(21, 8, 100 - 16, "Game Over");

	/* Bye! */
	GameOver_frame += 1;

	CmdHalt();
	return GameOver;
}


/*===========================*/


static uint8_t AnimationState_frame = 0;
static uint8_t AnimationState_actor = 0;

void* AnimationState()
{
	uint8_t target = g_actor[AnimationState_actor].target;

	/* Draw actors normally */
	DrawActors();

	/* Draw some fancy animation */
	if (AnimationState_actor < HEROES_NO)
		CmdDrawSprite(24, g_info[target].base_x, g_info[target].base_y, AnimationState_frame);
	else
		CmdDrawSprite(25, g_info[target].base_x, g_info[target].base_y, AnimationState_frame);

	/* Bye! */
	AnimationState_frame += 1;

	if ((g_actor[AnimationState_actor].attack_type == ATTACK_SIMPLE && AnimationState_frame >= 6) ||
	    AnimationState_frame >= 24)
	{
		if (AnimationState_actor >= HEROES_NO)
			HudDraw(22, &g_actor[0], &g_actor[1]); /* To update damages */

		CmdHalt();
		return FieldState;
	}

	CmdHalt();
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

	/* Only draw static elements on the first frame */
	if (UIState_screen == SCREEN_ACTION)
	{
		MenuActionDraw_static(22, &g_actor[UIState_actor], &g_actor[0], &g_actor[1]);
		UIState_screen = SCREEN_ACTION_DONE;
	}
	else if (UIState_screen == SCREEN_TARGET)
	{
		MenuTargetDraw_static(22, &g_actor[0], &g_actor[1]);
		UIState_screen = SCREEN_TARGET_DONE;
	}

	/* Player press ENTER */
	if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1)
	{
		/* Return to the field */
		if (UIState_screen == SCREEN_TARGET_DONE)
		{
			g_actor[UIState_actor].target = UIState_target;

			MenuClean();
			HudDraw(22, &g_actor[0], &g_actor[1]);
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
				MenuClean();
				HudDraw(22, &g_actor[0], &g_actor[1]);
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
					MenuClean();
					HudDraw(22, &g_actor[0], &g_actor[1]);
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

		UIState_action = MenuActionDraw_dynamic(26, &g_actor[UIState_actor], UIState_action);
	}
	else
	{
		if (INPUT_LEFT == 1 || INPUT_UP == 1)
			UIState_target -= 1;
		if (INPUT_RIGHT == 1 || INPUT_DOWN == 1)
			UIState_target += 1;

		UIState_target = MenuTargetDraw_dynamic(26, UIState_target);
	}

bye:
	/* Bye! */
	CmdHalt();
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

	/* Hack! */
	if (g_actor[0].state == ACTOR_STATE_DEAD && g_actor[1].state == ACTOR_STATE_DEAD)
	{
		next_state = GameOver;
		goto bye;
	}

	/* HACKKKK!, before any logic calculation we
	need to be 100% sure that there are some enemy */
	for (i = HEROES_NO; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state != ACTOR_STATE_DEAD)
			goto logic; /* Fantastic! */
	}

	/* Lest assume that we win */
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

				HudDraw(22, &g_actor[0], &g_actor[1]);

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
	CmdHalt();
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

	uint8_t i = 0;

	if (GameLoad_frame != 0)
		goto bye;

	Clear(GameLoad_initialized, TYPES_NO);

	/* Load remaining resources */
	IntLoadSprite("assets\\ui-ports.jvn", 22);
	IntLoadSprite("assets\\ui-items.jvn", 26);
	IntLoadSprite("assets\\fx1.jvn", 24);
	IntLoadSprite("assets\\fx2.jvn", 25);

	/* Load sprites, whitout repeat */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (GameLoad_initialized[g_actor[i].type] == 0)
		{
			if (g_actor[i].type == TYPE_HERO_A)
				IntLoadSprite("assets\\sayori.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_HERO_B)
				IntLoadSprite("assets\\kuro.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_A)
				IntLoadSprite("assets\\enemy-a.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_B)
				IntLoadSprite("assets\\enemy-b.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_C)
				IntLoadSprite("assets\\enemy-c.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_D)
				IntLoadSprite("assets\\enemy-d.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_E)
				IntLoadSprite("assets\\enemy-e.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_F)
				IntLoadSprite("assets\\enemy-f.jvn", g_actor[i].type);
			else if (g_actor[i].type == TYPE_G)
				IntLoadSprite("assets\\enemy-g.jvn", g_actor[i].type);

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
		CmdDrawBackground();
		HudDraw(22, &g_actor[0], &g_actor[1]);

		CmdHalt();

		IntPrintText("# GameLoad() ends: ");
		IntPrintNumber(s_battle_no);
		return FieldState;
	}

	CmdHalt();
	return GameLoad;
}


void* GameStart()
{
	uint8_t i = 0;
	uint16_t text_x = 0;

	/* Load indispensable resources (TODO!) */
	IntUnloadAll(); /* TODO, not everything! */

	IntLoadSprite("assets\\font1.jvn", 20);
	IntLoadSprite("assets\\font2.jvn", 21);

	/* Load an random background and draw it */
	switch (Random() % 4)
	{
	case 0: IntLoadBackground("assets\\bkg1.raw"); break;
	case 1: IntLoadBackground("assets\\bkg2.raw"); break;
	case 2: IntLoadBackground("assets\\bkg3.raw"); break;
	case 3: IntLoadBackground("assets\\bkg4.raw"); break;
	}

	CmdDrawBackground();

	/* Print "Enemies appear!" */
	CmdDrawRectangle(20 /* 320 px */, 2 /* 32 px */, 0, 100 - 16, 15);

	if (Layout(s_battle_no, s_battle_layout) != 1)
		CmdDrawText(21, 8, 100 - 16, "Monsters appear!");
	else
		CmdDrawText(21, 8, 100 - 16, "Monster appears!");

	text_x = 8;

	/* Initialize actors */
	for (i = 0; i < ACTORS_NO; i++)
	{
		InitializeActor(i, (s_battle_no < 32) ? s_battle_layout : (uint8_t*)NULL);

		/* Print enemy name */
		if (i < HEROES_NO)
			continue;

		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		CmdDrawText(20, text_x, 100 - 16 + 12, g_persona[g_actor[i].type].name);
		text_x += 76; /* (320 - 8 - 8) / 4 */
	}

	/* Bye! */
	GameLoad_frame = 0;

	IntPrintText("# GameStart() ends: ");
	IntPrintNumber(s_battle_no);

	CmdHalt();
	return GameLoad;
}
