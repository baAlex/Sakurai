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


void* StateHello();
static void* (*s_next_state)() = StateHello;


#if defined(SAKURAI_DOS)
void __attribute__((__section__(".sakuraiboot"))) boot()
#else
int GameMain()
#endif
{
	/* PROTIP: 'state-battle.c' is the file you are looking for, this
	   entry point only has the purpose of redirect to the actual state.
	   State already defined by the previous frame. */
	s_next_state = (void* (*)())s_next_state();

#if !defined(SAKURAI_DOS)
	return 0;
#endif
}


void* StateHello()
{
	/* 'StateHello' is the first state to be executed. From here every
	   other state should return what to call in the following frame,
	   most states returns themselves */

	IntPrintText("Tanaka's magical business v0.4-alpha\n");
	IntPrintText(" - Max commands: ");
	IntPrintNumber(MAX_COMMANDS);
	IntPrintText(" - Current milliseconds: ");
	IntPrintNumber(CURRENT_MILLISECONDS);
	IntPrintText("\n");

	IntLoadPalette("assets/palette.raw");

	#if defined(BATTLE)
	return StatePrepareBattle(0);
	#elif defined(SCREENSHOTS)
	return StateScreenshots();
	#elif defined(TEST4)
	return StateTest4();
	#elif defined(TEST3)
	return StateTest3();
	#elif defined(TEST2)
	return StateTest2();
	#elif defined(TEST1)
	return StateTest1();

	#else
	return StatePrepareIntro();
	#endif
}
