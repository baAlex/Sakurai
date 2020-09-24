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

* TODO: (v0.1-alpha) remove reads to 'g_info' and 'g_actor'
* TODO: (v0.2-alpha) still there are lots of hardcoded values
over this file. I replaced most of them, but knowing that in
a future the addition of an inventory will require a mayor
rewrite... I'm simply throwing myself into the lazy side walk.
-----------------------------*/

#include "ui.h"
#include "utilities.h"

#define WINDOW_BACK_COLOR 1
#define WINDOW_OUTLINE_COLOR 3
#define WINDOW_SHADOW_COLOR 64

#define PORTRAITS_X 13
#define PORTRAITS_Y 10

#define PORTRAITS_1COL_TEXT_X (PORTRAITS_X + 43) /* TODO */
#define PORTRAITS_2COL_TEXT_X (PORTRAITS_X + 68) /* TODO */

#define PORTRAITS_1ROW_TEXT_Y (PORTRAITS_Y + 8)  /* TODO */
#define PORTRAITS_2ROW_TEXT_Y (PORTRAITS_Y + 29) /* TODO */

static char s_buffer1[4] = {0};
static char s_buffer2[4] = {0};
static char s_buffer3[4] = {0};
static char s_buffer4[4] = {0};

#define WINDOW_NORMAL 0
#define WINDOW_TITLE 1


static void sDrawWindow(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t tags)
{
	CmdDrawRectangle(width, height, x, y, WINDOW_BACK_COLOR);

	if (tags == WINDOW_TITLE)
		CmdDrawHLine(width, x, y + 16, WINDOW_OUTLINE_COLOR);

	if (y != 0)
		CmdDrawHLine(width, x, y, WINDOW_OUTLINE_COLOR);

	if ((y + (height << 4) - 2) != 198)
		CmdDrawHLine(width, x, (y + (height << 4) - 2), WINDOW_OUTLINE_COLOR);

	if (x != 0)
		CmdDrawVLine(height, x, y, WINDOW_OUTLINE_COLOR);

	if ((x + (width << 4) - 2) != 318)
		CmdDrawVLine(height, (x + (width << 4) - 2), y, WINDOW_OUTLINE_COLOR);

	if ((x + (width << 4) - 1) != 319)
		CmdDrawVLine(height, (x + (width << 4) - 1), y, WINDOW_SHADOW_COLOR);

	if ((y + (height << 4) - 1) != 199)
		CmdDrawHLine(width, x, (y + (height << 4) - 1), WINDOW_SHADOW_COLOR);
}


static void sDrawPortraits(uint8_t portraits_sprite, uint8_t font_sprite, const struct Actor* actor_a,
                           const struct Actor* actor_b)
{
	char* c;

	CmdDrawSprite(portraits_sprite, PORTRAITS_X, PORTRAITS_Y, 0);

	/* Actor A */
	c = NumberToString(actor_a->health, s_buffer3);
	CmdDrawText(font_sprite, PORTRAITS_1COL_TEXT_X, PORTRAITS_1ROW_TEXT_Y, c);

	c = NumberToString(actor_a->magic, s_buffer4);
	CmdDrawText(font_sprite, PORTRAITS_2COL_TEXT_X, PORTRAITS_1ROW_TEXT_Y, c);

	/* Actor B */
	c = NumberToString(actor_b->health, s_buffer1);
	CmdDrawText(font_sprite, PORTRAITS_1COL_TEXT_X, PORTRAITS_2ROW_TEXT_Y, c);

	c = NumberToString(actor_b->magic, s_buffer2);
	CmdDrawText(font_sprite, PORTRAITS_2COL_TEXT_X, PORTRAITS_2ROW_TEXT_Y, c);
}


/*-----------------------------

 Dialogue
-----------------------------*/
#define DIALOG_TIME 125 /* In milliseconds */
#define DIALOG_X 13
#define DIALOG_Y 140
#define DIALOG_SPACING 12


void UiDialog(uint8_t font_sprite, uint16_t start_ms, const char* character, const char* lines[])
{
	uint16_t current_ms = CURRENT_MILLISECONDS;
	uint16_t y = DIALOG_Y;
	uint8_t i = 0;

	if (current_ms == start_ms)
		CmdDrawText(font_sprite, 12, y, character);
	else
	{
		character = NULL; /* Reusing it, from here should be called 'text_line' */

		for (i = 0; i < 4; i++) /* HARDCODED, maximum of 4 lines */
		{
			if (current_ms > (start_ms + (DIALOG_TIME * (i + 1))) && lines[i] != NULL)
			{
				character = lines[i];
				y += DIALOG_SPACING;
			}
			else
				break;
		}

		if (character != NULL)
			CmdDrawText(font_sprite, DIALOG_X, y, character);
	}
}


/*-----------------------------

 Banner
-----------------------------*/
#define BANNER_W 20 /* 320 px */
#define BANNER_H 3  /* 48 px */
#define BANNER_X 0
#define BANNER_Y 76

#define BANNER_TEXT_X 13
#define BANNER_TEXT_Y 94


void UiBanner(uint8_t font_sprite, const char* text)
{
	sDrawWindow(BANNER_W, BANNER_H, BANNER_X, BANNER_Y, WINDOW_NORMAL);
	CmdDrawText(font_sprite, BANNER_TEXT_X, BANNER_TEXT_Y, text);
}


/*-----------------------------

 HUD
-----------------------------*/
void UiHUD(uint8_t portraits_sprite, uint8_t font_sprite, const struct Actor* actor_a, const struct Actor* actor_b)
{
	CmdDrawRectangleBkg(5 /* 80 px */, 3 /* 48 px */, 8, 6); /* TODO */
	sDrawPortraits(portraits_sprite, font_sprite, actor_a, actor_b);
}


/*-----------------------------

 Action Panel
------------------------------*/
#define PANEL_W 19 /* 304 px */
#define PANEL_H 3  /* 48 px */
#define PANEL_X 8
#define PANEL_Y 6

#define PANEL_3COL_TEXT_X 168
#define PANEL_4COL_TEXT_X 244

#define PANEL_1ROW_TEXT_Y 11
#define PANEL_2ROW_TEXT_Y 25
#define PANEL_3ROW_TEXT_Y 39

#define PANEL_1COL_SELECTION_X 152
#define PANEL_2COL_SELECTION_X 228
#define PANEL_SELECTION_Y 11
#define PANEL_SELECTION_SPACING 14

#define TIP_X 0
#define TIP_Y 184
#define TIP_W 20 /* 320 px */
#define TIP_H 1  /* 16 px */

#define TIP_TEXT_X 13
#define TIP_TEXT_Y 189

static uint8_t s_prev_action_selection = 255;


void UiPanelAction_static(uint8_t portraits_sprite, uint8_t font_sprite, const struct Persona* persona,
                          const struct Actor* hud_a, const struct Actor* hud_b)
{
	sDrawWindow(PANEL_W, PANEL_H, PANEL_X, PANEL_Y, WINDOW_NORMAL);
	sDrawPortraits(portraits_sprite, font_sprite, hud_a, hud_b);

	CmdDrawText(font_sprite, 109, PANEL_2ROW_TEXT_Y, persona->name); /* TODO */

	/* Common actions */
	CmdDrawText(font_sprite, PANEL_3COL_TEXT_X, PANEL_1ROW_TEXT_Y, "Attack");
	CmdDrawText(font_sprite, PANEL_4COL_TEXT_X, PANEL_1ROW_TEXT_Y, "Combined");
	CmdDrawText(font_sprite, PANEL_3COL_TEXT_X, PANEL_3ROW_TEXT_Y, "Hold");

	/* Kuro actions */
	if (persona == &g_heroes[PERSONALITY_KURO])
	{
		CmdDrawText(font_sprite, PANEL_3COL_TEXT_X, PANEL_2ROW_TEXT_Y, "Heal");
		CmdDrawText(font_sprite, PANEL_4COL_TEXT_X, PANEL_2ROW_TEXT_Y, "Meditate");
	}

	/* Sayori */
	else
	{
		CmdDrawText(font_sprite, PANEL_3COL_TEXT_X, PANEL_2ROW_TEXT_Y, "Shock");
		CmdDrawText(font_sprite, PANEL_4COL_TEXT_X, PANEL_2ROW_TEXT_Y, "Thunder");
	}
}


uint8_t UiPanelAction_dynamic(uint8_t arrow_sprite, uint8_t font_sprite, const struct Persona* persona,
                              uint8_t selection)
{
	CmdDrawRectangle(1 /* 16 px */, PANEL_H, PANEL_1COL_SELECTION_X, PANEL_Y, WINDOW_BACK_COLOR);
	CmdDrawRectangle(1 /* 16 px */, PANEL_H, PANEL_2COL_SELECTION_X, PANEL_Y, WINDOW_BACK_COLOR);

	/* The previous rectangle overwrites window outline */
	CmdDrawHLine(6 /* 96 px */, PANEL_1COL_SELECTION_X, PANEL_Y, WINDOW_OUTLINE_COLOR);
	CmdDrawHLine(6 /* 96 px */, PANEL_1COL_SELECTION_X, PANEL_Y + (PANEL_H << 4) - 2, WINDOW_OUTLINE_COLOR);
	CmdDrawHLine(6 /* 96 px */, PANEL_1COL_SELECTION_X, PANEL_Y + (PANEL_H << 4) - 1, WINDOW_SHADOW_COLOR);

	/* Selection */
	if (selection > 128)
		selection = 0;
	else if (selection > 4)
		selection = 4;

	if ((selection % 2) == 1)
		CmdDrawSprite(arrow_sprite, PANEL_2COL_SELECTION_X,
		              PANEL_SELECTION_Y + PANEL_SELECTION_SPACING * (selection >> 1), (CURRENT_FRAME >> 2) % 2);
	else
		CmdDrawSprite(arrow_sprite, PANEL_1COL_SELECTION_X,
		              PANEL_SELECTION_Y + PANEL_SELECTION_SPACING * (selection >> 1), (CURRENT_FRAME >> 2) % 2);

	/* Draw a tip at the bottom of the screen */
	if (selection != s_prev_action_selection)
	{
		s_prev_action_selection = selection;
		sDrawWindow(TIP_W, TIP_H, TIP_X, TIP_Y, WINDOW_NORMAL);

		if (selection == 0)
			CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Simple attack.");
		else if (selection == 1)
			CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Combined attack, uses 20 MP.");
		else if (selection == 4)
			CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Hold position, cancels damage from attacks.");

		if (persona == &g_heroes[PERSONALITY_KURO])
		{
			if (selection == 2)
				CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Restores party HP.");
			else if (selection == 3)
				CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Restores party MP.");
		}
		else
		{
			if (selection == 2)
				CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Immobilizes target, uses 30 MP.");
			else if (selection == 3)
				CmdDrawText(font_sprite, TIP_TEXT_X, TIP_TEXT_Y, "Disintegrates target, uses 60 MP.");
		}
	}

	return selection;
}


/*-----------------------------

 Target menu
-----------------------------*/
static uint8_t s_prev_target_selection = 255;


void UiPanelTarget_static(uint8_t portraits_sprite, uint8_t font_sprite, const struct Actor* hud_a,
                          const struct Actor* hud_b)
{
	sDrawWindow(PANEL_W, PANEL_H, PANEL_X, PANEL_Y, WINDOW_NORMAL);
	sDrawPortraits(portraits_sprite, font_sprite, hud_a, hud_b);

	CmdDrawText(font_sprite, 109, PANEL_2ROW_TEXT_Y, "Select a target."); /* TODO */
}


uint8_t UiPanelTarget_dynamic(uint8_t arrow_sprite, uint8_t selection)
{
	/*
	TODO, determinate if an actor selection is a valid one should be responsibility of
	      the actor module. Here we simply should ask something like: 'GetNearValidActor()'

	Currently in v0.2-alpha, I'm not sure if make this change... unnecessary complexity.
	*/

	uint8_t i = 0;

	/* Black arrow, to clean previous frame */
	for (i = ON_SCREEN_HEROES; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		CmdDrawSprite(arrow_sprite, g_actor[i].x, g_actor[i].y + 40, 2);
	}

	/* Validate selection, we don't want attack our allies or dead enemies */
	if (selection == 0)
		selection = ON_SCREEN_HEROES;

	while (g_actor[selection].state == ACTOR_STATE_DEAD)
	{
		if (selection > s_prev_target_selection)
			selection += 1;
		else
			selection -= 1;
	}

	if (selection < ON_SCREEN_HEROES)
	{
		selection = ON_SCREEN_HEROES;
		while (g_actor[selection].state == ACTOR_STATE_DEAD)
			selection += 1;
	}
	else if (selection >= ON_SCREEN_ACTORS)
	{
		selection = ON_SCREEN_ACTORS - 1;
		while (g_actor[selection].state == ACTOR_STATE_DEAD)
			selection -= 1;
	}

	/* Selection */
	CmdDrawSprite(arrow_sprite, g_actor[selection].x, g_actor[selection].y + 40, (CURRENT_FRAME >> 2) % 2);

	s_prev_target_selection = selection;
	return selection;
}


/*-----------------------------

 UiPanelClean()
-----------------------------*/
void UiPanelClean()
{
	CmdDrawRectangleBkg(PANEL_W, PANEL_H, PANEL_X, PANEL_Y);
	CmdDrawRectangleBkg(TIP_W, TIP_H, TIP_X, TIP_Y);
}


/*-----------------------------

 Pause Menu
-----------------------------*/
#define PAUSE_TITLE_X 119
#define PAUSE_TITLE_Y 73

#define PAUSE_TEXT_X 128
#define PAUSE_1ROW_TEXT_Y 89
#define PAUSE_2ROW_TEXT_Y 103
#define PAUSE_3ROW_TEXT_Y 117

#define PAUSE_SELECTION_X 112
#define PAUSE_SELECTION_Y 89
#define PAUSE_SELECTION_SPACING 14


void UiMenuPause_static(uint8_t font_sprite1, uint8_t font_sprite2)
{
	sDrawWindow(PAUSE_W, PAUSE_H, PAUSE_X, PAUSE_Y, WINDOW_TITLE);

	CmdDrawText(font_sprite1, PAUSE_TITLE_X, PAUSE_TITLE_Y, "Game paused");

	CmdDrawText(font_sprite2, PAUSE_TEXT_X, PAUSE_1ROW_TEXT_Y, "Resume");
	CmdDrawText(font_sprite2, PAUSE_TEXT_X, PAUSE_2ROW_TEXT_Y, "Restart game");
	CmdDrawText(font_sprite2, PAUSE_TEXT_X, PAUSE_3ROW_TEXT_Y, "Exit to system");
}


uint8_t UiMenuPause_dynamic(uint8_t arrow_sprite, uint8_t selection)
{
	CmdDrawRectangle(1 /* 16 px */, 3, 112, 82, WINDOW_BACK_COLOR); /* TODO, see mockup */
	CmdDrawVLine(3, 112, 82, WINDOW_OUTLINE_COLOR);
	CmdDrawHLine(1, 112, PAUSE_Y + 16, WINDOW_OUTLINE_COLOR);

	/* Selection */
	if (selection == 255)
		selection = 0;
	else if (selection > 2)
		selection = 2;

	CmdDrawSprite(arrow_sprite, PAUSE_SELECTION_X, PAUSE_SELECTION_Y + (PAUSE_SELECTION_SPACING * selection),
	              (CURRENT_FRAME >> 2) % 2);

	return selection;
}
