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

 [engine.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "engine.h"


#define INT_FD_ARG1_OFFSET 0x0008
#define INT_FD_ARG2_OFFSET 0x000A
#define INT_FD_ARG3_OFFSET 0x000C
#define INT_FD_ARG4_OFFSET 0x000E

#define COMMANDS_TABLE_OFFSET 0x0020

#define CODE_HALT 0x00
#define CODE_DRAW_BKG 0x01
#define CODE_DRAW_PIXEL 0x02
#define CODE_DRAW_RECTANGLE 0x04
#define CODE_DRAW_RECTANGLE_BKG 0x05
#define CODE_DRAW_RECTANGLE_PRECISE 0x06
#define CODE_DRAW_SPRITE 0x07
#define CODE_DRAW_TEXT 0x08
#define CODE_DRAW_H_LINE 0x09
#define CODE_DRAW_V_LINE 0x0A

struct CmdDrawShape
{
	/* DRAW_PIXEL, DRAW_RECTANGLE,
	DRAW_RECTANGLE_BKG, DRAW_RECTANGLE_PRECISE */
	uint8_t code;
	uint8_t color;
	uint8_t width;  /* Multiplied by 16, except on 'Precise' variants */
	uint8_t height; /* Multiplied by 16, except on 'Precise' variants */
	uint16_t x;
	uint16_t y;
};

struct CmdDrawSprite
{
	/* DRAW_SPRITE */
	uint8_t code;
	uint8_t slot;
	uint8_t frame;
	uint8_t unused;
	uint16_t x;
	uint16_t y;
};

struct CmdDrawText
{
	/* CODE_DRAW_TEXT */
	uint8_t code;
	uint8_t slot;
	uint16_t text;
	uint16_t x;
	uint16_t y;
};

union Command {
	uint8_t code;
	struct CmdDrawShape shape;
	struct CmdDrawSprite sprite;
	struct CmdDrawText text;
};


static uint8_t s_cmd_counter = 0;


void IntPrintText(char* text)
{
#if defined(__BCC__) && defined(__MSDOS__)
	*((uint16_t*)INT_FD_ARG1_OFFSET) = 0x01;
	*((uint16_t*)INT_FD_ARG2_OFFSET) = (uint16_t)text;
	asm("int 0xFD");
#else
	(void)text;
#endif
}


void IntPrintNumber(uint16_t number)
{
#if defined(__BCC__) && defined(__MSDOS__)
	*((uint16_t*)INT_FD_ARG1_OFFSET) = 0x02;
	*((uint16_t*)INT_FD_ARG2_OFFSET) = number;
	asm("int 0xFD");
#else
	(void)number;
#endif
}


void IntLoadBackground(char* filename)
{
#if defined(__BCC__) && defined(__MSDOS__)
	*((uint16_t*)INT_FD_ARG1_OFFSET) = 0x03;
	*((uint16_t*)INT_FD_ARG2_OFFSET) = (uint16_t)filename;
	asm("int 0xFD");
#else
	(void)filename;
#endif
}


void IntLoadSprite(char* filename, uint16_t slot)
{
#if defined(__BCC__) && defined(__MSDOS__)
	*((uint16_t*)INT_FD_ARG1_OFFSET) = 0x04;
	*((uint16_t*)INT_FD_ARG2_OFFSET) = (uint16_t)filename;
	*((uint16_t*)INT_FD_ARG3_OFFSET) = slot;
	asm("int 0xFD");
#else
	(void)filename;
	(void)slot;
#endif
}


void IntUnloadAll()
{
#if defined(__BCC__) && defined(__MSDOS__)
	*((uint16_t*)INT_FD_ARG1_OFFSET) = 0x05;
	asm("int 0xFD");
#endif
}


void IntFlushCommands()
{
	CmdHalt();

#if defined(__BCC__) && defined(__MSDOS__)
	*((uint16_t*)INT_FD_ARG1_OFFSET) = 0x06;
	asm("int 0xFD");
#endif
}


#define sIncrementCounter() {if ((s_cmd_counter += 1) == (MAX_COMMANDS -1)){ IntFlushCommands(); }}


void CmdDrawBackground()
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_BKG;

	sIncrementCounter();
}


void CmdDrawPixel(uint16_t x, uint16_t y, uint8_t color)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_PIXEL;
	c->shape.x = x;
	c->shape.y = y;
	c->shape.color = color;

	sIncrementCounter();
}


void CmdDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_RECTANGLE;
	c->shape.width = width;
	c->shape.height = height;
	c->shape.x = x;
	c->shape.y = y;
	c->shape.color = color;

	sIncrementCounter();
}


void CmdDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_RECTANGLE_BKG;
	c->shape.width = width;
	c->shape.height = height;
	c->shape.x = x;
	c->shape.y = y;

	sIncrementCounter();
}


void CmdDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_RECTANGLE_PRECISE;
	c->shape.width = width;
	c->shape.height = height;
	c->shape.x = x;
	c->shape.y = y;
	c->shape.color = color;

	sIncrementCounter();
}


void CmdDrawSprite(uint8_t slot, uint16_t x, uint16_t y, uint8_t frame)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_SPRITE;
	c->sprite.slot = slot;
	c->sprite.x = x;
	c->sprite.y = y;
	c->sprite.frame = frame;

	sIncrementCounter();
}


void CmdDrawText(uint8_t slot, uint16_t x, uint16_t y, char* text)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_TEXT;
	c->text.slot = slot;
	c->text.x = x;
	c->text.y = y;
	c->text.text = (uint16_t)text;

	sIncrementCounter();
}


void CmdDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_H_LINE;
	c->shape.color = color;
	c->shape.width = width;
	c->shape.x = x;
	c->shape.y = y;

	sIncrementCounter();
}


void CmdDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color)
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_DRAW_V_LINE;
	c->shape.color = color;
	c->shape.width = height; /* Yup, in width */
	c->shape.x = x;
	c->shape.y = y;

	sIncrementCounter();
}


void CmdHalt()
{
	union Command* c = (union Command*)(COMMANDS_TABLE_OFFSET) + s_cmd_counter;
	c->code = CODE_HALT;

	s_cmd_counter = 0;
}
