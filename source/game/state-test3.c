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

 [state-test3.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "state.h"
#include "ui.h"
#include "utilities.h"
#include "actor-traits.h"

static uint8_t s_font1;
static uint8_t s_font2;
static uint8_t s_spr_portraits;
static uint8_t s_spr_items;
static uint8_t s_spr_sao;

static struct Actor s_actor1;
static struct Actor s_actor2;


static void* sFrame()
{
	uint16_t x = 0;

	if ((CURRENT_FRAME % 96) == 0) /* Every 4 seconds */
	{
		switch (Random() % 4)
		{
		case 0: IntLoadBackground("assets/bkg1.raw"); break;
		case 1: IntLoadBackground("assets/bkg2.raw"); break;
		case 2: IntLoadBackground("assets/bkg3.raw"); break;
		case 3: IntLoadBackground("assets/bkg4.raw");
		}

		CmdDrawBackground();

		UiPanelAction_static(s_spr_portraits, s_font2, s_actor1.persona, &s_actor1, &s_actor2);
	}

	/* Sprite oscillating in position */
	s_actor1.phase += 1;
	x = (uint16_t)((int16_t)s_actor1.x + ((int16_t)Sin(s_actor1.phase) >> 2));

	CmdDrawRectangleBkg(4 /* 64 px */, 6 /* 96 px*/, x, s_actor1.y);
	CmdDrawSprite(s_spr_sao, x, s_actor1.y, s_actor1.phase >> 4);

	/* Ui elements */
	UiPanelAction_dynamic(s_spr_items, s_font1, s_actor1.persona, (uint8_t)(x >> 4) % 6);
	UiBanner(s_font2, "Victory");

	/* Bye! */
	CmdHalt();
	return (void*)sFrame;
}


void* StateTest3()
{
	IntPrintText("# StateTest3\n");

	s_font1 = IntLoadSprite("assets/font1.jvn");
	s_font2 = IntLoadSprite("assets/font2.jvn");
	s_spr_portraits = IntLoadSprite("assets/ui-ports.jvn");
	s_spr_items = IntLoadSprite("assets/ui-items.jvn");
	s_spr_sao = IntLoadSprite("assets/sayori.jvn");

	TraitsInitialize();

	/* Minimum necessary fields to draw the HUD... */
	s_actor1.health = 100;
	s_actor1.magic = 80;
	s_actor2.health = 75;
	s_actor2.magic = 50;

	/* ...and to draw the Action menu */
	s_actor1.persona = &g_heroes[PERSONALITY_SAO];
	s_actor2.persona = &g_heroes[PERSONALITY_KURO];

	/* Finally, fields to draw the actor */
	s_actor1.x = 160 - 32;
	s_actor1.y = 100 - 32;
	s_actor1.phase = 0;

	return sFrame();
}
