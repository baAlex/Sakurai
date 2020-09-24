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

#define COMMANDS_TABLE_LEN 64   // 28 in the DOS engine
#define COMMANDS_TABLE_SIZE 512 // 224 in the DOS engine
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

	// --- Roughly DOS-compatible PSP ends here ---

	// What follow replaces the 'uint16_t' DOS ifd arguments,
	// as these are intended to pass pointers and in modern
	// platform we have extra thicc addresses
	uintptr_t ifd_arg1;
	uintptr_t ifd_arg2;
	uintptr_t ifd_arg3;
	uintptr_t ifd_arg4;

	char* text_stack[COMMANDS_TABLE_LEN];
};


struct GameCommand
{
	uint8_t code;
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint16_t d;
	uint16_t e;
};


struct GlueData
{
	struct GamePSP psp;

	uintptr_t (*callback_func)(struct GameInterruption, void*, struct jaStatus*);
	void* callback_data;
	struct jaStatus callback_st;

	uintptr_t sprite_indirection[SPRITE_INDIRECTION_LEN];
};


// <!!!>

static struct GlueData s_glue;

uintptr_t g_psp_offset;        // Game code requires it, points to 's_glue.psp'
uintptr_t g_ifd_args_offset;   // Game code requires it, points to 's_glue.psp::ifd_arg1'
uintptr_t g_text_stack_offset; // Game code requires it, points to 's_glue.psp::text_stack'

void (*g_interrupt)();  // Where game calls
extern void GameMain(); // Where we, the engine, call

// </!!!>


static void sGameInterrupt()
{
	struct GameInterruption game_int = {0};
	uintptr_t engine_ret = 0;

	// Don't handle new interruptions if the last one wasn't successful
	if (s_glue.callback_st.code != JA_STATUS_SUCCESS)
		return;

	// Make a lovely abstraction for the engine
	// (aka: convert the raw psp values into a 'struct GameInterruption')
	switch (s_glue.psp.ifd_arg1)
	{
	case 0x01: // GamePrintString
		game_int.type = GAME_PRINT_STRING;
		game_int.string = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);
		break;

	case 0x02: // GamePrintNumber
		game_int.type = GAME_PRINT_NUMBER;
		game_int.number = (unsigned)s_glue.psp.ifd_arg2;
		s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);
		break;

	case 0x03: // GameLoadBackground
		game_int.type = GAME_LOAD_BACKGROUND;
		game_int.filename = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);
		break;

	case 0x04: // GameLoadPalette
		game_int.type = GAME_LOAD_PALETTE;
		game_int.filename = (const char*)s_glue.psp.ifd_arg2;
		s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);
		break;

	case 0x05: // GameUnloadEverything
		game_int.type = GAME_UNLOAD_EVERYTHING;
		s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);

		for (int t = 0; t < SPRITE_INDIRECTION_LEN; t++)
		{
			// We assume that the engine unloaded everything on his side
			s_glue.sprite_indirection[t] = 0;
		}

		break;

	case 0x07: // GameLoadSprite
		game_int.type = GAME_LOAD_SPRITE;
		game_int.filename = (const char*)s_glue.psp.ifd_arg2;
		engine_ret = s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);

		if (s_glue.callback_st.code != JA_STATUS_SUCCESS) // Engine had problems loading the sprite
			return;

		for (int t = 0; t < SPRITE_INDIRECTION_LEN; t++)
		{
			if (s_glue.sprite_indirection[t] == 0 || s_glue.sprite_indirection[t] == engine_ret)
			{
				s_glue.sprite_indirection[t] = engine_ret;
				s_glue.psp.ifd_arg1 = t; // Value for the game
				break;
			}
		}

		break;

	case 0x09: // GameExitRequest
		game_int.type = GAME_EXIT_REQUEST;
		s_glue.callback_func(game_int, s_glue.callback_data, &s_glue.callback_st);
		break;
	}
}


void GlueStart(uintptr_t (*callback_func)(struct GameInterruption, void*, struct jaStatus*), void* callback_data)
{
	s_glue.psp.max_commands = COMMANDS_TABLE_LEN;
	s_glue.callback_func = callback_func;
	s_glue.callback_data = callback_data;

	g_psp_offset = (uintptr_t)(&s_glue.psp);
	g_ifd_args_offset = (uintptr_t)(&(s_glue.psp.ifd_arg1));
	g_text_stack_offset = (uintptr_t)(&(s_glue.psp.text_stack));
	g_interrupt = sGameInterrupt;
}


void GlueStop() {}


int GlueFrame(struct kaEvents ev, size_t ms, const struct jaImage* buffer_background, struct jaImage* buffer_out,
              struct jaStatus* st)
{
	struct GameCommand* cmd = (struct GameCommand*)s_glue.psp.commands_table;
	struct GameCommand* end = cmd + COMMANDS_TABLE_LEN;

	s_glue.psp.ms_counter = (uint16_t)(ms % UINT16_MAX); // Imitating DOS behaviour

	s_glue.psp.input_x = ((ev.x == true) ? 1 : 0) | ((ev.a == true) ? 1 : 0);
	s_glue.psp.input_y = ((ev.y == true) ? 1 : 0) | ((ev.b == true) ? 1 : 0);
	s_glue.psp.input_pad_l = (ev.pad_l == true) ? 1 : 0;
	s_glue.psp.input_pad_r = (ev.pad_r == true) ? 1 : 0;
	s_glue.psp.input_pad_u = (ev.pad_u == true) ? 1 : 0;
	s_glue.psp.input_pad_d = (ev.pad_d == true) ? 1 : 0;
	s_glue.psp.input_start = (ev.start == true) ? 1 : 0;
	s_glue.psp.input_select = (ev.select == true) ? 1 : 0;

	GameMain();
	s_glue.psp.frame_counter += 1;

	// Interruptions had an happy ending?
	if (s_glue.callback_st.code != JA_STATUS_SUCCESS)
	{
		jaStatusCopy(&s_glue.callback_st, st);
		return 1;
	}

	// Iterate game commands
	for (; cmd < end; cmd++)
	{
		switch (cmd->code)
		{
		case 0x00: // CODE_HALT
			return 0;
			break;
		case 0x01: // CODE_DRAW_BKG
			DrawBkg(buffer_background, buffer_out);
			break;
		case 0x02: // CODE_DRAW_RECTANGLE_BKG
			DrawRectangleBkg(cmd->b, cmd->c, cmd->d, cmd->e, buffer_background, buffer_out);
			break;
		case 0x03: // CODE_DRAW_SPRITE
			DrawSprite((struct JvnImage*)s_glue.sprite_indirection[cmd->a], cmd->d, cmd->e, cmd->b, buffer_out);
			break;
		case 0x04: // CODE_DRAW_RECTANGLE
			DrawRectangle(cmd->b, cmd->c, cmd->d, cmd->e, cmd->a, buffer_out);
			break;
		case 0x05: // CODE_DRAW_RECTANGLE_PRECISE
			DrawRectanglePrecise(cmd->b, cmd->c, cmd->d, cmd->e, cmd->a, buffer_out);
			break;
		case 0x06: // CODE_DRAW_TEXT
			DrawText((struct JvnImage*)s_glue.sprite_indirection[cmd->a], cmd->d, cmd->e,
			         s_glue.psp.text_stack[(cmd - (struct GameCommand*)s_glue.psp.commands_table)], buffer_out);
			break;
		case 0x07: // CODE_DRAW_H_LINE
			DrawHLine(cmd->b, cmd->d, cmd->e, cmd->a, buffer_out);
			break;
		case 0x08: // CODE_DRAW_V_LINE
			DrawVLine(cmd->b, cmd->d, cmd->e, cmd->a, buffer_out);
			break;
		case 0x09: // CODE_DRAW_PIXEL
			DrawPixel(cmd->d, cmd->e, cmd->a, buffer_out);
			break;
		default: break;
		}
	}

	return 0;
}
