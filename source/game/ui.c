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
#define UI_COLUMN_3_X (UI_COLUMN_2_X + 75)


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
}


void DrawHUD(uint8_t portraits_slot)
{
	sDrawPortraits(1, portraits_slot);
}


void DrawStaticUI(uint8_t portraits_slot)
{
	union Command* com;

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
	com->draw_text.text = (uint16_t) "[Sayori]";

	/* Actions */
	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_3_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y;
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "Attack";

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_3_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y + UI_LINE_SPACE;
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "Magic";

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = UI_X + UI_COLUMN_3_X;
	com->draw_text.y = UI_Y + UI_PADDING_Y + (UI_LINE_SPACE << 1);
	com->draw_text.slot = 21;
	com->draw_text.text = (uint16_t) "Item";
}


void DrawDynamicUI(uint8_t selection, uint8_t item_slot)
{
	union Command* com;

	com = NewCommand(CODE_DRAW_RECTANGLE);
	com->draw_shape.color = UI_BACK_COLOR;
	com->draw_sprite.x = UI_X + UI_COLUMN_3_X - 16;
	com->draw_sprite.y = UI_Y;
	com->draw_shape.width = 1; /* 16 px */
	com->draw_shape.height = UI_HEIGHT;

	com = NewCommand(CODE_DRAW_SPRITE);
	com->draw_sprite.x = UI_X + UI_COLUMN_3_X - 16;
	com->draw_sprite.y = UI_Y + UI_PADDING_Y + (UI_LINE_SPACE * selection);
	com->draw_sprite.slot = item_slot;
	com->draw_sprite.frame = CURRENT_FRAME >> 2;
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
