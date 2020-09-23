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

 [engine-dos.c]
 - Alexander Brandt 2020

 https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
 https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html
-----------------------------*/

#include "engine-dos.h"

#define COMMANDS_TABLE_OFFSET 0x0020
#define COMMAND_SIZE 8

uint16_t volatile s_command_offset = COMMANDS_TABLE_OFFSET;


void IntPrintText(const char* text)
{
	asm volatile("movw [0x0008], 0x01\n\
	              movw [0x000A], %[text]\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */[ text ] "r"(text)
	             : /* Clobbers */);
}

void IntPrintNumber(uint16_t number)
{
	asm volatile("movw [0x0008], 0x02\n\
	              movw [0x000A], %[number]\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */[ number ] "r"(number)
	             : /* Clobbers */);
}

void IntLoadBackground(char* filename)
{
	asm volatile("movw [0x0008], 0x03\n\
	              movw [0x000A], %[filename]\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */[ filename ] "r"(filename)
	             : /* Clobbers */);
}

void IntLoadPalette(char* filename)
{
	asm volatile("movw [0x0008], 0x04\n\
	              movw [0x000A], %[filename]\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */[ filename ] "r"(filename)
	             : /* Clobbers */);
}

uint8_t IntLoadSprite(char* filename)
{
	asm volatile("movw [0x0008], 0x07\n\
	              movw [0x000A], %[filename]\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */[ filename ] "r"(filename)
	             : /* Clobbers */);

	return *((uint8_t*)0x0008);
}

void IntUnloadAll()
{
	asm volatile("movw [0x0008], 0x05\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */
	             : /* Clobbers */);
}

void IntFlushCommands()
{
	asm volatile("movw [0x0008], 0x06\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */
	             : /* Clobbers */);

	*((uint8_t*)s_command_offset) = 0x00;
	s_command_offset = COMMANDS_TABLE_OFFSET;
}

void IntExitRequest()
{
	asm volatile("movw [0x0008], 0x09\n\
	              int 0xFD\n"
	             : /* Output */
	             : /* Input */
	             : /* Clobbers */);
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


void CmdHalt()
{
	*((uint8_t*)s_command_offset) = 0x00;
	s_command_offset = COMMANDS_TABLE_OFFSET;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawBackground()
{
	*((uint8_t*)s_command_offset) = 0x01;
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawPixel(uint16_t x, uint16_t y, uint8_t color)
{
	*((struct Command*)s_command_offset) = (struct Command){.code = 0x09, .a = color, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	*((struct Command*)s_command_offset) =
	    (struct Command){.code = 0x04, .a = color, .b = width, .c = height, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y)
{
	*((struct Command*)s_command_offset) = (struct Command){.code = 0x02, .b = width, .c = height, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	*((struct Command*)s_command_offset) =
	    (struct Command){.code = 0x05, .a = color, .b = width, .c = height, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawSprite(uint8_t sprite, uint16_t x, uint16_t y, uint8_t frame)
{
	*((struct Command*)s_command_offset) = (struct Command){.code = 0x03, .a = sprite, .b = frame, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawText(uint8_t sprite, uint16_t x, uint16_t y, char* text)
{
	struct __attribute__((packed)) TxtCommand
	{
		uint8_t code;
		uint8_t a;
		uint16_t bc;
		uint16_t d;
		uint16_t e;
	};

	*((struct TxtCommand*)s_command_offset) =
	    (struct TxtCommand){.code = 0x06, .a = sprite, .bc = (uint16_t)text, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color)
{
	*((struct Command*)s_command_offset) = (struct Command){.code = 0x07, .a = color, .b = width, 0, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}

void CmdDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	*((struct Command*)s_command_offset) = (struct Command){.code = 0x08, .a = color, .b = height, .d = x, .e = y};
	s_command_offset += COMMAND_SIZE;

	if ((s_command_offset + COMMAND_SIZE) == (COMMANDS_TABLE_OFFSET + (MAX_COMMANDS << 3)))
		IntFlushCommands();
}
