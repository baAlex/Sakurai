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
	uint8_t input_a;      // 0x0012
	uint8_t input_b;      // 0x0013
	uint8_t input_up;     // 0x0014
	uint8_t input_down;   // 0x0015
	uint8_t input_left;   // 0x0016
	uint8_t input_right;  // 0x0017
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
};


struct GlueData
{
	struct GamePSP psp;
};


// <!!!>

static struct GlueData s_glue;

uintptr_t g_psp_offset;      // Game code requires it, points to 's_glue.psp'
uintptr_t g_ifd_args_offset; // Game code requires it, points to 's_glue.psp::ifd_arg1'

void (*g_interrupt)(); // Where game calls
extern int GameMain(); // Where we, the engine, call

// </!!!>


static void sGameInterrupt()
{
	switch (s_glue.psp.ifd_arg1)
	{
	case 0x01: // GamePrintString
		break;
	case 0x02: // GamePrintNumber
		break;
	case 0x03: // GameLoadBackground
		break;
	case 0x05: // GameUnloadEverything
		break;
	case 0x06: // GameFlushCommands
		break;
	case 0x07: // GameLoadSprite
		break;
	case 0x08: // GameFreeSprite
		break;
	case 0x09: // GameExitRequest
		break;
	}
}


int GlueStart()
{
	s_glue.psp.max_commands = COMMANDS_TABLE_LEN;

	g_psp_offset = (uintptr_t)(&s_glue.psp);
	g_ifd_args_offset = (uintptr_t)(&(s_glue.psp.ifd_arg1));
	g_interrupt = sGameInterrupt;

	return 0;
}


void GlueStop() {}


#if 0
struct GameCommand
{
	uint8_t code;

	union
	{
		struct
		{
			uint8_t color;
			uint8_t width;
		};
		struct
		{
			uint8_t sprite;
			uint8_t frame;
		};
	};

	uint8_t height;
	uint16_t x;
	uint16_t y;
};


void DrawGameCommands(void* raw_cmd, size_t max_commands, const struct jaImage* bkg, struct jaImage* out)
{
	struct GameCommand* cmd = raw_cmd;
	struct GameCommand* end = cmd + max_commands;

	for (; cmd < end; cmd++)
	{
		switch (cmd->code)
		{
		case 0x00: // CODE_HALT
			return;
			break;
		case 0x01: // CODE_DRAW_BKG
			DrawBkg(bkg, out);
			break;
		case 0x02: // CODE_DRAW_RECTANGLE_BKG
			DrawRectangleBkg(cmd->width, cmd->height, cmd->x, cmd->y, bkg, out);
			break;
		case 0x03: // CODE_DRAW_SPRITE
			DrawSprite(cmd->sprite, cmd->x, cmd->y, cmd->frame, out);
			break;
		case 0x04: // CODE_DRAW_RECTANGLE
			DrawRectangle(cmd->width, cmd->height, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x05: // CODE_DRAW_RECTANGLE_PRECISE
			DrawRectanglePrecise(cmd->width, cmd->height, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x06: // CODE_DRAW_TEXT
			DrawText(cmd->sprite, cmd->x, cmd->y, "Nope", out);
			break;
		case 0x07: // CODE_DRAW_H_LINE
			DrawHLine(cmd->width, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x08: // CODE_DRAW_V_LINE
			DrawVLine(cmd->width, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x09: // CODE_DRAW_PIXEL
			DrawPixel(cmd->x, cmd->y, cmd->color, out);
			break;
		default: break;
		}
	}
}
#endif
