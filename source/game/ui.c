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

 [ui.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "actor.h"
#include "attacks.h"
#include "utilities.h"

#define UI_WIDTH 19 /* 304 px */
#define UI_HEIGHT 3 /* 48 px */
#define UI_X 8
#define UI_Y 6 /* Not 8 because the 1.2 ratio */

#define UI_BACK_COLOR 15
#define UI_LINE_SPACE 14 /* For Font2 */

#define UI_PADDING_Y 3

#define UI_COLUMN_1_X 8
#define UI_COLUMN_2_X 110
#define UI_COLUMN_3_X (UI_COLUMN_2_X + 60)
#define UI_COLUMN_4_X (UI_COLUMN_3_X + 60)


static uint8_t s_buffer1[4] = {0, 0, 0, 0};
static uint8_t s_buffer2[4] = {0, 0, 0, 0};
static uint8_t s_buffer3[4] = {0, 0, 0, 0};
static uint8_t s_buffer4[4] = {0, 0, 0, 0};


static uint16_t sToDecimalString(uint8_t no, uint8_t* out)
{
	/* https://stackoverflow.com/a/32871108 */
	uint8_t* c = out + 3;

	do
	{
		c -= 1;
		*c = 0x30 + (no % 10);
		no /= 10;

	} while (no != 0);

	return (uint16_t)(c);
}


static sDrawPortraits(uint8_t clean, uint8_t portraits_slot)
{
	char* c;

	/* Background */
	if (clean == 1)
		CmdDrawRectangleBkg(6 /* 96 px */, 3 /* 48 px */, UI_X + 2, UI_Y + 2);

	/* Portraits */
	CmdDrawSprite(portraits_slot, UI_X + 2, UI_Y + 2, 0);

	/* Kuro stats */
	c = (char*)sToDecimalString(g_actor[0].health, s_buffer1);
	CmdDrawText(21, UI_X + 43, UI_Y + 32, c);

	c = (char*)sToDecimalString(g_actor[0].magic, s_buffer2);
	CmdDrawText(21, UI_X + 43 + 28, UI_Y + 32, c);

	/* Sayori stats */
	c = (char*)sToDecimalString(g_actor[1].health, s_buffer3);
	CmdDrawText(21, UI_X + 43, UI_Y + 9, c);

	c = (char*)sToDecimalString(g_actor[1].magic, s_buffer4);
	CmdDrawText(21, UI_X + 43 + 28, UI_Y + 9, c);
}


void DrawHUD(uint8_t portraits_slot)
{
	sDrawPortraits(1, portraits_slot);
}


void DrawActionUI_static(uint8_t portraits_slot, uint8_t actor_index)
{
	struct Actor* actor = &g_actor[actor_index];

	/* UI background */
	CmdDrawRectangle(UI_WIDTH, UI_HEIGHT, UI_X, UI_Y, UI_BACK_COLOR);

	/* Portraits */
	sDrawPortraits(0, portraits_slot);

	/* Hero name */
	CmdDrawText(21, UI_X + UI_COLUMN_2_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, g_persona[actor->type].name);

	/* Common actions */
	CmdDrawText(21, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y, "Attack");
	CmdDrawText(21, UI_X + UI_COLUMN_4_X, UI_Y + UI_PADDING_Y, "A. Combined");
	CmdDrawText(21, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y + (UI_LINE_SPACE << 1), "Hold");

	/* Kuro actions */
	if (actor_index == 0)
	{
		CmdDrawText(21, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Heal");
		CmdDrawText(21, UI_X + UI_COLUMN_4_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Meditate");
	}

	/* Sayori */
	else
	{
		CmdDrawText(21, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Shock");
		CmdDrawText(21, UI_X + UI_COLUMN_4_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Thunder");
	}
}


static uint8_t DrawActionUI_prev_selection = 255;

uint8_t DrawActionUI_dynamic(uint8_t selection, uint8_t item_slot, uint8_t actor_index)
{

	/* Background, two columns */
	CmdDrawRectangle(1 /* 16 px */, UI_HEIGHT, UI_X + UI_COLUMN_3_X - 16, UI_Y, UI_BACK_COLOR);
	CmdDrawRectangle(1 /* 16 px */, UI_HEIGHT, UI_X + UI_COLUMN_4_X - 16, UI_Y, UI_BACK_COLOR);

	/* Selection arrow */
	if (selection > 128)
		selection = 0;
	else if (selection > 4)
		selection = 4;

	if ((selection % 2) == 1)
		CmdDrawSprite(item_slot, UI_X + UI_COLUMN_4_X - 16, UI_Y + UI_PADDING_Y + UI_LINE_SPACE * (selection >> 1),
		              (CURRENT_FRAME >> 2) % 2);
	else
		CmdDrawSprite(item_slot, UI_X + UI_COLUMN_3_X - 16, UI_Y + UI_PADDING_Y + UI_LINE_SPACE * (selection >> 1),
		              (CURRENT_FRAME >> 2) % 2);

	/* Draw a tip at the bottom of the screen */
	if (selection != DrawActionUI_prev_selection)
	{
		DrawActionUI_prev_selection = selection;

		/* Background */
		CmdDrawRectangle(20 /* 320 */, 1 /* 16 px */, 0, 200 - 16, UI_BACK_COLOR);

		/* Text */
		if (selection == 0)
			CmdDrawText(20, 8, 200 - 16, "Simple attack.");
		else if (selection == 1)
			CmdDrawText(20, 8, 200 - 16, "Combined attack, uses 20 MP.");
		else if (selection == 4)
			CmdDrawText(20, 8, 200 - 16, "Hold position, mitigates damage from imminent attack.");

		if (actor_index == 0)
		{
			if (selection == 2)
				CmdDrawText(20, 8, 200 - 16, "Restores party HP.");
			else if (selection == 3)
				CmdDrawText(20, 8, 200 - 16, "Restores party MP.");
		}
		else
		{
			if (selection == 2)
				CmdDrawText(20, 8, 200 - 16, "Immobilizes target, uses 30 MP.");
			else if (selection == 3)
				CmdDrawText(20, 8, 200 - 16, "Desintegrates target, uses 60 MP.");
		}
	}

	return selection;
}


void CleanUI()
{
	CmdDrawRectangleBkg(UI_WIDTH, UI_HEIGHT, UI_X, UI_Y);
}


void DrawTargetUI_static(uint8_t portraits_slot)
{
	/* UI background */
	CmdDrawRectangle(UI_WIDTH, UI_HEIGHT, UI_X, UI_Y, UI_BACK_COLOR);

	/* Portraits */
	sDrawPortraits(0, portraits_slot);

	/* Instruction */
	CmdDrawText(20, UI_X + UI_COLUMN_2_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Select your target.");
}


static uint8_t DrawTargetUI_prev_selection = 255;

uint8_t DrawTargetUI_dynamic(uint8_t selection, uint8_t item_slot)
{
	uint8_t i = 0;

	/* Black arrow, to clean previous frame */
	for (i = (HEROES_NO); i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		CmdDrawSprite(item_slot, g_info[i].base_x, g_info[i].base_y + 40, 2);
	}

	/* Active arrow */
	if (selection == 0)
		selection = HEROES_NO;

	while (g_actor[selection].state == ACTOR_STATE_DEAD)
	{
		if (selection > DrawTargetUI_prev_selection)
			selection += 1;
		else
			selection -= 1;
	}

	if (selection < HEROES_NO)
	{
		selection = HEROES_NO;
		while (g_actor[selection].state == ACTOR_STATE_DEAD)
			selection += 1;
	}
	else if (selection >= ACTORS_NO)
	{
		selection = ACTORS_NO - 1;
		while (g_actor[selection].state == ACTOR_STATE_DEAD)
			selection -= 1;
	}

	CmdDrawSprite(item_slot, g_info[selection].base_x, g_info[selection].base_y + 40, (CURRENT_FRAME >> 2) % 2);

	DrawTargetUI_prev_selection = selection;
	return selection;
}
