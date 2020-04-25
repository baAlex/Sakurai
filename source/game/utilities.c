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

 [utilities.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "utilities.h"

static uint8_t s_commands_counter = 0;
static uint16_t s_marsaglia = 1;


uint16_t Random()
{
	/* http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html */
	s_marsaglia ^= s_marsaglia << 7;
	s_marsaglia ^= s_marsaglia >> 9;
	s_marsaglia ^= s_marsaglia << 8;
	return s_marsaglia;
}


union Command* NewCommand(uint8_t code)
{
	union Command* i = (union Command*)(COMMANDS_TABLE_OFFSET) + s_commands_counter;
	s_commands_counter += 1;
	i->code = code;
	return i;
}


void CleanCommands()
{
	s_commands_counter = 0;
}


void PrintString(uint16_t string)
{
	uint16_t* a1 = (uint16_t*)INT_FD_ARG1;
	uint16_t* a2 = (uint16_t*)INT_FD_ARG2;
	*a1 = 0x01;
	*a2 = string;
	asm("int 0xFD");
}


void PrintNumber(uint16_t number)
{
	uint16_t* a1 = (uint16_t*)INT_FD_ARG1;
	uint16_t* a2 = (uint16_t*)INT_FD_ARG2;
	*a1 = 0x02;
	*a2 = number;
	asm("int 0xFD");
}


void LoadBackground(uint16_t filename)
{
	uint16_t* a1 = (uint16_t*)INT_FD_ARG1;
	uint16_t* a2 = (uint16_t*)INT_FD_ARG2;
	*a1 = 0x03;
	*a2 = filename;
	asm("int 0xFD");
}


void LoadSprite(uint16_t filename, uint16_t slot)
{
	uint16_t* a1 = (uint16_t*)INT_FD_ARG1;
	uint16_t* a2 = (uint16_t*)INT_FD_ARG2;
	uint16_t* a3 = (uint16_t*)INT_FD_ARG3;
	*a1 = 0x04;
	*a2 = filename;
	*a3 = slot;
	asm("int 0xFD");
}
