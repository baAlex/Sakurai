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

#include "actor-traits.h"
#include "state.h"
#include "ui.h"
#include "utilities.h"

static uint8_t s_font1;
static uint8_t s_font2;
static uint8_t s_spr_items;
static void* s_resume_to;

static uint8_t s_selection = 0;


static void* sPauseFrame()
{
	if (INPUT_LEFT == 1 || INPUT_UP == 1)
		s_selection -= 1;
	if (INPUT_RIGHT == 1 || INPUT_DOWN == 1)
		s_selection += 1;

	s_selection = UiMenuPause_dynamic(s_spr_items, s_selection);

	if (INPUT_START == 1 && s_resume_to != NULL)
	{
		CmdHalt();
		return s_resume_to;
	}

	if (INPUT_X == 1 || INPUT_Y == 1)
	{
		if (s_selection == 0 && s_resume_to != NULL)
		{
			CmdHalt();
			return s_resume_to;
		}
		else if (s_selection == 1)
		{
			CmdHalt();
			return StateIntro;
		}
		else if (s_selection == 2)
			IntExitRequest();
	}

	CmdHalt();
	return (void*)sPauseFrame;
}


void* SetStatePause(uint8_t spr_font1, uint8_t spr_font2, uint8_t spr_items, void* resume_to)
{
	s_font1 = spr_font1;
	s_font2 = spr_font2;
	s_spr_items = spr_items;
	s_resume_to = resume_to;
	s_selection = 0;

	UiMenuPause_static(s_font1, s_font2);

	return (void*)sPauseFrame;
}


void* StateTest4()
{
	IntPrintText("# StateTest4\n");
	IntLoadBackground("assets\\bkg4.raw");

	return SetStatePause(IntLoadSprite("assets\\font1.jvn"), IntLoadSprite("assets\\font2.jvn"),
	                     IntLoadSprite("assets\\ui-items.jvn"), StateBattle);
}
