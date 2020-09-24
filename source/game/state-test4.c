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
#include "utilities.h"

static uint8_t s_toggle_x = 0;
static uint8_t s_toggle_y = 0;
static uint8_t s_toggle_l = 0;
static uint8_t s_toggle_r = 0;
static uint8_t s_toggle_u = 0;
static uint8_t s_toggle_d = 0;
static uint8_t s_toggle_start = 0;
static uint8_t s_toggle_select = 0;


static void* sFrame()
{
	if ((CURRENT_FRAME % 48) == 0) /* Every 2 seconds */
		CmdDrawRectangle(20 /* 320 px */, 13 /* 208 px */, 0, 0, 64 + (Random() % 10));

	CmdDrawRectangle(1, 1, (18 * 1) + 9, (18 * 4), 7 + KeyToggle(INPUT_X, &s_toggle_x));
	CmdDrawRectangle(1, 1, (18 * 2) + 9, (18 * 4), 7 + KeyToggle(INPUT_Y, &s_toggle_y));

	CmdDrawRectangle(1, 1, (18 * 5), (18 * 4), 7 + KeyRepeat(INPUT_PAD_L, &s_toggle_l));
	CmdDrawRectangle(1, 1, (18 * 6), (18 * 4), 7 + KeyRepeat(INPUT_PAD_D, &s_toggle_d));
	CmdDrawRectangle(1, 1, (18 * 6), (18 * 3), 7 + KeyRepeat(INPUT_PAD_U, &s_toggle_u));
	CmdDrawRectangle(1, 1, (18 * 7), (18 * 4), 7 + KeyRepeat(INPUT_PAD_R, &s_toggle_r));

	CmdDrawRectangle(1, 1, (18 * 1), (18 * 2), 7 + KeyToggle(INPUT_START, &s_toggle_start));
	CmdDrawRectangle(1, 1, (18 * 2), (18 * 2), 7 + KeyToggle(INPUT_SELECT, &s_toggle_select));

	CmdHalt();
	return (void*)sFrame;
}


void* StateTest4()
{
	IntPrintText("# StateTest4\n");
	return sFrame();
}
