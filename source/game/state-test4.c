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

static char s_buffer[4] = {0, 0, 0, 0};
static uint8_t s_battle_no = 0;


static void* sFrame()
{
	/* Re-initialize actors on user demand */
	if (INPUT_LEFT == 1)
	{
		if (s_battle_no >= 1)
		{
			s_battle_no -= 1;

			ActorsInitialize(s_battle_no);
			goto clean_redraw;
		}
	}
	else if (INPUT_RIGHT == 1)
	{
		if (s_battle_no < 255)
			s_battle_no += 1;

		ActorsInitialize(s_battle_no);
		goto clean_redraw;
	}

	/* Random background, every 4 seconds */
	if ((CURRENT_FRAME % 96) == 0)
	{
	clean_redraw:
		switch (Random() % 4)
		{
		case 0: IntLoadBackground("assets\\bkg1.raw"); break;
		case 1: IntLoadBackground("assets\\bkg2.raw"); break;
		case 2: IntLoadBackground("assets\\bkg3.raw"); break;
		case 3: IntLoadBackground("assets\\bkg4.raw");
		}

		CmdDrawBackground();

		CmdDrawText(SPRITE_FONT1, 10, 0, "Battle number:");
		CmdDrawText(SPRITE_FONT1, 70, 0, NumberToString(s_battle_no, s_buffer));

		/*HudDraw(SPRITE_PORTRAITS, SPRITE_FONT2, &s_actor1, &s_actor2);*/
	}

	ActorsDraw();

	/* Bye! */
	CmdHalt();
	return (void*)sFrame;
}


void* StateTest4()
{
	IntPrintText("# StateTest4\n");

	IntLoadSprite("assets\\ui-ports.jvn", SPRITE_PORTRAITS);
	IntLoadSprite("assets\\font1.jvn", SPRITE_FONT1);
	IntLoadSprite("assets\\font2.jvn", SPRITE_FONT2);

	IntLoadSprite("assets\\sprite1.jvn", 20);
	ActorsInitialize(s_battle_no);

	return sFrame();
}
