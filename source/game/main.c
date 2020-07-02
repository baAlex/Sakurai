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


#define TEST4


void* StateHello();
static void* (*s_next_state)() = StateHello;


int main()
{
	/* PROTIP: 'state-battle.c' is the file you are looking for */
	/* This entry point only has the purpose of redirect to the next state... */
	s_next_state = (void* (*)())s_next_state();
	return 0;
}


void* StateHello()
{
	/* ... 'StateHello' being the first state executed, from here every state should
	   return what to call the next frame, most states returns themself */

	IntPrintText("Tanaka's magical business v0.2-alpha\n");
	IntPrintText("- Max commands: ");
	IntPrintNumber(MAX_COMMANDS);
	IntPrintText("- Current milliseconds: ");
	IntPrintNumber(CURRENT_MILLISECONDS);
	IntPrintText("\n");

	#if defined(INTRO)
	return StateIntro();
	#elif defined(BATTLE)
	return StateBattle();
	#elif defined(SCREENSHOTS)
	return StateScreenshots();
	#elif defined(TEST4)
	return StateTest4();
	#elif defined(TEST3)
	return StateTest3();
	#elif defined(TEST2)
	return StateTest2();
	#else
	return StateIntro();
	#endif
}
