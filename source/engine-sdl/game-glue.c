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

 [game-glue.c]
 - Alexander Brandt 2020
-----------------------------*/

#include <stdint.h>
#include <string.h>

#include "draw.h"
#include "game-glue.h"

#define COMMAND_SIZE 8
#define COMMANDS_TABLE_LEN 64   // 28 in the Dos engine
#define COMMANDS_TABLE_SIZE 512 // 224 in the Dos engine
#define SPRITE_INDIRECTION_LEN 32

// Game PSP, counterpart of the same structure in 'shared.inc'
// https://en.wikipedia.org/wiki/Program_Segment_Prefix
struct GamePSP
{
	uint16_t empty_word;    // 0x0000
	uint16_t frame_counter; // 0x0002
	uint16_t ms_counter;    // 0x0004
	uint16_t max_commands;  // 0x0006
	uint16_t unused1;       // 0x0008
	uint16_t unused2;       // 0x000A
	uint16_t unused3;       // 0x000C
	uint16_t unused4;       // 0x000E

	uint8_t input_x;      // 0x0010
	uint8_t input_y;      // 0x0011
	uint8_t unused11;     // 0x0012
	uint8_t unused12;     // 0x0013
	uint8_t input_pad_u;  // 0x0014
	uint8_t input_pad_d;  // 0x0015
	uint8_t input_pad_l;  // 0x0016
	uint8_t input_pad_r;  // 0x0017
	uint8_t input_select; // 0x0018
	uint8_t input_start;  // 0x0019
	uint8_t unused5;      // 0x001A
	uint8_t unused6;      // 0x001B
	uint8_t unused7;      // 0x001C
	uint8_t unused8;      // 0x001D
	uint8_t unused9;      // 0x001E
	uint8_t unused10;     // 0x001F

	uint8_t commands_table[COMMANDS_TABLE_SIZE]; // 0x0020

	// --- Roughly Dos-compatible PSP ends here ---

	// What follow replaces the 'uint16_t' Dos ifd arguments,
	// as these are intended to pass pointers and in modern
	// platform we have extra thicc addresses
	uintptr_t ifd_arg1;
	uintptr_t ifd_arg2;
	uintptr_t ifd_arg3;
	uintptr_t ifd_arg4;

	char* txt_stack[COMMANDS_TABLE_LEN];
};


union GameCommand
{
	uint8_t code;

	struct
	{
		uint8_t code;
		uint8_t color;
		uint8_t width;
		uint8_t height;
		uint16_t x;
		uint16_t y;
	} shape;

	struct
	{
		uint8_t code;
		uint8_t sprite;
		uint8_t frame;
		uint8_t unused;
		uint16_t x;
		uint16_t y;
	} sprite;

	struct
	{
		uint8_t code;
		uint8_t sprite;
		uint16_t text;
		uint16_t x;
		uint16_t y;
	} text;
};


struct GlueData
{
	struct GamePSP psp;

	void (*callback_func)(struct GameInterruption, uintptr_t*, void*);
	void* callback_data;

	uintptr_t sprite_indirection[SPRITE_INDIRECTION_LEN];
};


// <!!!>

static struct GlueData s_glue;

uintptr_t g_psp_offset;       // Game code requires it, points to 's_glue.psp'
uintptr_t g_ifd_args_offset;  // Game code requires it, points to 's_glue.psp::ifd_arg1'
uintptr_t g_txt_stack_offset; // Game code requires it, points to 's_glue.psp::txt_stack'

void (*g_interrupt)(); // Where game calls
extern int GameMain(); // Where we, the engine, call

// </!!!>


static void sGameInterrupt()
{
	struct GameInterruption i = {0};
	uintptr_t ret = 0;

	switch (s_glue.psp.ifd_arg1)
	{
	case 0x01: // GamePrintString
		i.type = GAME_PRINT_STRING;
		i.string = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		break;

	case 0x02: // GamePrintNumber
		i.type = GAME_PRINT_NUMBER;
		i.number = (unsigned)s_glue.psp.ifd_arg2;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		break;

	case 0x03: // GameLoadBackground
		i.type = GAME_LOAD_BACKGROUND;
		i.filename = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		break;

	case 0x04: // GameLoadPalette
		i.type = GAME_LOAD_PALETTE;
		i.filename = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		break;

	case 0x05: // GameUnloadEverything
		i.type = GAME_UNLOAD_EVERYTHING;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		break;

	case 0x07: // GameLoadSprite
		i.type = GAME_LOAD_SPRITE;
		i.filename = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(i, &ret, s_glue.callback_data);

		for (int t = 0; t < SPRITE_INDIRECTION_LEN; t++) // TODO, ERRORS!
		{
			if (s_glue.sprite_indirection[t] == 0 || s_glue.sprite_indirection[t] == ret) // TODO, horrible resolution
			{
				s_glue.sprite_indirection[t] = ret;
				s_glue.psp.ifd_arg1 = t; // Value for the game
				break;
			}
		}
		break;

	case 0x08: // GameFreeSprite
		i.type = GAME_FREE_SPRITE;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		s_glue.sprite_indirection[s_glue.psp.ifd_arg2] = 0;
		break;

	case 0x09: // GameExitRequest
		i.type = GAME_EXIT_REQUEST;
		s_glue.callback_func(i, NULL, s_glue.callback_data);
		break;
	}
}


int GlueStart(void (*callback_func)(struct GameInterruption, uintptr_t*, void*), void* callback_data)
{
	s_glue.psp.max_commands = COMMANDS_TABLE_LEN;
	s_glue.callback_func = callback_func;
	s_glue.callback_data = callback_data;

	g_psp_offset = (uintptr_t)(&s_glue.psp);
	g_ifd_args_offset = (uintptr_t)(&(s_glue.psp.ifd_arg1));
	g_txt_stack_offset = (uintptr_t)(&(s_glue.psp.txt_stack));
	g_interrupt = sGameInterrupt;
	return 0;
}


void GlueStop() {}


void GlueFrame(struct kaEvents in, size_t ms, const struct jaImage* buffer_background, struct jaImage* buffer_out)
{
	union GameCommand* cmd = (union GameCommand*)s_glue.psp.commands_table;
	union GameCommand* end = cmd + COMMANDS_TABLE_LEN;

	s_glue.psp.ms_counter = (uint16_t)(ms % UINT16_MAX);

	s_glue.psp.input_x = (in.x == true) ? 1 : 0 | (in.a == true) ? 1 : 0;
	s_glue.psp.input_y = (in.y == true) ? 1 : 0 | (in.b == true) ? 1 : 0;
	s_glue.psp.input_pad_l = (in.pad_l == true) ? 1 : 0;
	s_glue.psp.input_pad_r = (in.pad_r == true) ? 1 : 0;
	s_glue.psp.input_pad_u = (in.pad_u == true) ? 1 : 0;
	s_glue.psp.input_pad_d = (in.pad_d == true) ? 1 : 0;
	s_glue.psp.input_start = (in.start == true) ? 1 : 0;
	s_glue.psp.input_select = (in.select == true) ? 1 : 0;

	GameMain(); // FIXME, the game always return zero!

	s_glue.psp.frame_counter += 1;

	for (; cmd < end; cmd++)
	{
		switch (cmd->code)
		{
		case 0x00: // CODE_HALT
			return;
			break;
		case 0x01: // CODE_DRAW_BKG
			DrawBkg(buffer_background, buffer_out);
			break;
		case 0x02: // CODE_DRAW_RECTANGLE_BKG
			DrawRectangleBkg(cmd->shape.width, cmd->shape.height, cmd->shape.x, cmd->shape.y, buffer_background,
			                 buffer_out);
			break;
		case 0x03: // CODE_DRAW_SPRITE
			DrawSprite((struct JvnImage*)s_glue.sprite_indirection[cmd->sprite.sprite], cmd->sprite.x, cmd->sprite.y,
			           cmd->sprite.frame, buffer_out);
			break;
		case 0x04: // CODE_DRAW_RECTANGLE
			DrawRectangle(cmd->shape.width, cmd->shape.height, cmd->shape.x, cmd->shape.y, cmd->shape.color,
			              buffer_out);
			break;
		case 0x05: // CODE_DRAW_RECTANGLE_PRECISE
			DrawRectanglePrecise(cmd->shape.width, cmd->shape.height, cmd->shape.x, cmd->shape.y, cmd->shape.color,
			                     buffer_out);
			break;
		case 0x06: // CODE_DRAW_TEXT
			DrawText((struct JvnImage*)s_glue.sprite_indirection[cmd->text.sprite], cmd->text.x, cmd->text.y,
			         s_glue.psp.txt_stack[cmd->text.text], buffer_out);
			break;
		case 0x07: // CODE_DRAW_H_LINE
			DrawHLine(cmd->shape.width, cmd->shape.x, cmd->shape.y, cmd->shape.color, buffer_out);
			break;
		case 0x08: // CODE_DRAW_V_LINE
			DrawVLine(cmd->shape.width, cmd->shape.x, cmd->shape.y, cmd->shape.color, buffer_out);
			break;
		case 0x09: // CODE_DRAW_PIXEL
			DrawPixel(cmd->shape.x, cmd->shape.y, cmd->shape.color, buffer_out);
			break;
		default: break;
		}
	}
}