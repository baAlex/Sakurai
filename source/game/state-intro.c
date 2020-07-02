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

 [state-intro.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "state.h"
#include "ui.h"
#include "utilities.h"

static uint8_t s_font1;
static uint8_t s_font2;
static uint8_t s_spr_items;

static uint8_t s_dialog = 0;
static uint16_t s_last_updated = 0;


/* Presenting the main characters */
static char* s_intro01_kuro[] = {"Sao, your magical girl license already expired!.",
                                 "The last time we renewed it was ten years ago!...", NULL};
static char* s_intro02_sao[] = {"Come on!.", NULL};
static char* s_intro03_kuro[] = {"You didn't even remember the spells!.", NULL};
static char* s_intro04_sao[] = {"...", NULL};
static char* s_intro05_kuro[] = {"Well...", NULL};


/* Wall text here because even if this prototype is an incomplete mess, at least
I want that the player note this core feature (this broken sentence makes any sense?) */
static char* s_intro06_walltext[] = {"Just note that the combat being turn based, demands",
                                     "special attention to the time of enemies turns, attacks",
                                     "recovery, and of course our own turns.", NULL};
static char* s_intro07_walltext[] = {"Every character has a time meter above his head, so",
                                     "please use them to elaborate a proper strategy.", NULL};
static char* s_intro08_walltext[] = {"And... I don't need to say that some attacks require", "magical points, right?.",
                                     NULL};
static char* s_intro09_sao[] = {"Right", NULL};


/* Breaking the four wall to make really obvious that this is an alpha alpha alpha version */

/* TODO: use the Clarke Europa thing?: « All this game is yours, except runtime errors. » */
/* « Attempt no landing there. Enjoy it together. Play it in peace. » */

static char* s_intro10_kuro[] = {"Oh... and I got a message from the developer:",
                                 "«I'm having fun making this game prototype, hope you", "have fun too».", NULL};
static char* s_intro11_kuro[] = {"Ends with: «Further details following...».", NULL};
static char* s_intro12_sao[] = {"???", NULL};


static void sCleanScreen()
{
	CmdDrawRectangle(20 /* 320 px */, 13 /* 208 px */, 0, 0, 64);
}


static uint16_t s_title_start = 0; /* In milliseconds */

static void* sTitle()
{
	if (CURRENT_MILLISECONDS < s_title_start + 3000 && CURRENT_MILLISECONDS > s_title_start)
		return (void*)sTitle;

	CmdDrawText(s_font2, 100, 140, "Press a key to continue...");
	CmdHalt();

	if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1 || INPUT_RIGHT == 1 || INPUT_DOWN == 1 ||
	    INPUT_LEFT == 1 || INPUT_UP == 1)
	{
		Seed(CURRENT_MILLISECONDS);
		return StateBattle();
	}

	return (void*)sTitle;
}

static void* sFrame();

static void sResume()
{
	sCleanScreen();
	s_last_updated = CURRENT_MILLISECONDS;

	return sFrame();
}

static void* sFrame()
{
	if (INPUT_START == 1)
		return SetStatePause(s_font1, s_font2, s_spr_items, sResume);

	if (INPUT_LEFT == 1 || INPUT_UP == 1)
	{
		sCleanScreen();
		s_last_updated = CURRENT_MILLISECONDS;

		if (s_dialog > 0)
			s_dialog -= 1;
	}

	if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1 || INPUT_RIGHT == 1 || INPUT_DOWN == 1)
	{
		sCleanScreen();
		s_last_updated = CURRENT_MILLISECONDS;

		s_dialog += 1;
	}

	switch (s_dialog)
	{
	case 0: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro01_kuro); break;
	case 1: UiDialog(s_font2, s_last_updated, "[Sayori]", s_intro02_sao); break;
	case 2: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro03_kuro); break;
	case 3: UiDialog(s_font2, s_last_updated, "[Sayori]", s_intro04_sao); break;
	case 4: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro05_kuro); break;

	case 5: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro06_walltext); break;
	case 6: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro07_walltext); break;
	case 7: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro08_walltext); break;
	case 8: UiDialog(s_font2, s_last_updated, "[Sayori]", s_intro09_sao); break;

	case 9: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro10_kuro); break;
	case 10: UiDialog(s_font2, s_last_updated, "[Kuro]", s_intro11_kuro); break;
	case 11: UiDialog(s_font2, s_last_updated, "[Sayori]", s_intro12_sao); break;

	default: break;
	}

	if (s_dialog == 12)
	{
		s_title_start = CURRENT_MILLISECONDS;
		CmdDrawBackground();
		CmdHalt();
		return (void*)sTitle;
	}

	CmdHalt();
	return (void*)sFrame;
}


void* StateIntro()
{
	IntPrintText("# StateIntro\n");
	IntUnloadAll();

	IntLoadBackground("assets\\title.raw");
	s_font1 = IntLoadSprite("assets\\font1.jvn");
	s_font2 = IntLoadSprite("assets\\font2.jvn");
	s_spr_items = IntLoadSprite("assets\\ui-items.jvn");

	sCleanScreen();
	s_dialog = 0;
	s_last_updated = CURRENT_MILLISECONDS;

	return sFrame();
}
