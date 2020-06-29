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
/*#define AUTO_BATTLE*/

static uint8_t s_battle_no = 0;

static uint8_t s_font1;
static uint8_t s_font1a;
static uint8_t s_font2;
static uint8_t s_spr_portraits;
static uint8_t s_spr_fx1;
static uint8_t s_spr_fx2;
static uint8_t s_spr_items;

/* Don't set states directly!... */
static void* sChoreographyFrame();
static void* sPanelFrame();
static void* sBattleFrame();

/* Use the following setters: */
static void* sSetChoreography();
static void* sSetPanel(uint8_t actor_index);
static void* sSetBattle();


/*-----------------------------

 Choreography
-----------------------------*/
#define CHOREO_DURATION 36 /* In frames */

static uint16_t s_choreo_start = 0; /* In frames */
static struct Actor* s_choreo_attacker = NULL;

static char s_choreo_buffer[4] = {0, 0, 0, 0};
static char* s_choreo_hp_str = NULL;

static void* sSetChoreography()
{
	s_choreo_start = CURRENT_FRAME;
	s_choreo_attacker = NULL;
	s_choreo_hp_str = NULL;

	return (void*)sChoreographyFrame;
}

static void* sChoreographyFrame()
{
	uint8_t i = 0;

	/* Just for the first frame we run the logic */
	if (CURRENT_FRAME == s_choreo_start + 1)
	{
		for (i = 0; i < ON_SCREEN_ACTORS; i++)
		{
			if (g_actor[i].state == ACTOR_STATE_DEAD)
				continue;

			if (g_actor[i].state == ACTOR_STATE_ATTACK)
				s_choreo_attacker = &g_actor[i];

			ActorLogic(&g_actor[i]);

			/* Some new attackers can arise, we obligate them to wait until
			   the next logic step (that isn't gonna happens on this choreo) */
			if (g_actor[i].state == ACTOR_STATE_ATTACK)
				g_actor[i].state = ACTOR_STATE_CHARGE;
		}

		/* During a choreography is the only time that actors lose health,
		   so we keep an eye on the main heroes to update the Hud accordingly */
		if (g_actor[ACTOR_KURO].health != g_actor[ACTOR_KURO].prev_health ||
		    g_actor[ACTOR_SAO].health != g_actor[ACTOR_SAO].prev_health)
			UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		/* How many health our target loss? */
		s_choreo_hp_str = NumberToString((s_choreo_attacker->target->prev_health - s_choreo_attacker->target->health),
		                                 s_choreo_buffer);
	}

	/* Draw actors and other things, every frame for the rest of the choreography */
	{
		ActorsDraw(0);

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
		CmdDrawText(s_font1a, s_choreo_attacker->target->x, s_choreo_attacker->target->y, s_choreo_hp_str);
	}

	/* Bye! */
	CmdHalt();

	if (CURRENT_FRAME < s_choreo_start + CHOREO_DURATION && CURRENT_FRAME > s_choreo_start)
		return (void*)sChoreographyFrame;

	return sSetBattle();
}


/*-----------------------------

 Panel
-----------------------------*/
#define PANEL_SCREEN_ACTION 0
#define PANEL_SCREEN_TARGET 1

static uint16_t s_panel_start = 0; /* In frames */
static uint8_t s_panel_screen = 0;
static uint8_t s_panel_actor = 0;
static uint8_t s_panel_selection = 0;

static void* sSetPanel(uint8_t actor_index)
{
	s_panel_start = CURRENT_FRAME;
	s_panel_screen = PANEL_SCREEN_ACTION;
	s_panel_actor = actor_index;
	s_panel_selection = 0;
	return (void*)sPanelFrame;
}

static void* sPanelFrame()
{
	void* next_frame = (void*)sPanelFrame;

	/* Select an action for the current actor... */
	if (s_panel_screen == PANEL_SCREEN_ACTION)
	{
		if (CURRENT_FRAME == s_panel_start + 1) /* First frame only */
		{
			UiPanelAction_static(s_spr_portraits, s_font2, g_actor[s_panel_actor].persona, &g_actor[ACTOR_SAO],
			                     &g_actor[ACTOR_KURO]);
			CmdDrawRectanglePrecise(34, 3, g_actor[s_panel_actor].x, g_actor[s_panel_actor].y, 8);
		}

		if (INPUT_UP == 1)
			s_panel_selection -= 2;
		if (INPUT_DOWN == 1)
			s_panel_selection += 2;
		if (INPUT_LEFT == 1)
			s_panel_selection -= 1;
		if (INPUT_RIGHT == 1)
			s_panel_selection += 1;

		s_panel_selection =
		    UiPanelAction_dynamic(s_spr_items, s_font1, g_actor[s_panel_actor].persona, s_panel_selection);
	}

	/* Select a target for the current actor */
	else if (s_panel_screen == PANEL_SCREEN_TARGET)
	{
		if (CURRENT_FRAME == s_panel_start + 1) /* First frame only */
			UiPanelTarget_static(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		if (INPUT_LEFT == 1 || INPUT_UP == 1)
			s_panel_selection -= 1;
		if (INPUT_RIGHT == 1 || INPUT_DOWN == 1)
			s_panel_selection += 1;

		s_panel_selection = UiPanelTarget_dynamic(s_spr_items, s_panel_selection);
	}

	/* Player pressed enter, is turn for the next screen, next player or go back to the battle? */
	if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1)
	{
		s_panel_start = CURRENT_FRAME;

		/* Go to next screen, show next one? */
		if (s_panel_screen == PANEL_SCREEN_ACTION)
			s_panel_screen = PANEL_SCREEN_TARGET;
		else
		{
			g_actor[s_panel_actor].panel_done = 1; /* To avoid show the panel multiple times */

		again:
			/* Check the next actor (if it need the panel) */
			if (s_panel_actor < (ON_SCREEN_HEROES - 1))
			{
				s_panel_actor += 1;

				if (g_actor[s_panel_actor].state != ACTOR_STATE_CHARGE || g_actor[s_panel_actor].panel_done == 1)
					goto again;
			}
			else
			{
				/* Back to battle! */
				UiPanelClean();
				UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);
				next_frame = sSetBattle();
			}
		}
	}

	CmdHalt();
	return next_frame;
}


/*-----------------------------

 Battle
-----------------------------*/
static uint16_t s_battle_banner = 0;

static void* sSetBattle()
{
	s_battle_banner = 0;
	return (void*)sBattleFrame;
}

static void* sBattleFrame()
{
	void* next_frame = (void*)sBattleFrame;
	uint8_t i = 0;

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
	/* Heal */
	if (INPUT_UP)
	{
		g_actor[ACTOR_KURO].health = 100;
		g_actor[ACTOR_SAO].health = 100;
		UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);
	}
#endif

	/* Game logic */
	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD) /* You are dead, not big surprise */
			continue;

		ActorLogic(&g_actor[i]);

#ifndef AUTO_BATTLE
		if (i < ON_SCREEN_HEROES)
		{
			if (g_actor[i].state == ACTOR_STATE_CHARGE && g_actor[i].panel_done == 0)
				next_frame = sSetPanel(i);

			if (g_actor[i].state != ACTOR_STATE_CHARGE)
				g_actor[i].panel_done = 0;
		}
#endif

		/* If after the logic step the actor set itself to 'attack', we
		   prepare the attack choreography to be executed the next frame */
		if (g_actor[i].state == ACTOR_STATE_ATTACK)
		{
			if (next_frame == (void*)sBattleFrame) /* Indirectly this check gives 'Panel' an high priority */
				next_frame = sSetChoreography();

			/* If a previous actor already gained the choreo, we set this
			   one to 'charge', an state previous to 'attack'. Obligating it
			   to wait until the next logic step */
			else
				g_actor[i].state = ACTOR_STATE_CHARGE;
		}
	}

	/* Draw */
	ActorsDraw(1);

	/* 'Victory!', 'Game over' dialogs */
	if (g_live_enemies == 0)
	{
		s_battle_banner += 1;

		if (s_battle_banner > 36) /* One second and half */
			UiBanner(s_font2, "Victory");

		if (s_battle_banner > 96) /* Four seconds */
		{
			s_battle_banner = 0;
			s_battle_no += 1;
			next_frame = (void*)StateBattle; /* Restarts the entire world */
		}
	}

	if (g_live_heroes == 0)
		UiBanner(s_font2, "Game Over");

	/* Bye! */
	CmdHalt();
	return next_frame;
}


/*-----------------------------

 Initialize the entire state
-----------------------------*/
static uint16_t s_state_start = 0; /* In milliseconds */
static uint8_t s_state_prev_bkg = 0;

static void* sWait()
{
	/* We need to be sure of show the loading screen for at least 2 seconds in
	   those cpus that did the loading job unrealistically fast (emulated software).

	   In slower cpus this timer will overflow if the loading time pass the
	   ~6 seconds mark, making a final wait of ~8 seconds. Not really a problem */
	if (CURRENT_MILLISECONDS < s_state_start + 2000 && CURRENT_MILLISECONDS > s_state_start)
		return (void*)sWait;

	/* Yay, next state! */
	CmdDrawBackground();
	UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

	CmdHalt();
	return sSetBattle();
}

static void* sLoad()
{
	s_font1 = IntLoadSprite("assets\\font1.jvn");
	s_font1a = IntLoadSprite("assets\\font1a.jvn");
	s_spr_portraits = IntLoadSprite("assets\\ui-ports.jvn");
	s_spr_fx1 = IntLoadSprite("assets\\fx1.jvn");
	s_spr_fx2 = IntLoadSprite("assets\\fx2.jvn");
	s_spr_items = IntLoadSprite("assets\\ui-items.jvn");

	ActorsInitializeSprites();

	return sWait();
}

void* StateBattle()
{
	uint8_t bkg = 0;

	s_state_start = CURRENT_MILLISECONDS;

	IntPrintText("# StateBattle\n");
	IntUnloadAll();

	/* Reload minimal assets for the 'loading' screen */
	s_font2 = IntLoadSprite("assets\\font2.jvn");

	while (bkg == s_state_prev_bkg)
		bkg = (uint8_t)(Random() % 4);

	switch (bkg)
	{
	case 0: IntLoadBackground("assets\\bkg1.raw"); break;
	case 1: IntLoadBackground("assets\\bkg2.raw"); break;
	case 2: IntLoadBackground("assets\\bkg3.raw"); break;
	case 3: IntLoadBackground("assets\\bkg4.raw");
	}

	s_state_prev_bkg = bkg;
	CmdDrawBackground();

	/* Draw loading screen */
	ActorsInitialize(s_battle_no);
	UiBanner(s_font2, (g_live_enemies > 1) ? "Monsters appear!" : "Monster appears!");

	/* Bye! */
	CmdHalt();
	return (void*)sLoad;
}
