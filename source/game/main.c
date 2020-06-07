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

 [main.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "state.h"
#include "utilities.h"


#define TEST3


void* Hello();
static void* (*s_next_function)() = Hello;


int main()
{
	s_next_function = (void *(*)())s_next_function(); /* void *(*)() !!! */
	return 0;
}


void* Hello()
{
	IntPrintText("Tanaka's magical business v0.2-alpha\n");
	IntPrintText("- Max commands: ");
	IntPrintNumber(MAX_COMMANDS);
	IntPrintText("- Current milliseconds: ");
	IntPrintNumber(CURRENT_MILLISECONDS);
	IntPrintText("\n");

	#if defined(GAME)
	return StateGame();
	#elif defined(TEST4)
	return StateTest4();
	#elif defined(TEST3)
	return StateTest3();
	#elif defined(TEST2)
	return StateTest2();
	#else
	return StateTest1();
	#endif
}


