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

 [engine-sdl.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "engine-sdl.h"

#define COMMANDS_TABLE_OFFSET 0x0020

static unsigned s_cmd_counter = 0;
extern void (*g_interrupt)();


void IntPrintText(const char* text)
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x01;
	*((uintptr_t*)g_ifd_args_offset + 1) = (uintptr_t)text;
	g_interrupt();
}


void IntPrintNumber(uint16_t number)
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x02;
	*((uintptr_t*)g_ifd_args_offset + 1) = number;
	g_interrupt();
}


void IntLoadBackground(const char* filename)
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x03;
	*((uintptr_t*)g_ifd_args_offset + 1) = (uintptr_t)filename;
	g_interrupt();
}


void IntLoadPalette(const char* filename)
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x04;
	*((uintptr_t*)g_ifd_args_offset + 1) = (uintptr_t)filename;
	g_interrupt();
}


uint8_t IntLoadSprite(const char* filename)
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x07;
	*((uintptr_t*)g_ifd_args_offset + 1) = (uintptr_t)filename;
	g_interrupt();

	return (uint8_t)(*((uintptr_t*)g_ifd_args_offset)); /* My eyes! */
}


void IntUnloadAll()
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x05;
	g_interrupt();
}


void IntFlushCommands()
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x06;
	g_interrupt();

	s_cmd_counter = 0;
}


void IntExitRequest()
{
	*((uintptr_t*)g_ifd_args_offset + 0) = 0x09;
	g_interrupt();
}


/* ---- */


struct __attribute__((packed)) Command
{
	uint8_t code;
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint16_t d;
	uint16_t e;
};


static inline void sIncrementCounter()
{
	if ((s_cmd_counter += 1) == (MAX_COMMANDS))
		IntFlushCommands();
}


void CmdHalt()
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x00};

	s_cmd_counter = 0;
}


void CmdDrawBackground()
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x01};

	sIncrementCounter();
}


void CmdDrawPixel(uint16_t x, uint16_t y, uint8_t color)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x09, .a = color, .d = x, .e = y};

	sIncrementCounter();
}


void CmdDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x04, .a = color, .b = width, .c = height, .d = x, .e = y};

	sIncrementCounter();
}


void CmdDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x02, .b = width, .c = height, .d = x, .e = y};

	sIncrementCounter();
}


void CmdDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x05, .a = color, .b = width, .c = height, .d = x, .e = y};

	sIncrementCounter();
}


void CmdDrawSprite(uint8_t sprite, uint16_t x, uint16_t y, uint8_t frame)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x03, .a = sprite, .b = frame, .d = x, .e = y};

	sIncrementCounter();
}


void CmdDrawText(uint8_t sprite, uint16_t x, uint16_t y, const char* text)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x06, .a = sprite, .d = x, .e = y};

	const char** text_stack = (const char**)g_text_stack_offset;
	text_stack[s_cmd_counter] = text;

	sIncrementCounter();
}


void CmdDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x07, .a = color, .b = width, 0, .d = x, .e = y};

	sIncrementCounter();
}


void CmdDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	struct Command* c = (struct Command*)(g_psp_offset + COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	*c = (struct Command){.code = 0x08, .a = color, .b = height, .d = x, .e = y};

	sIncrementCounter();
}
