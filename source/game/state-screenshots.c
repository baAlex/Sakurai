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

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		g_actor[i].idle_timer = (uint8_t)Random();
		g_actor[i].charge_timer = (uint8_t)Random();

		if (i >= HEROES_NO)
		{
			if ((Random() % 100) < 60)
				g_actor[i].state = ACTOR_STATE_CHARGE;

			if ((Random() % 100) < 40)
				g_actor[i].recover_timer = (uint8_t)Random();
		}
	}
}

void* StateScreenshots()
{
	IntPrintText("# StateScreenshots\n");
	IntUnloadAll();

	IntLoadSprite("assets\\font1.jvn", SPRITE_FONT1);
	IntLoadSprite("assets\\font2.jvn", SPRITE_FONT2);

	IntLoadSprite("assets\\ui-ports.jvn", SPRITE_PORTRAITS);
	IntLoadSprite("assets\\ui-items.jvn", SPRITE_ARROW);

	Seed(14850);
	TraitsInitialize();
	ActorsInitialize(0);

	/* Four for Itch.io */
	if (s_stage_no == 0)
	{
		/* Simple one showing the UI, to make clear that is an RPG */
		ActorsInitialize(23);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets\\bkg4.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 95;
		g_actor[ACTOR_SAO].magic = 0;
		g_actor[ACTOR_KURO].health = 100;
		g_actor[ACTOR_KURO].magic = 60;

		g_actor[ACTOR_KURO].state = ACTOR_STATE_CHARGE;
		g_actor[ACTOR_KURO].charge_timer = 170;

		MenuActionDraw_static(SPRITE_PORTRAITS, SPRITE_FONT2, g_actor[ACTOR_SAO].persona, &g_actor[ACTOR_SAO],
		                      &g_actor[ACTOR_KURO]);
		MenuActionDraw_dynamic(SPRITE_ARROW, SPRITE_FONT1, g_actor[ACTOR_SAO].persona, 0);

		ActorsDraw();
	}
	else if (s_stage_no == 1)
	{
		/* Screenshot showcasing the background, Sao is being hurt in a hard battle */
		ActorsInitialize(5);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets\\bkg3.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 40;
		g_actor[ACTOR_SAO].magic = 0;
		g_actor[ACTOR_KURO].health = 85;
		g_actor[ACTOR_KURO].magic = 60;

		g_actor[ACTOR_SAO].recover_timer = 40;

		HudDraw(SPRITE_PORTRAITS, SPRITE_FONT2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw();
	}
	else if (s_stage_no == 2)
	{
		/* Another one showing the menu, Kuro healing the party */
		ActorsInitialize(32);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets\\bkg2.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 5;
		g_actor[ACTOR_SAO].magic = 60;
		g_actor[ACTOR_KURO].health = 30;
		g_actor[ACTOR_KURO].magic = 20;

		g_actor[ACTOR_SAO].state = ACTOR_STATE_CHARGE;
		g_actor[ACTOR_SAO].charge_timer = 64;

		MenuActionDraw_static(SPRITE_PORTRAITS, SPRITE_FONT2, g_actor[ACTOR_KURO].persona, &g_actor[ACTOR_SAO],
		                      &g_actor[ACTOR_KURO]);
		MenuActionDraw_dynamic(SPRITE_ARROW, SPRITE_FONT1, g_actor[ACTOR_KURO].persona, 2);

		ActorsDraw();
	}
	else if (s_stage_no == 3)
	{
		/* Finally, a tipical battle */
		ActorsInitialize(3);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets\\bkg1.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 70;
		g_actor[ACTOR_SAO].magic = 20;
		g_actor[ACTOR_KURO].health = 60;
		g_actor[ACTOR_KURO].magic = 80;

		HudDraw(SPRITE_PORTRAITS, SPRITE_FONT2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw();
	}

	/* Two for GitHub */
	else if (s_stage_no == 4)
	{
		/* Showcasing the menu */
		ActorsInitialize(27);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets\\bkg3.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 60;
		g_actor[ACTOR_SAO].magic = 100;
		g_actor[ACTOR_KURO].health = 40;
		g_actor[ACTOR_KURO].magic = 0;

		MenuActionDraw_static(SPRITE_PORTRAITS, SPRITE_FONT2, g_actor[ACTOR_SAO].persona, &g_actor[ACTOR_SAO],
		                      &g_actor[ACTOR_KURO]);
		MenuActionDraw_dynamic(SPRITE_ARROW, SPRITE_FONT1, g_actor[ACTOR_SAO].persona, 3);

		ActorsDraw();
	}
	else
	{
		/* And an uninteresing battle */
		ActorsInitialize(4);
		ActorsInitializeSprites();
		sRandomizeActors();

		IntLoadBackground("assets\\bkg4.raw");
		CmdDrawBackground();

		g_actor[ACTOR_SAO].health = 80;
		g_actor[ACTOR_SAO].magic = 0;
		g_actor[ACTOR_KURO].health = 100;
		g_actor[ACTOR_KURO].magic = 30;

		HudDraw(SPRITE_PORTRAITS, SPRITE_FONT2, &g_actor[ACTOR_SAO], &g_actor[ACTOR_KURO]);

		ActorsDraw();
	}

	return sFrame();
}
