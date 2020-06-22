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


/*-----------------------------

 Dialog
-----------------------------*/

#define DIALOG_TIME 125

void DialogDraw(uint8_t font_sprite, uint16_t start_ms, char* character, char** text)
{
	uint16_t current_ms = CURRENT_MILLISECONDS;
	uint16_t y = 140;
	uint8_t i = 0;

	if (current_ms == start_ms)
		CmdDrawText(font_sprite, 12, y, character);
	else
	{
		character = NULL;

		for (i = 0; i < 4; i++)
		{
			if (current_ms > (start_ms + (DIALOG_TIME * (i + 1))) && text[i] != NULL)
			{
				character = text[i];
				y += 12;
			}
			else
				break;
		}

		if (character != NULL)
			CmdDrawText(font_sprite, 12, y, character);
	}
}


/*-----------------------------

 sPortraitsDraw()
-----------------------------*/

#define PORTRAITS_X 13
#define PORTRAITS_Y 10

#define PORTRAITS_1ST_COL (PORTRAITS_X + 43)
#define PORTRAITS_2ND_COL (PORTRAITS_X + 68)

#define PORTRAITS_1ST_ROW (PORTRAITS_Y + 7)
#define PORTRAITS_2ND_ROW (PORTRAITS_Y + 28)

static char s_buffer1[4] = {0, 0, 0, 0};
static char s_buffer2[4] = {0, 0, 0, 0};
static char s_buffer3[4] = {0, 0, 0, 0};
static char s_buffer4[4] = {0, 0, 0, 0};

static void sPortraitsDraw(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor_a, struct Actor* actor_b)
{
	char* c;

	CmdDrawSprite(portraits_sprite, PORTRAITS_X, PORTRAITS_Y, 0);

	/* Actor A */
	c = NumberToString(actor_a->health, s_buffer3);
	CmdDrawText(font_sprite, PORTRAITS_1ST_COL, PORTRAITS_1ST_ROW, c);

	c = NumberToString(actor_a->magic, s_buffer4);
	CmdDrawText(font_sprite, PORTRAITS_2ND_COL, PORTRAITS_1ST_ROW, c);

	/* Actor B */
	c = NumberToString(actor_b->health, s_buffer1);
	CmdDrawText(font_sprite, PORTRAITS_1ST_COL, PORTRAITS_2ND_ROW, c);

	c = NumberToString(actor_b->magic, s_buffer2);
	CmdDrawText(font_sprite, PORTRAITS_2ND_COL, PORTRAITS_2ND_ROW, c);
}


/*-----------------------------

 Hud
-----------------------------*/
void HudDraw(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor_a, struct Actor* actor_b)
{
	CmdDrawRectangleBkg(5 /* 80 px */, 3 /* 48 px */, PORTRAITS_X, PORTRAITS_Y);
	sPortraitsDraw(portraits_sprite, font_sprite, actor_a, actor_b);
}


/*-----------------------------

 Action menu
------------------------------*/

#define MENU_WIDTH 19 /* 304 px */
#define MENU_HEIGHT 3 /* 48 px */
#define MENU_X 8
#define MENU_Y 6 /* Not 8 because the 1.2 ratio */

#define MENU_BACK_COLOR 1
#define MENU_OUTLINE_COLOR 3

#define MENU_LINE_SPACE 14

#define MENU_PADDING_Y 3

#define MENU_1ST_COL 8
#define MENU_2ND_COL 110
#define MENU_3RD_COL (MENU_2ND_COL + 60)
#define MENU_4TH_COL (MENU_3RD_COL + 60)

static uint8_t s_prev_action_selection = 255;

void MenuActionDraw_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Persona* persona, struct Actor* hud_a,
                           struct Actor* hud_b)
{
	/*
	TODO, in the future the actors themself should have an entry: 'actor->action[]'
	      so I can delete all the following harcoded values.
	*/

	CmdDrawRectangle(MENU_WIDTH, MENU_HEIGHT, MENU_X, MENU_Y, MENU_BACK_COLOR);

	/* Light outline */
	CmdDrawHLine(MENU_WIDTH, MENU_X, MENU_Y, MENU_OUTLINE_COLOR);
	CmdDrawHLine(MENU_WIDTH, MENU_X, MENU_Y + (MENU_HEIGHT << 4) - 2, MENU_OUTLINE_COLOR);

	CmdDrawVLine(MENU_HEIGHT, MENU_X, MENU_Y, MENU_OUTLINE_COLOR);
	CmdDrawVLine(MENU_HEIGHT, MENU_X + (MENU_WIDTH << 4) - 2, MENU_Y, MENU_OUTLINE_COLOR);

	/* Shadow outline */
	CmdDrawVLine(MENU_HEIGHT, MENU_X + (MENU_WIDTH << 4) - 1, MENU_Y, 64);
	CmdDrawHLine(MENU_WIDTH, MENU_X, MENU_Y + (MENU_HEIGHT << 4) - 1, 64);

	/* Portraits, hero name */
	sPortraitsDraw(portraits_sprite, font_sprite, hud_a, hud_b);
	CmdDrawText(font_sprite, MENU_X + MENU_2ND_COL, MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE, persona->name);

	/* Common actions */
	CmdDrawText(font_sprite, MENU_X + MENU_3RD_COL, MENU_Y + MENU_PADDING_Y, "Attack");
	CmdDrawText(font_sprite, MENU_X + MENU_4TH_COL, MENU_Y + MENU_PADDING_Y, "A. Combined");
	CmdDrawText(font_sprite, MENU_X + MENU_3RD_COL, MENU_Y + MENU_PADDING_Y + (MENU_LINE_SPACE << 1), "Hold");

	/* Kuro actions */
	if (persona == &g_heroes[HERO_KURO])
	{
		CmdDrawText(font_sprite, MENU_X + MENU_3RD_COL, MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE, "Heal");
		CmdDrawText(font_sprite, MENU_X + MENU_4TH_COL, MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE, "Meditate");
	}

	/* Sayori */
	else
	{
		CmdDrawText(font_sprite, MENU_X + MENU_3RD_COL, MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE, "Shock");
		CmdDrawText(font_sprite, MENU_X + MENU_4TH_COL, MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE, "Thunder");
	}
}

uint8_t MenuActionDraw_dynamic(uint8_t arrow_sprite, uint8_t font_sprite, struct Persona* persona, uint8_t selection)
{
	/*
	TODO, same as before, every action should be in companion with a tip.
	*/

	CmdDrawRectangle(1 /* 16 px */, MENU_HEIGHT, MENU_X + MENU_3RD_COL - 16, MENU_Y, MENU_BACK_COLOR);
	CmdDrawRectangle(1 /* 16 px */, MENU_HEIGHT, MENU_X + MENU_4TH_COL - 16, MENU_Y, MENU_BACK_COLOR);

	/* The previous rectangle overwrite the outlines */
	CmdDrawHLine(5 /* 80 px */, MENU_X + MENU_3RD_COL - 16, MENU_Y, MENU_OUTLINE_COLOR);
	CmdDrawHLine(5 /* 80 px */, MENU_X + MENU_3RD_COL - 16, MENU_Y + (MENU_HEIGHT << 4) - 2, MENU_OUTLINE_COLOR);
	CmdDrawHLine(5 /* 80 px */, MENU_X + MENU_3RD_COL - 16, MENU_Y + (MENU_HEIGHT << 4) - 1, 64);

	/* Selection arrow */
	if (selection > 128)
		selection = 0;
	else if (selection > 4)
		selection = 4;

	if ((selection % 2) == 1)
		CmdDrawSprite(arrow_sprite, MENU_X + MENU_4TH_COL - 16,
		              MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE * (selection >> 1), (CURRENT_FRAME >> 2) % 2);
	else
		CmdDrawSprite(arrow_sprite, MENU_X + MENU_3RD_COL - 16,
		              MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE * (selection >> 1), (CURRENT_FRAME >> 2) % 2);

	/* Draw a tip at the bottom of the screen */
	if (selection != s_prev_action_selection)
	{
		CmdDrawRectangle(20 /* 320 */, 1 /* 16 px */, 0, 200 - 16, MENU_BACK_COLOR);
		s_prev_action_selection = selection;

		if (selection == 0)
			CmdDrawText(font_sprite, 8, 200 - 16, "Simple attack.");
		else if (selection == 1)
			CmdDrawText(font_sprite, 8, 200 - 16, "Combined attack, uses 20 MP.");
		else if (selection == 4)
			CmdDrawText(font_sprite, 8, 200 - 16, "Hold position, mitigates damage from imminent attack.");

		if (persona == &g_heroes[HERO_KURO])
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
	CmdDrawRectangle(MENU_WIDTH, MENU_HEIGHT, MENU_X, MENU_Y, MENU_BACK_COLOR);
	sPortraitsDraw(portraits_sprite, font_sprite, hud_a, hud_b);

	CmdDrawText(font_sprite, MENU_X + MENU_2ND_COL, MENU_Y + MENU_PADDING_Y + MENU_LINE_SPACE, "Select your target.");
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
	CmdDrawRectangleBkg(MENU_WIDTH, MENU_HEIGHT, MENU_X, MENU_Y);
}
