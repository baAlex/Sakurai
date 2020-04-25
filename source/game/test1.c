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

 [test1.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "tests.h"
#include "utilities.h"

#define TEST_TEXT "[Tanaka]\n\
But being as this is a Schwarz Sechs Prototype MkII, the\n\
most powerful device ever conceived, and would vanish\n\
you out from this world clean off, you've got to ask\n\
yourself one question: \"Do I feel lucky?\".\n\
\n\
Well, do ya, punk?"


void* Test1()
{
	union Command* com;

	if ((CURRENT_FRAME % 96) == 0) /* Every 4 seconds */
	{
		com = NewCommand(CODE_DRAW_RECTANGLE);
		com->draw_shape.color = 64 + (Random() % 10);
		com->draw_shape.x = 0;
		com->draw_shape.y = 0;
		com->draw_shape.width = 20;
		com->draw_shape.height = 13;

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = 1;
		com->draw_text.y = 0;
		com->draw_text.slot = 1;
		com->draw_text.text = (uint16_t)TEST_TEXT;

		com = NewCommand(CODE_DRAW_TEXT);
		com->draw_text.x = 1;
		com->draw_text.y = 100;
		com->draw_text.slot = 2;
		com->draw_text.text = (uint16_t)TEST_TEXT;
	}

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	return Test1;
}


void* Test1Start()
{
	LoadSprite("assets\\font1.jvn", 1);
	LoadSprite("assets\\font2.jvn", 2);

	Test1();
	return Test1;
}
