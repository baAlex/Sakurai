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

 [state-test2.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "state.h"
#include "utilities.h"

static char s_buffer[4] = {0, 0, 0, 0};


static void* sFrame()
{
	if ((CURRENT_FRAME % 96) == 0) /* Every 4 seconds */
	{
		switch (Random() % 4)
		{
		case 0: IntLoadBackground("assets\\bkg1.raw"); break;
		case 1: IntLoadBackground("assets\\bkg2.raw"); break;
		case 2: IntLoadBackground("assets\\bkg3.raw"); break;
		case 3: IntLoadBackground("assets\\bkg4.raw");
		}

		CmdDrawBackground();
		CmdDrawSprite(SPRITE_SAO, 160 - 32, 100 - 32, 0);
	}

	CmdDrawRectangleBkg(2 /* 32 px */, 1 /* 16 px*/, 10, 0);
	CmdDrawText(SPRITE_FONT2, 10, 0, NumberToString((uint8_t)CURRENT_FRAME, s_buffer));

	CmdHalt();
	return (void*)sFrame;
}


void* StateTest2()
{
	IntLoadSprite("assets\\font2.jvn", SPRITE_FONT2);
	IntLoadSprite("assets\\sayori.jvn", SPRITE_SAO);

	return sFrame();
}
