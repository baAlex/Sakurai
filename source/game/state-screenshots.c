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

 [state-screenshots.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "actor-traits.h"
#include "state.h"
#include "ui.h"
#include "utilities.h"

static uint8_t s_font1;
static uint8_t s_font2;
static uint8_t s_spr_portraits;
static uint8_t s_spr_items;

static uint8_t s_stage_no = 0;


static void* sFrame()
{
	if (INPUT_LEFT == 1)
	{
		if (s_stage_no >= 1)
		{
			s_stage_no -= 1;
			return (void*)StateScreenshots;
		}
	}
	else if (INPUT_RIGHT == 1)
	{
		if (s_stage_no < 255)
		{
			s_stage_no += 1;
			return (void*)StateScreenshots;
		}
	}

	CmdHalt();
	return (void*)sFrame;
}


void sRandomizeActors()
{
	uint8_t i = 0;

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		g_actor[i].idle_timer = (uint8_t)Random();
		g_actor[i].charge_timer = (uint8_t)Random();

		if (i >= ON_SCREEN_HEROES)
		{
			if ((Random() % 100) < 50)
				g_actor[i].state = ACTOR_STATE_CHARGE;

			if ((Random() % 100) < 20)
				g_actor[i].recover_timer = (uint8_t)Random();
		}
	}
}


void* StateScreenshots()
{
	IntPrintText("# StateScreenshots\n");
	IntUnloadAll();

	s_font1 = IntLoadSprite("assets/font1.jvn");
	s_font2 = IntLoadSprite("assets/font2.jvn");
	s_spr_portraits = IntLoadSprite("assets/ui-ports.jvn");
	s_spr_items = IntLoadSprite("assets/ui-items.jvn");

	Seed(307);
	TraitsInitialize();
	ActorsInitialize(0);

	/* Four for Itch.io */
	if (s_stage_no == 0)
	{
		/* Simple one showing the UI, to make it clear that is an RPG */
		ActorsInitialize(23);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets/bkg4.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 95;
		g_actor[ACTOR_SAO].magic = 0;
		g_actor[ACTOR_KURO].health = 100;
		g_actor[ACTOR_KURO].magic = 60;

		g_actor[ACTOR_KURO].state = ACTOR_STATE_CHARGE;
		g_actor[ACTOR_KURO].charge_timer = 170;

		UiPanelAction_static(s_spr_portraits, s_font2, g_actor[ACTOR_SAO].persona, &g_actor[ACTOR_SAO],
		                      &g_actor[ACTOR_KURO]);
		UiPanelAction_dynamic(s_spr_items, s_font1, g_actor[ACTOR_SAO].persona, 0);

		ActorsDraw(1);

		CmdDrawRectanglePrecise(34, 3, g_actor[ACTOR_SAO].x, g_actor[ACTOR_SAO].y, 8); /* Indicate who charges */
	}
	else if (s_stage_no == 1)
	{
		/* Screenshot showcasing the background, Sao is being hurt in a hard battle */
		ActorsInitialize(5);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets/bkg3.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 40;
		g_actor[ACTOR_SAO].magic = 0;
		g_actor[ACTOR_KURO].health = 85;
		g_actor[ACTOR_KURO].magic = 60;

		g_actor[ACTOR_SAO].recover_timer = 40;

		UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw(1);
	}
	else if (s_stage_no == 2)
	{
		/* Another one showing the panel, Kuro healing the party */
		ActorsInitialize(32);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets/bkg2.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 5;
		g_actor[ACTOR_SAO].magic = 60;
		g_actor[ACTOR_KURO].health = 30;
		g_actor[ACTOR_KURO].magic = 20;

		g_actor[ACTOR_SAO].state = ACTOR_STATE_CHARGE;
		g_actor[ACTOR_SAO].charge_timer = 64;

		UiPanelAction_static(s_spr_portraits, s_font2, g_actor[ACTOR_KURO].persona, &g_actor[ACTOR_SAO],
		                      &g_actor[ACTOR_KURO]);
		UiPanelAction_dynamic(s_spr_items, s_font1, g_actor[ACTOR_KURO].persona, 2);

		ActorsDraw(1);

		CmdDrawRectanglePrecise(34, 3, g_actor[ACTOR_KURO].x, g_actor[ACTOR_KURO].y, 8); /* Indicate who charges */
	}
	else if (s_stage_no == 3)
	{
		/* Finally, a typical battle */
		ActorsInitialize(3);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets/bkg1.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 70;
		g_actor[ACTOR_SAO].magic = 20;
		g_actor[ACTOR_KURO].health = 60;
		g_actor[ACTOR_KURO].magic = 80;

		UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw(1);
	}

	/* Two for GitHub */
	else if (s_stage_no == 4)
	{
		/* Showcasing the panel */
		ActorsInitialize(27);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets/bkg3.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 60;
		g_actor[ACTOR_SAO].magic = 100;
		g_actor[ACTOR_KURO].health = 40;
		g_actor[ACTOR_KURO].magic = 0;

		UiPanelAction_static(s_spr_portraits, s_font2, g_actor[ACTOR_SAO].persona, &g_actor[ACTOR_SAO],
		                      &g_actor[ACTOR_KURO]);
		UiPanelAction_dynamic(s_spr_items, s_font1, g_actor[ACTOR_SAO].persona, 3);

		ActorsDraw(1);

		CmdDrawRectanglePrecise(34, 3, g_actor[ACTOR_SAO].x, g_actor[ACTOR_SAO].y, 8); /* Indicate who charges */
	}
	else
	{
		/* And an uninteresting battle */
		ActorsInitialize(4);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets/bkg4.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 80;
		g_actor[ACTOR_SAO].magic = 0;
		g_actor[ACTOR_KURO].health = 100;
		g_actor[ACTOR_KURO].magic = 30;

		UiHUD(s_spr_portraits, s_font2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw(1);
	}

	return sFrame();
}
