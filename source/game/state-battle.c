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

/*#define DEVELOPER
#define AUTO_BATTLE*/

static uint8_t s_battle_no = 0;

static uint8_t s_font1 = 0;
static uint8_t s_font1a = 0;
static uint8_t s_font2 = 0;
static uint8_t s_spr_portraits = 0;
static uint8_t s_spr_fx1 = 0;
static uint8_t s_spr_fx2 = 0;
static uint8_t s_spr_items = 0;

static uint8_t s_toggle_x = 0;
static uint8_t s_toggle_l = 0;
static uint8_t s_toggle_r = 0;
static uint8_t s_toggle_u = 0;
static uint8_t s_toggle_d = 0;
static uint8_t s_toggle_start = 0;

static void* sPrepareChoreography();
static void* sPreparePanel(uint8_t actor_index, uint8_t screen);
static void* sPrepareBattle();

static void* sChoreographyResumeFromPause();
static void* sPanelResumeFromPause();
static void* sBattleResumeFromPause();


/*-----------------------------

 Choreography
-----------------------------*/
#define CHOREO_DURATION 36 /* In frames */

static uint16_t s_choreo_start = 0; /* In frames */
static struct Actor* s_choreo_attacker = NULL;

static char s_choreo_buffer[10] = {0};
static char* s_choreo_hp_str = NULL;


static void* sChoreographyFrame()
{
	if (KeyToggle(INPUT_START, &s_toggle_start) == 1)
		return StatePreparePause(s_font1, s_font2, s_spr_items, (void*)sChoreographyResumeFromPause);

	ActorsDraw(0);

	if (!(s_choreo_attacker->action->tags & TAG_ACTION_PARTY))
	{
		if ((CURRENT_FRAME - s_choreo_start) - 1 < 6)
		{
			if (s_choreo_attacker->persona->tags & TAG_PERSONA_ENEMY)
				CmdDrawSprite(s_spr_fx2, s_choreo_attacker->target->x, s_choreo_attacker->target->y,
				              (CURRENT_FRAME - s_choreo_start) - 1);
			else
				CmdDrawSprite(s_spr_fx1, s_choreo_attacker->target->x, s_choreo_attacker->target->y,
				              (CURRENT_FRAME - s_choreo_start) - 1);
		}

		CmdDrawRectanglePrecise(34, 3, s_choreo_attacker->x, s_choreo_attacker->y, 41);
		CmdDrawRectanglePrecise(34, 3, s_choreo_attacker->target->x, s_choreo_attacker->target->y, 61);
		CmdDrawText(s_font1a, s_choreo_attacker->target->x, s_choreo_attacker->target->y + 3, s_choreo_hp_str);
	}
	else /* The action applies to the own party */
	{
		CmdDrawRectanglePrecise(34, 3, s_choreo_attacker->x, s_choreo_attacker->y, 36);
	}

	/* Bye! */
	CmdHalt();

	if (CURRENT_FRAME < s_choreo_start + CHOREO_DURATION && CURRENT_FRAME > s_choreo_start)
		return (void*)sChoreographyFrame;

	return sPrepareBattle();
}


static void* sChoreographyInit()
{
	uint8_t i = 0;

	/* Just for the first frame we run the logic */
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

	/* How many health our target loss? */
	if (s_choreo_attacker->target->state == ACTOR_STATE_HOLD)
		s_choreo_hp_str = "Cancelled";
	else
	{
		if (s_choreo_attacker->action == &g_action[ACTION_GENERIC])
			s_choreo_hp_str = NumberToString(s_choreo_attacker->persona->generic_damage, s_choreo_buffer);
		else
			s_choreo_hp_str = NumberToString(s_choreo_attacker->action->amount, s_choreo_buffer);
	}

	return sChoreographyFrame();
}


static void* sChoreographyResumeFromPause()
{
	s_toggle_x = 0;
	s_toggle_l = 0;
	s_toggle_r = 0;
	s_toggle_u = 0;
	s_toggle_d = 0;
	s_toggle_start = 0;

	return sChoreographyInit();
}


static void* sPrepareChoreography()
{
	s_choreo_start = CURRENT_FRAME;
	s_choreo_attacker = NULL;
	s_choreo_hp_str = NULL;

	return (void*)sChoreographyInit;
}


/*-----------------------------

 Panel
-----------------------------*/
#define PANEL_SCREEN_ACTION 0
#define PANEL_SCREEN_TARGET 1

static uint8_t s_panel_screen = 0;
static uint8_t s_panel_actor = 0;
static uint8_t s_panel_selection = 0;


static void* sPanelFrame()
{
	void* next_frame = (void*)sPanelFrame;
	struct Action* action = NULL;

	if (KeyToggle(INPUT_START, &s_toggle_start) == 1)
		return StatePreparePause(s_font1, s_font2, s_spr_items, (void*)sPanelResumeFromPause);

	/* Select an action for the current actor */
	if (s_panel_screen == PANEL_SCREEN_ACTION)
	{
		if (KeyRepeat(INPUT_PAD_U, &s_toggle_u) == 1)
			s_panel_selection -= 2;
		if (KeyRepeat(INPUT_PAD_D, &s_toggle_d) == 1)
			s_panel_selection += 2;
		if (KeyRepeat(INPUT_PAD_L, &s_toggle_l) == 1)
			s_panel_selection -= 1;
		if (KeyRepeat(INPUT_PAD_R, &s_toggle_r) == 1)
			s_panel_selection += 1;

		s_panel_selection =
		    UiPanelAction_dynamic(s_spr_items, s_font1, g_actor[s_panel_actor].persona, s_panel_selection);
	}

	/* Select a target for the current actor */
	else if (s_panel_screen == PANEL_SCREEN_TARGET)
	{
		if (KeyRepeat(INPUT_PAD_L, &s_toggle_l) == 1 || KeyRepeat(INPUT_PAD_U, &s_toggle_u) == 1)
			s_panel_selection -= 1;
		if (KeyRepeat(INPUT_PAD_R, &s_toggle_r) == 1 || KeyRepeat(INPUT_PAD_D, &s_toggle_d) == 1)
			s_panel_selection += 1;

		s_panel_selection = UiPanelTarget_dynamic(s_spr_items, s_panel_selection);
	}

	if (KeyToggle(INPUT_X, &s_toggle_x) == 1)
	{
		/* Overwrite actor action */
		if (s_panel_screen == PANEL_SCREEN_ACTION)
		{
			/* HACK, following values are all hardcoded */
			if (s_panel_selection == 1)
				action = &g_action[ACTION_COMBINED];
			else if (s_panel_selection == 4)
			{
				/* TODO, this procedure feels hacky */
				ActorSetHold(&g_actor[s_panel_actor]);
				goto next_actor;
			}

			if (g_actor[s_panel_actor].persona == &g_heroes[PERSONALITY_KURO])
			{
				if (s_panel_selection == 0)
					action = &g_action[ACTION_SIMPLE_KURO];
				else if (s_panel_selection == 2)
					action = &g_action[ACTION_HEAL];
				else if (s_panel_selection == 3)
					action = &g_action[ACTION_MEDITATE];
			}
			else
			{
				if (s_panel_selection == 0)
					action = &g_action[ACTION_SIMPLE_SAO];
				else if (s_panel_selection == 2)
					action = &g_action[ACTION_SHOCK];
				else if (s_panel_selection == 3)
					action = &g_action[ACTION_THUNDER];
			}
			/* </HACK> */

			if (action == NULL)
				action = &g_action[ACTION_GENERIC];

			if (g_actor[s_panel_actor].magic >= action->magic_cost)
			{
				g_actor[s_panel_actor].action = action;

				/* This action needs an target screen? */
				if (!(action->tags & TAG_ACTION_PARTY))
					next_frame = sPreparePanel(s_panel_actor, PANEL_SCREEN_TARGET);
				else
					goto next_actor;
			}
		}

		/* Overwrite actor target */
		else
		{
			g_actor[s_panel_actor].target = &g_actor[s_panel_selection];

		next_actor:
			/* Check the next actor, if it need the panel */
			g_actor[s_panel_actor].panel_done = 1; /* To avoid show the panel multiple times */

		again:
			if (s_panel_actor < (ON_SCREEN_HEROES - 1))
			{
				s_panel_actor += 1;

				if (g_actor[s_panel_actor].state != ACTOR_STATE_CHARGE || g_actor[s_panel_actor].panel_done == 1)
					goto again;
			}
			else
				next_frame = sPrepareBattle();
		}
	}

	CmdHalt();
	return next_frame;
}


static void* sPanelInit()
{
	CmdDrawBackground();
	ActorsDraw(0);

	if (s_panel_screen == PANEL_SCREEN_ACTION)
		UiPanelAction_static(s_spr_portraits, s_font2, g_actor[s_panel_actor].persona, &g_actor[ACTOR_SAO],
		                     &g_actor[ACTOR_KURO]);
	else if (s_panel_screen == PANEL_SCREEN_TARGET)
		UiPanelTarget_static(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

	CmdDrawRectanglePrecise(34, 3, g_actor[s_panel_actor].x, g_actor[s_panel_actor].y, 8);

	return sPanelFrame();
}


static void* sPanelResumeFromPause()
{
	s_toggle_x = 0;
	s_toggle_l = 0;
	s_toggle_r = 0;
	s_toggle_u = 0;
	s_toggle_d = 0;
	s_toggle_start = 0;

	return sPanelInit();
}


static void* sPreparePanel(uint8_t actor_index, uint8_t screen)
{
	s_panel_screen = screen;
	s_panel_actor = actor_index;
	s_panel_selection = 0;
	return (void*)sPanelInit;
}


/*-----------------------------

 Battle
-----------------------------*/
static uint16_t s_battle_banner = 0;


static void* sBattleFrame()
{
	void* next_frame = (void*)sBattleFrame;
	uint8_t i = 0;

#ifdef DEVELOPER
	/* Change battle */
	if (KeyToggle(INPUT_PAD_L, &s_toggle_l) == 1 && s_battle_no >= 1)
	{
		s_battle_no -= 1;
		next_frame = StatePrepareBattle(s_battle_no); /* Restarts the entire world */
	}
	else if (KeyToggle(INPUT_PAD_R, &s_toggle_r) == 1 && s_battle_no < 255)
	{
		s_battle_no += 1;
		next_frame = StatePrepareBattle(s_battle_no); /* Restarts the entire world */
	}
	/* Heal */
	if (KeyToggle(INPUT_PAD_U, &s_toggle_u))
	{
		g_actor[ACTOR_KURO].health = 100;
		g_actor[ACTOR_SAO].health = 100;

		if (g_actor[ACTOR_KURO].state == ACTOR_STATE_DEAD)
		{
			g_actor[ACTOR_KURO].state = ACTOR_STATE_IDLE;
			g_live_heroes += 1;
		}

		if (g_actor[ACTOR_SAO].state == ACTOR_STATE_DEAD)
		{
			g_actor[ACTOR_SAO].state = ACTOR_STATE_IDLE;
			g_live_heroes += 1;
		}

		UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);
	}
#endif

	if (KeyToggle(INPUT_START, &s_toggle_start) == 1)
		return StatePreparePause(s_font1, s_font2, s_spr_items, (void*)sBattleResumeFromPause);

	/* Game logic */
	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD) /* «You are dead, not big surprise» */
			continue;

		ActorLogic(&g_actor[i]);

#ifndef AUTO_BATTLE
		if (i < ON_SCREEN_HEROES)
		{
			if (g_actor[i].state == ACTOR_STATE_CHARGE && g_actor[i].panel_done == 0)
				next_frame = sPreparePanel(i, PANEL_SCREEN_ACTION);

			if (g_actor[i].state != ACTOR_STATE_CHARGE)
				g_actor[i].panel_done = 0;
		}
#endif

		/* If after the logic step the actor set itself to 'attack', we
		   prepare the attack choreography to be executed next frame */
		if (g_actor[i].state == ACTOR_STATE_ATTACK)
		{
			if (next_frame == (void*)sBattleFrame) /* Indirectly this check gives 'Panel' an high priority */
				next_frame = sPrepareChoreography();

			/* If a previous actor already gained the choreo, we set this
			   one to 'charge', an state previous to 'attack'. Obligating it
			   to wait until the next logic step */
			else
				g_actor[i].state = ACTOR_STATE_CHARGE;
		}
	}

	/* Draw */
	ActorsDraw(1);

	/* 'Victory!', 'Game over' dialogues */
	if (g_live_enemies == 0)
	{
		s_battle_banner += 1;

		if (s_battle_banner > 36) /* One second and half */
			UiBanner(s_font2, "Victory");

		if (s_battle_banner > 96) /* Four seconds */
		{
			s_battle_banner = 0;
			s_battle_no += 1;
			next_frame = StatePrepareBattle(s_battle_no); /* Restarts the entire world */
		}
	}

	if (g_live_heroes == 0)
		UiBanner(s_font2, "Game Over");

	/* Bye! */
	CmdHalt();
	return next_frame;
}


static void* sBattleInit()
{
	CmdDrawBackground();
	UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);
	return sBattleFrame();
}


static void* sBattleResumeFromPause()
{
	s_toggle_x = 0;
	s_toggle_l = 0;
	s_toggle_r = 0;
	s_toggle_u = 0;
	s_toggle_d = 0;
	s_toggle_start = 0;

	return sBattleInit();
}


static void* sPrepareBattle()
{
	s_battle_banner = 0;
	return (void*)sBattleInit;
}


/*-----------------------------

 State management
-----------------------------*/
static uint16_t s_state_start = 0; /* In milliseconds */
static uint8_t s_state_prev_bkg = 0;


static void* sWait()
{
	/* We need to be sure of show the loading screen for at least 2 seconds in
	   those CPUs that did the loading job unrealistically fast (emulated software).

	   In slower CPUs this timer will overflow if the loading time pass the
	   ~6 seconds mark, making a final wait of ~8 seconds. Not really a problem */
	if (CURRENT_MILLISECONDS < s_state_start + 2000 && CURRENT_MILLISECONDS > s_state_start)
		return (void*)sWait;

	/* Yay, next state! */
	return sPrepareBattle();
}


static void* sLoad()
{
	s_font1 = IntLoadSprite("assets/font1.jvn");
	s_font1a = IntLoadSprite("assets/font1a.jvn");
	s_spr_portraits = IntLoadSprite("assets/ui-ports.jvn");
	s_spr_fx1 = IntLoadSprite("assets/fx1.jvn");
	s_spr_fx2 = IntLoadSprite("assets/fx2.jvn");
	s_spr_items = IntLoadSprite("assets/ui-items.jvn");

	ActorsInitializeSprites();

	return sWait();
}


static void* sInit()
{
	uint8_t bkg = 0;

	s_state_start = CURRENT_MILLISECONDS;

	s_toggle_x = 0;
	s_toggle_l = 0;
	s_toggle_r = 0;
	s_toggle_u = 0;
	s_toggle_d = 0;
	s_toggle_start = 0;

	IntPrintText("# StateBattle\n");
	IntUnloadAll();

	/* Reload minimal assets for the 'loading' screen */
	s_font2 = IntLoadSprite("assets/font2.jvn");

	while (bkg == s_state_prev_bkg)
		bkg = (uint8_t)(Random() % 4);

	switch (bkg)
	{
	case 0: IntLoadBackground("assets/bkg1.raw"); break;
	case 1: IntLoadBackground("assets/bkg2.raw"); break;
	case 2: IntLoadBackground("assets/bkg3.raw"); break;
	case 3: IntLoadBackground("assets/bkg4.raw");
	}

	s_state_prev_bkg = bkg;
	CmdDrawBackground();

	/* Draw screen */
	ActorsInitialize(s_battle_no);
	UiBanner(s_font2, (g_live_enemies > 1) ? "Monsters appear!" : "Monster appears!");

	/* Bye! */
	CmdHalt();
	return (void*)sLoad;
}


void* StatePrepareBattle(uint8_t battle_no)
{
	s_battle_no = battle_no;
	return (void*)sInit;
}
