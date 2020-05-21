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

* TODO: remove reads to 'g_info' and 'g_actor'
* Plus other TODOs below :)
-----------------------------*/

#include "ui.h"
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


/*-----------------------------

 sPortraitsDraw()
-----------------------------*/
static char s_buffer1[4] = {0, 0, 0, 0};
static char s_buffer2[4] = {0, 0, 0, 0};
static char s_buffer3[4] = {0, 0, 0, 0};
static char s_buffer4[4] = {0, 0, 0, 0};

static void sPortraitsDraw(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor_a, struct Actor* actor_b)
{
	char* c;

	CmdDrawSprite(portraits_sprite, UI_X + 2, UI_Y + 2, 0);

	/* Actor A */
	c = NumberToString(actor_a->health, s_buffer3);
	CmdDrawText(font_sprite, UI_X + 43, UI_Y + 9, c);

	c = NumberToString(actor_a->magic, s_buffer4);
	CmdDrawText(font_sprite, UI_X + 43 + 28, UI_Y + 9, c);

	/* Actor B */
	c = NumberToString(actor_b->health, s_buffer1);
	CmdDrawText(font_sprite, UI_X + 43, UI_Y + 32, c);

	c = NumberToString(actor_b->magic, s_buffer2);
	CmdDrawText(font_sprite, UI_X + 43 + 28, UI_Y + 32, c);
}


/*-----------------------------

 Hud
-----------------------------*/
void HudDraw(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor_a, struct Actor* actor_b)
{
	CmdDrawRectangleBkg(6 /* 96 px */, 3 /* 48 px */, UI_X + 2, UI_Y + 2);
	sPortraitsDraw(portraits_sprite, font_sprite, actor_a, actor_b);
}


/*-----------------------------

 Action menu
------------------------------*/
static uint8_t s_prev_action_selection = 255;

void MenuActionDraw_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor, struct Actor* hud_a, struct Actor* hud_b)
{
	/*
	TODO, in the future the actors themself should have an entry: 'actor->action[]'
	      so I can delete all the following harcoded values.
	*/

	CmdDrawRectangle(UI_WIDTH, UI_HEIGHT, UI_X, UI_Y, UI_BACK_COLOR);
	sPortraitsDraw(portraits_sprite, font_sprite, hud_a, hud_b);

	/* Hero name */
	CmdDrawText(font_sprite, UI_X + UI_COLUMN_2_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, actor->persona->name);

	/* Common actions */
	CmdDrawText(font_sprite, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y, "Attack");
	CmdDrawText(font_sprite, UI_X + UI_COLUMN_4_X, UI_Y + UI_PADDING_Y, "A. Combined");
	CmdDrawText(font_sprite, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y + (UI_LINE_SPACE << 1), "Hold");

	/* Kuro actions */
	if (actor->persona == &g_persona[PERSONA_KURO])
	{
		CmdDrawText(font_sprite, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Heal");
		CmdDrawText(font_sprite, UI_X + UI_COLUMN_4_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Meditate");
	}

	/* Sayori */
	else
	{
		CmdDrawText(font_sprite, UI_X + UI_COLUMN_3_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Shock");
		CmdDrawText(font_sprite, UI_X + UI_COLUMN_4_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Thunder");
	}
}

uint8_t MenuActionDraw_dynamic(uint8_t arrow_sprite, uint8_t font_sprite, struct Actor* actor, uint8_t selection)
{
	/*
	TODO, same as before, every action should be in companion with a tip.
	*/

	CmdDrawRectangle(1 /* 16 px */, UI_HEIGHT, UI_X + UI_COLUMN_3_X - 16, UI_Y, UI_BACK_COLOR);
	CmdDrawRectangle(1 /* 16 px */, UI_HEIGHT, UI_X + UI_COLUMN_4_X - 16, UI_Y, UI_BACK_COLOR);

	/* Selection arrow */
	if (selection > 128)
		selection = 0;
	else if (selection > 4)
		selection = 4;

	if ((selection % 2) == 1)
		CmdDrawSprite(arrow_sprite, UI_X + UI_COLUMN_4_X - 16, UI_Y + UI_PADDING_Y + UI_LINE_SPACE * (selection >> 1),
		              (CURRENT_FRAME >> 2) % 2);
	else
		CmdDrawSprite(arrow_sprite, UI_X + UI_COLUMN_3_X - 16, UI_Y + UI_PADDING_Y + UI_LINE_SPACE * (selection >> 1),
		              (CURRENT_FRAME >> 2) % 2);

	/* Draw a tip at the bottom of the screen */
	if (selection != s_prev_action_selection)
	{
		CmdDrawRectangle(20 /* 320 */, 1 /* 16 px */, 0, 200 - 16, UI_BACK_COLOR);
		s_prev_action_selection = selection;

		if (selection == 0)
			CmdDrawText(font_sprite, 8, 200 - 16, "Simple attack.");
		else if (selection == 1)
			CmdDrawText(font_sprite, 8, 200 - 16, "Combined attack, uses 20 MP.");
		else if (selection == 4)
			CmdDrawText(font_sprite, 8, 200 - 16, "Hold position, mitigates damage from imminent attack.");

		if (actor->persona == &g_persona[PERSONA_KURO])
		{
			if (selection == 2)
				CmdDrawText(font_sprite, 8, 200 - 16, "Restores party HP.");
			else if (selection == 3)
				CmdDrawText(font_sprite, 8, 200 - 16, "Restores party MP.");
		}
		else
		{
			if (selection == 2)
				CmdDrawText(font_sprite, 8, 200 - 16, "Immobilizes target, uses 30 MP.");
			else if (selection == 3)
				CmdDrawText(font_sprite, 8, 200 - 16, "Desintegrates target, uses 60 MP.");
		}
	}

	return selection;
}


/*-----------------------------

 Target menu
-----------------------------*/
static uint8_t s_prev_target_selection = 255;

void MenuTargetDraw_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* hud_a, struct Actor* hud_b)
{
	CmdDrawRectangle(UI_WIDTH, UI_HEIGHT, UI_X, UI_Y, UI_BACK_COLOR);
	sPortraitsDraw(portraits_sprite, font_sprite, hud_a, hud_b);

	CmdDrawText(font_sprite, UI_X + UI_COLUMN_2_X, UI_Y + UI_PADDING_Y + UI_LINE_SPACE, "Select your target.");
}

uint8_t MenuTargetDraw_dynamic(uint8_t arrow_sprite, uint8_t selection)
{
	/*
	TODO, determinate if an actor selection is a valid one should be responsability of
	      the actor module. Here we simply should ask something like: 'GetNearValidActor()'
	*/

	uint8_t i = 0;

	/* Black arrow, to clean previous frame */
	for (i = (HEROES_NO); i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		CmdDrawSprite(arrow_sprite, g_actor[i].x, g_actor[i].y + 40, 2);
	}

	/* Validate selection, we dont' want
	attack our allies or dead enemies */
	if (selection == 0)
		selection = HEROES_NO;

	while (g_actor[selection].state == ACTOR_STATE_DEAD)
	{
		if (selection > s_prev_target_selection)
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

	/* Active arrow */
	CmdDrawSprite(arrow_sprite, g_actor[selection].x, g_actor[selection].y + 40, (CURRENT_FRAME >> 2) % 2);

	s_prev_target_selection = selection;
	return selection;
}


/*-----------------------------

 MenuClean()
-----------------------------*/
void MenuClean()
{
	CmdDrawRectangleBkg(UI_WIDTH, UI_HEIGHT, UI_X, UI_Y);
}
