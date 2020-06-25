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

 [state-battle.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "state.h"
#include "ui.h"
#include "utilities.h"

#define DEVELOPER

static uint8_t s_font1;
static uint8_t s_font1a;
static uint8_t s_font2;
static uint8_t s_spr_portraits;
static uint8_t s_spr_fx1;
static uint8_t s_spr_fx2;

static uint8_t s_battle_no = 0;
static void* sBattleFrame();


/*-----------------------------

 Attack choreography
-----------------------------*/
static uint16_t s_choreo_start = 0; /* In frames */
static char s_buffer1[5] = {'-', 0, 0, 0, 0};

static void* sAttackChoreography()
{
	struct Actor* attacker;
	char* c = NULL;

	uint8_t i = 0;
	uint8_t kuro_prev_hp = 0;
	uint8_t sao_prev_hp = 0;

	if (CURRENT_FRAME == s_choreo_start + 1)
	{
		kuro_prev_hp = g_actor[ACTOR_KURO].health;
		sao_prev_hp = g_actor[ACTOR_SAO].health;

		for (i = 0; i < ACTORS_NO; i++)
		{
			if (g_actor[i].state == ACTOR_STATE_DEAD)
				continue;

			if (g_actor[i].state == ACTOR_STATE_ATTACK)
				attacker = &g_actor[i];

			ActorLogic(&g_actor[i]);

			if (g_actor[i].state == ACTOR_STATE_ATTACK)
				g_actor[i].state = ACTOR_STATE_CHARGE; /* Nope, wait to the next choreo */
		}

		if (g_actor[ACTOR_KURO].health != kuro_prev_hp || g_actor[ACTOR_SAO].health != sao_prev_hp)
			HudDraw(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw();

		CmdDrawRectanglePrecise(34, 3, attacker->x, attacker->y, 41);
		CmdDrawRectanglePrecise(34, 3, attacker->target->x, attacker->target->y, 61);

		c = NumberToString((attacker->target->prev_health - attacker->target->health), s_buffer1);
		CmdDrawText(s_font1a, attacker->target->x, attacker->target->y, c - 1);
	}

	CmdHalt();

	if (CURRENT_FRAME < s_choreo_start + 36 && CURRENT_FRAME > s_choreo_start)
		return (void*)sAttackChoreography;

	return (void*)sBattleFrame;
}


/*-----------------------------

 Battle frame
-----------------------------*/
static void* sBattleFrame()
{
	void* next_frame = (void*)sBattleFrame;

	uint8_t i = 0;
	uint8_t kuro_prev_hp = 0;
	uint8_t sao_prev_hp = 0;

#ifdef DEVELOPER
	/* Change battle */
	if (INPUT_LEFT == 1 && s_battle_no >= 1)
	{
		s_battle_no -= 1;
		return (void*)StateBattle; /* Restarts the entire world */
	}
	else if (INPUT_RIGHT == 1 && s_battle_no < 255)
	{
		s_battle_no += 1;
		return (void*)StateBattle; /* Restarts the entire world */
	}
#endif

	/* Game logic */
	kuro_prev_hp = g_actor[ACTOR_KURO].health;
	sao_prev_hp = g_actor[ACTOR_SAO].health;

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD) /* You are dead, not big surprise */
			continue;

		ActorLogic(&g_actor[i]);

		if (g_actor[i].state == ACTOR_STATE_ATTACK)
		{
			s_choreo_start = CURRENT_FRAME;

			if (next_frame != (void*)sAttackChoreography)
				next_frame = (void*)sAttackChoreography;
			else
				g_actor[i].state = ACTOR_STATE_CHARGE; /* Nope, wait to the next choreo */
		}
	}

	/* Draw */
	if (g_actor[ACTOR_KURO].health != kuro_prev_hp || g_actor[ACTOR_SAO].health != sao_prev_hp)
		HudDraw(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

	ActorsDraw();

	/* Bye! */
	CmdHalt();
	return next_frame;
}


/*-----------------------------

 Start a new battle
-----------------------------*/
static uint16_t s_loading_start = 0;

static void* sWait()
{

#ifdef DEVELOPER
	/* Change battle */
	if (INPUT_LEFT == 1 && s_battle_no >= 1)
	{
		s_battle_no -= 1;
		return (void*)StateBattle; /* Restarts the entire world */
	}
	else if (INPUT_RIGHT == 1 && s_battle_no < 255)
	{
		s_battle_no += 1;
		return (void*)StateBattle; /* Restarts the entire world */
	}
#endif

	/* We need to be sure of show the loading screen for at least 1.5 seconds */
	if (CURRENT_MILLISECONDS < s_loading_start + 1500 && CURRENT_MILLISECONDS > s_loading_start)
		return (void*)sWait;

	/* Next state! */
	CmdDrawBackground();
	HudDraw(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

	return sBattleFrame();
}

static void* sLoad()
{
	s_font1a = IntLoadSprite("assets\\font1a.jvn");
	s_spr_portraits = IntLoadSprite("assets\\ui-ports.jvn");
	s_spr_fx1 = IntLoadSprite("assets\\fx1.jvn");
	s_spr_fx2 = IntLoadSprite("assets\\fx2.jvn");

	ActorsInitializeSprites();

	return sWait();
}

void* StateBattle()
{
	uint8_t i = 0;
	uint16_t text_y = 0;

	IntPrintText("# StateBattle\n");
	IntUnloadAll();

	/* Reload minimal assets for the 'loading' screen */
	s_font1 = IntLoadSprite("assets\\font1.jvn");
	s_font2 = IntLoadSprite("assets\\font2.jvn");

	switch (Random() % 4)
	{
	case 0: IntLoadBackground("assets\\bkg1.raw"); break;
	case 1: IntLoadBackground("assets\\bkg2.raw"); break;
	case 2: IntLoadBackground("assets\\bkg3.raw"); break;
	case 3: IntLoadBackground("assets\\bkg4.raw");
	}

	/* Draw loading screen */
	CmdDrawBackground();

	ActorsInitialize(s_battle_no);
	CmdDrawText(s_font2, 10, 10, (g_live_enemies > 1) ? "Monsters appear!" : "Monster appears!");

	text_y = 10;

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state != ACTOR_STATE_DEAD && (g_actor[i].persona->tags & TAG_ENEMY))
		{
			text_y += 10;
			CmdDrawText(s_font1, 10, text_y, g_actor[i].persona->name);
		}
	}

	/* Bye! */
	s_loading_start = CURRENT_MILLISECONDS;

	CmdHalt();
	return (void*)sLoad;
}
