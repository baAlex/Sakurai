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

static struct Actor s_actor1;
static struct Actor s_actor2;


static void* sFrame()
{
	uint16_t x = 0;

	/* Random background plus a menu, every 4 seconds */
	if ((CURRENT_FRAME % 96) == 0)
	{
		switch (Random() % 4)
		{
		case 0: IntLoadBackground("assets\\bkg1.raw"); break;
		case 1: IntLoadBackground("assets\\bkg2.raw"); break;
		case 2: IntLoadBackground("assets\\bkg3.raw"); break;
		case 3: IntLoadBackground("assets\\bkg4.raw");
		}

		CmdDrawBackground();

		/*HudDraw(SPRITE_PORTRAITS, SPRITE_FONT2, &s_actor1, &s_actor2);*/
		MenuActionDraw_static(SPRITE_PORTRAITS, SPRITE_FONT2, &s_actor1, &s_actor1, &s_actor2);
	}

	/* Sprite oscillating in position */
	s_actor1.phase += 1;
	x = (uint16_t)((int16_t)s_actor1.x + ((int16_t)Sin(s_actor1.phase) >> 2));

	CmdDrawRectangleBkg(4 /* 64 px */, 6 /* 96 px*/, x, s_actor1.y);
	CmdDrawSprite(SPRITE_SAO, x, s_actor1.y, s_actor1.phase >> 4);

	/* Pseudo-pseudo-random menu selection */
	MenuActionDraw_dynamic(SPRITE_ARROW, SPRITE_FONT1, &s_actor1, (uint8_t)(x >> 4) % 6);

	/* Bye! */
	CmdHalt();
	return (void*)sFrame;
}


void* StateTest3()
{
	IntPrintText("# StateTest3\n");

	IntLoadSprite("assets\\ui-ports.jvn", SPRITE_PORTRAITS);
	IntLoadSprite("assets\\font1.jvn", SPRITE_FONT1);
	IntLoadSprite("assets\\font2.jvn", SPRITE_FONT2);
	IntLoadSprite("assets\\sayori.jvn", SPRITE_SAO);
	IntLoadSprite("assets\\ui-items.jvn", SPRITE_ARROW);

	/* Minimum necessary fields to draw the HUD... */
	s_actor1.health = 100;
	s_actor1.magic = 80;
	s_actor2.health = 75;
	s_actor2.magic = 50;

	/* ...and to draw the Action menu */
	s_actor1.persona = &g_persona[PERSONA_SAO];
	s_actor2.persona = &g_persona[PERSONA_KURO];

	/* Finally, fields to draw the actor */
	s_actor1.x = 160 - 32;
	s_actor1.y = 100 - 32;
	s_actor1.phase = 0;

	return sFrame();
}
