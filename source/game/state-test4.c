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

 [state-test4.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "state.h"
#include "ui.h"
#include "utilities.h"


static uint8_t s_battle_no = 0;


static void* sFrame()
{
	/* Re-initialize actors on user demand */
	if (INPUT_LEFT == 1)
	{
		if (s_battle_no >= 1)
		{
			s_battle_no -= 1;
			return (void*)StateTest4; /* Restarts the entire world */
		}
	}
	else if (INPUT_RIGHT == 1)
	{
		if (s_battle_no < 255)
		{
			s_battle_no += 1;
			return (void*)StateTest4; /* Restarts the entire world */
		}
	}

	/* Draw actors */
	ActorsDraw();

	/* Bye! */
	CmdHalt();
	return (void*)sFrame;
}


/*-----------------------------

 Start a new battle
-----------------------------*/

static uint16_t s_loading_start = 0;

static void* sWait()
{
	/* We need to be sure of show the loading screen for at least 1.5 seconds */
	if (CURRENT_MILLISECONDS > s_loading_start + 1500 || CURRENT_MILLISECONDS < s_loading_start)
	{
		CmdDrawRectangle(20 /* 320 px */, 13 /* 208 px */, 0, 0, 64 + (Random() % 10));
		return sFrame();
	}

	return (void*)sWait;
}

static void* sLoad()
{
	uint8_t i = 0;

	for (i = 0; i < ACTORS_NO; i++)
	{
		/* TODO */
	}

	return sWait();
}

void* StateTest4()
{
	uint8_t enemies_no = 0;
	uint8_t i = 0;
	uint16_t text_y = 0;

	IntPrintText("# StateTest4\n");
	IntUnloadAll();

	/* Reload minimal assets for the 'loading' screen */
	IntLoadSprite("assets\\font1.jvn", SPRITE_FONT1);
	IntLoadSprite("assets\\font2.jvn", SPRITE_FONT2);

	switch (Random() % 4)
	{
	case 0: IntLoadBackground("assets\\bkg1.raw"); break;
	case 1: IntLoadBackground("assets\\bkg2.raw"); break;
	case 2: IntLoadBackground("assets\\bkg3.raw"); break;
	case 3: IntLoadBackground("assets\\bkg4.raw");
	}

	/* Draw loading screen */
	CmdDrawBackground();

	enemies_no = ActorsInitialize(s_battle_no);
	CmdDrawText(SPRITE_FONT2, 10, 10, (enemies_no > 1) ? "Monsters appear!" : "Monster appears!");

	text_y = 10;

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state != ACTOR_STATE_DEAD && (g_actor[i].persona->tags & TAG_ENEMY))
		{
			text_y += 10;
			CmdDrawText(SPRITE_FONT1, 10, text_y, g_actor[i].persona->name);
		}
	}

	/* Bye! */
	s_loading_start = CURRENT_MILLISECONDS;

	CmdHalt();
	return sLoad;
}
