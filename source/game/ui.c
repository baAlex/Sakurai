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
	union Command* com;

	/* Background */
	if (clean == 1)
	{
		com = NewCommand(CODE_DRAW_RECTANGLE_BKG);
		com->draw_shape.color = UI_BACK_COLOR + 1;
		com->draw_shape.x = UI_X + 2;
		com->draw_shape.y = UI_Y + 2;
		com->draw_shape.width = 6;  /* 96 px */
		com->draw_shape.height = 3; /* 48 px */
	}

	/* Portraits */
	com = NewCommand(CODE_DRAW_SPRITE);
	com->draw_sprite.x = UI_X + 2;
	com->draw_sprite.y = UI_Y + 2;
	com->draw_sprite.slot = portraits_slot;

	/* Sayori stats */
	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + 43;
	com->draw_text.y = UI_Y + 9;
	com->draw_text.slot = 21;
	com->draw_text.text = sToDecimalString(g_actor[1].health, s_buffer1);

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + 43 + 28;
	com->draw_text.y = UI_Y + 9;
	com->draw_text.slot = 21;
	com->draw_text.text = sToDecimalString(g_actor[1].magic, s_buffer2);

	/* Kuro stats */
	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + 43;
	com->draw_text.y = UI_Y + 32;
	com->draw_text.slot = 21;
	com->draw_text.text = sToDecimalString(g_actor[0].health, s_buffer3);

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + 43 + 28;
	com->draw_text.y = UI_Y + 32;
	com->draw_text.slot = 21;
	com->draw_text.text = sToDecimalString(g_actor[0].magic, s_buffer4);
}


void DrawHUD(uint8_t portraits_slot)
{
	sDrawPortraits(1, portraits_slot);
}


void DrawStaticUI(uint8_t portraits_slot, uint8_t actor_index)
{
	union Command* com;
	struct Actor* actor = &g_actor[actor_index];

	/* UI background */
	com = NewCommand(CODE_DRAW_RECTANGLE);
	com->draw_shape.color = UI_BACK_COLOR;
	com->draw_shape.x = UI_X;
	com->draw_shape.y = UI_Y;
	com->draw_shape.width = UI_WIDTH;
	com->draw_shape.height = UI_HEIGHT;

	/* Portraits */
	sDrawPortraits(0, portraits_slot);

	/* Hero name */
	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_2_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE;
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t)g_persona[actor->type].name;

	/* Common actions */
	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_3_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y;
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "Attack";

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_4_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y;
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "A. Combined";

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_3_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y + (UI_LINE_SPACE << 1);
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "Hold";

	/* Kuro actions */
	if (actor_index == 0)
	{
		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = UI_X + UI_COLUMN_3_X;
		com->draw_text.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE;
		com->draw_text.slot = 21;
		com->draw_text.text = (uint16_t) "Heal";

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = UI_X + UI_COLUMN_4_X;
		com->draw_text.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE;
		com->draw_text.slot = 21;
		com->draw_text.text = (uint16_t) "Meditate";
	}

	/* Sayori */
	else
	{
		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = UI_X + UI_COLUMN_3_X;
		com->draw_text.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE;
		com->draw_text.slot = 21;
		com->draw_text.text = (uint16_t) "Shock";

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = UI_X + UI_COLUMN_4_X;
		com->draw_text.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE;
		com->draw_text.slot = 21;
		com->draw_text.text = (uint16_t) "Thunder";
	}
}


static uint8_t s_previous_selection = 255;

void DrawDynamicUI(uint8_t selection, uint8_t item_slot, uint8_t actor_index)
{
	union Command* com;

	com = NewCommand(CODE_DRAW_RECTANGLE);
	com->draw_shape.color = UI_BACK_COLOR;
	com->draw_sprite.x = UI_X + UI_COLUMN_3_X - 16;
	com->draw_sprite.y = UI_Y;
	com->draw_shape.width = 1; /* 16 px */
	com->draw_shape.height = UI_HEIGHT;

	com = NewCommand(CODE_DRAW_RECTANGLE);
	com->draw_shape.color = UI_BACK_COLOR;
	com->draw_sprite.x = UI_X + UI_COLUMN_4_X - 16;
	com->draw_sprite.y = UI_Y;
	com->draw_shape.width = 1; /* 16 px */
	com->draw_shape.height = UI_HEIGHT;

	com = NewCommand(CODE_DRAW_SPRITE);
	com->draw_sprite.slot = item_slot;
	com->draw_sprite.frame = CURRENT_FRAME >> 2;
	com->draw_sprite.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE * (selection >> 1);

	if ((selection % 2) == 1)
		com->draw_sprite.x = UI_X + UI_COLUMN_4_X - 16;
	else
		com->draw_sprite.x = UI_X + UI_COLUMN_3_X - 16;

	if (selection != s_previous_selection)
	{
		s_previous_selection = selection;

		com = NewCommand(CODE_DRAW_RECTANGLE);
		com->draw_shape.color = UI_BACK_COLOR;
		com->draw_sprite.x = 0;
		com->draw_sprite.y = 184;
		com->draw_shape.width = 20;
		com->draw_shape.height = 1;

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = 8;
		com->draw_text.y = 184;
		com->draw_text.slot = 20;

		if (selection == 0)
			com->draw_text.text = (uint16_t) "Simple attack.";
		else if (selection == 1)
			com->draw_text.text = (uint16_t) "Combined attack, uses 20 MP.";
		else if (selection == 4)
			com->draw_text.text = (uint16_t) "Hold position, mitigates damage of imminent attack.";

		if (actor_index == 0)
		{
			if (selection == 2)
				com->draw_text.text = (uint16_t) "Restores party HP.";
			else if (selection == 3)
				com->draw_text.text = (uint16_t) "Restores party MP.";
		}
		else
		{
			if (selection == 2)
				com->draw_text.text = (uint16_t) "Immobilizes target, uses 30 MP.";
			else if (selection == 3)
				com->draw_text.text = (uint16_t) "Attacks multiple targets at once, uses 50 MP.";
		}
	}
}


void CleanStaticUI()
{
	union Command* com;

	com = NewCommand(CODE_DRAW_RECTANGLE_BKG);
	com->draw_shape.color = UI_BACK_COLOR;
	com->draw_shape.x = UI_X;
	com->draw_shape.y = UI_Y;
	com->draw_shape.width = UI_WIDTH;
	com->draw_shape.height = UI_HEIGHT;
}
