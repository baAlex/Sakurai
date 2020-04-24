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

 [test2.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "tests.h"
#include "utilities.h"


static void sChangeBackground()
{
	union Command* com;

	switch (Random() % 8)
	{
	case 0: LoadBackground((uint16_t) "assets\\bkg1.raw"); break;
	case 1: LoadBackground((uint16_t) "assets\\bkg2.raw"); break;
	case 2: LoadBackground((uint16_t) "assets\\bkg3.raw"); break;
	case 3: LoadBackground((uint16_t) "assets\\bkg4.raw"); break;
	case 4: LoadBackground((uint16_t) "assets\\bkg8.raw"); break;
	case 5: LoadBackground((uint16_t) "assets\\bkg7.raw"); break;
	case 6: LoadBackground((uint16_t) "assets\\bkg6.raw"); break;
	case 7: LoadBackground((uint16_t) "assets\\bkg5.raw");
	}

	NewCommand(CODE_DRAW_BKG);

	com = NewCommand(CODE_DRAW_TEXT);
	com->draw_text.x = 1;
	com->draw_text.y = 0;
	com->draw_text.slot = 1;
	com->draw_text.text = (uint16_t)"Sakurai, alpha build";
}


void* Test2()
{
	union Command* com;

	int8_t x_sin = Sin((uint8_t)CURRENT_FRAME) >> 2;

	if ((CURRENT_FRAME % 240) == 0) /* Every 10 seconds */
		sChangeBackground();

	/* Draw an sprite whitout clean the background */
	com = NewCommand(CODE_DRAW_SPRITE);
	com->draw_sprite.slot = 2;
	com->draw_sprite.x = 40 + x_sin;
	com->draw_sprite.y = 50;
	com->draw_sprite.frame = CURRENT_FRAME >> 2;

	/* Another, cleaning the background */
	com = NewCommand(CODE_DRAW_RECTANGLE_BKG);
	com->draw_shape.x = 200 + x_sin;
	com->draw_shape.y = 50;
	com->draw_shape.width = 5;  /* 80 px */
	com->draw_shape.height = 6; /* 96 px */

	com = NewCommand(CODE_DRAW_SPRITE);
	com->draw_sprite.slot = 2;
	com->draw_sprite.x = 200 + x_sin;
	com->draw_sprite.y = 50;
	com->draw_sprite.frame = CURRENT_FRAME >> 2;

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return Test2;
}


void* Test2Start()
{
	LoadSprite("assets\\font1.jvn", 1);
	LoadSprite("assets\\idle.jvn", 2);

	Test2();
	return Test2;
}
