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

 [intro.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "game.h"
#include "engine.h"
#include "utilities.h"

static uint16_t Intro_frame = 0;
static uint8_t Intro_line = 0;

/* My sincere try on provide some story background
(and showcase my text rendering, of course!) */

static char* Intro_text[] = {

    "[Kuro]",
    "Sao, your magical girl license already expired!.",
    "The last time we renewed it was ten years ago!...",
    0x00,

    "[Sayori]",
    "Come on!.",
    0x00,
    0x00,

    "[Kuro]",
    "You didn't even remember the spells!.",
    0x00,
    0x00,

    "[Sayori]",
    "...",
    0x00,
    0x00,

    "[Kuro]",
    "Well...",
    0x00,
    0x00,

    /* Wall text here because even if this prototype is an incomplete mess,
    at least I want that the player note this core feature
    (this broken sentence makes any sense?) */

    "[Kuro]",
    "Just note that the combat being turn based, demands",
    "special attention to the time of enemies turns, attacks",
    "recovery, and of course our own turns.",

    "[Kuro]",
    "Every character has a time meter above his head, so",
    "please use them to elaborate a proper strategy.",
    0x00,

    "[Kuro]",
    "And... I don't need to say that some attacks require",
    "magical points, right?.",
    0x00,

    "[Sayori]",
    "Right.",
    0x00,
    0x00,

    /* Breaking the four wall to make really evident that this
    is an alpha alpha alpha version */

    "[Kuro]",
    "Oh... and I got a message from the developer:",
    "«I'm having fun making this game prototype, hope you",
    "have fun too».",

    "[Kuro]",
    "Ends with: «Further details following...».", /* FIXME, "Sordid" as the Bowie quoute?.... */
    0x00, /* ... or the Clarke Europa thing?: "All this game is yours, except runtime errors." */
    0x00, /* Attempt no landing there. Enjoy it together. Play it in peace */

    "[Sayori]",
    "???",
    0x00,
    0x00,
};


void* Title()
{
	Intro_frame += 1;

	if (Intro_frame > 72)
	{
		CmdDrawText(21, 100, 140, "Press a key to continue...");

		if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1 || INPUT_LEFT == 1 ||
		    INPUT_RIGHT == 1 || INPUT_UP == 1 || INPUT_DOWN == 1)
		{
			CmdHalt();
			return GameStart;
		}
	}

	CmdHalt();
	return Title;
}


void* Intro()
{
	void* next_state = Intro;

	if (Intro_line % 4 == 3)
	{
		if (INPUT_LEFT == 1 || INPUT_UP == 1)
		{
			if (Intro_line >= 7)
				Intro_line -= 7;
		}

		if (INPUT_X == 1 || INPUT_Y == 1 || INPUT_START == 1 || INPUT_SELECT == 1 || INPUT_RIGHT == 1 ||
		    INPUT_DOWN == 1)
		{
			Intro_frame = 0;
			Intro_line += 1;
		}

		if (Intro_line > 48)
		{
			CmdDrawBackground();
			Intro_frame = 0;
			next_state = Title;
			goto bye;
		}
	}

	if ((Intro_frame % 4) == 0)
	{
		if ((Intro_line % 4) == 0)
			CmdDrawRectangle(20 /* 320 px */, 13 /* 208 px */, 0, 0, 64);

		CmdDrawText(21, 12, 140 + (12 * (Intro_line % 4)), Intro_text[Intro_line]);

		if ((Intro_line + 1) % 4 != 0)
			Intro_line += 1;
	}

bye:
	/* Bye! */
	Intro_frame += 1;

	CmdHalt();
	return next_state;
}


void* IntroLoad()
{
	IntLoadBackground("assets\\title.raw");
	IntLoadSprite("assets\\font2.jvn", 21);

	Intro_frame = 0;
	return Intro;
}


void* IntroStart()
{
	CmdDrawRectangle(20 /* 320 px */, 13 /* 208 px */, 0, 0, 64);
	return IntroLoad;
}
