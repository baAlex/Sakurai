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

#define CHOREO_DURATION 36 /* In frames */

static uint16_t s_choreo_start = 0; /* In frames */
struct Actor* s_choreo_attacker;
static char s_buffer[5] = {'-', 0, 0, 0, 0};

static void* sAttackChoreography()
{
	char* c = NULL;

	uint8_t i = 0;
	uint8_t kuro_prev_hp = 0;
	uint8_t sao_prev_hp = 0;

	/* For the first frame we run the logic as always... */
	if (CURRENT_FRAME == s_choreo_start + 1)
	{
		kuro_prev_hp = g_actor[ACTOR_KURO].health;
		sao_prev_hp = g_actor[ACTOR_SAO].health;

		for (i = 0; i < ON_SCREEN_ACTORS; i++)
		{
			if (g_actor[i].state == ACTOR_STATE_DEAD)
				continue;

			/* ... except that we keep note of who is attacking */
			if (g_actor[i].state == ACTOR_STATE_ATTACK)
				s_choreo_attacker = &g_actor[i];

			ActorLogic(&g_actor[i]);

			/* After the logic step some new attackers can arise, we
			   obligate them to wait until the next logic step */
			if (g_actor[i].state == ACTOR_STATE_ATTACK)
				g_actor[i].state = ACTOR_STATE_CHARGE;
		}

		if (g_actor[ACTOR_KURO].health != kuro_prev_hp || g_actor[ACTOR_SAO].health != sao_prev_hp)
		{

#ifdef DEVELOPER
			g_actor[ACTOR_KURO].health = 100;
			g_actor[ACTOR_SAO].health = 100;
#endif
			HudDraw(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);
		}
	}

	/* Draw actors every frame like we normally do... */
	ActorsDraw(0);

	/* ... and then draw a lot of new things: */
	{
		if ((CURRENT_FRAME - s_choreo_start) - 1 < 6)
		{
			if (s_choreo_attacker->persona->tags & TAG_ENEMY)
				CmdDrawSprite(s_spr_fx2, s_choreo_attacker->target->x, s_choreo_attacker->target->y,
				              (CURRENT_FRAME - s_choreo_start) - 1);
			else
				CmdDrawSprite(s_spr_fx1, s_choreo_attacker->target->x, s_choreo_attacker->target->y,
				              (CURRENT_FRAME - s_choreo_start) - 1);
		}

		CmdDrawRectanglePrecise(34, 3, s_choreo_attacker->x, s_choreo_attacker->y, 41);
		CmdDrawRectanglePrecise(34, 3, s_choreo_attacker->target->x, s_choreo_attacker->target->y, 61);

		c = NumberToString((s_choreo_attacker->target->prev_health - s_choreo_attacker->target->health), s_buffer);
		CmdDrawText(s_font1a, s_choreo_attacker->target->x, s_choreo_attacker->target->y, c - 1);
	}

	/* Bye! */
	CmdHalt();

	if (CURRENT_FRAME < s_choreo_start + CHOREO_DURATION && CURRENT_FRAME > s_choreo_start)
		return (void*)sAttackChoreography;

	return (void*)sBattleFrame;
}


/*-----------------------------

 Battle frame
-----------------------------*/
static uint16_t s_show_banner = 0;

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

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD) /* You are dead, not big surprise */
			continue;

		ActorLogic(&g_actor[i]);

		/* If after the logic step the actor set itself to 'attack', we
		   prepare the attack choreography to be executed the next frame */
		if (g_actor[i].state == ACTOR_STATE_ATTACK)
		{
			s_choreo_start = CURRENT_FRAME;

			if (next_frame != (void*)sAttackChoreography)
				next_frame = (void*)sAttackChoreography;

			/* If a previous actor already gained the choreo, we set this
			   one to 'charge', an state previous to 'attack'. Obligating it
			   to wait until the next logic step */
			else
				g_actor[i].state = ACTOR_STATE_CHARGE;
		}
	}

	/* Draw
	if (g_actor[ACTOR_KURO].health != kuro_prev_hp || g_actor[ACTOR_SAO].health != sao_prev_hp)
	    HudDraw(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);*/

	ActorsDraw(1);

	/* 'Victory!', 'Game over' dialogs */
	if (g_live_enemies == 0)
	{
		s_show_banner += 1;

		if (s_show_banner > 36)
			GenericDialogDraw(s_font2, "Victory!");

		if (s_show_banner > 96)
		{
			s_battle_no += 1;
			CmdHalt();
			return (void*)StateBattle; /* Restarts the entire world */
		}
	}

	if (g_live_heroes == 0)
		GenericDialogDraw(s_font2, "Game Over");

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

	s_show_banner = 0;

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

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
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
