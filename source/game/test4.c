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

 [test4.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "tests.h"
#include "utilities.h"

#include "ui.h"


uint16_t Test4_frame = 0;
uint8_t Test4_selection = 0;


void* Test4()
{
	union Command* com;

	/* First frame only... */
	if (Test4_frame == 0)
		DrawTargetUI_static(3);

	/* Every single one */
	{
		if (INPUT_UP == 1)
			Test4_selection -= 2;

		if (INPUT_DOWN == 1)
			Test4_selection += 2;

		if (Test4_selection > 128)
			Test4_selection = 0;
		else if (Test4_selection > 3)
			Test4_selection = 3;

		DrawTargetUI_dynamic(Test4_selection, 4);
	}

	/* Bye! */
	NewCommand(CODE_HALT);
	CleanCommands();

	Test4_frame += 1;

	return Test4;
}


void* Test4Start()
{
	LoadSprite("assets\\font1.jvn", 20);
	LoadSprite("assets\\font2.jvn", 21);
	LoadSprite("assets\\ui-ports.jvn", 3);
	LoadSprite("assets\\ui-items.jvn", 4);

	LoadBackground((uint16_t) "assets\\bkg2.raw");

	Test4_frame = 0;
	Test4_selection = 0;

	Test4();
	return Test4;
}
