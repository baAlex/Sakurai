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

 [sakurai.c]
 - Alexander Brandt 2020
-----------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "japan-matrix.h"
#include "japan-version.h"
#include "kansai-context.h"
#include "kansai-version.h"

#define NAME "Sakurai"
#define VERSION "0.3-alpha"
#define CAPTION "Sakurai v0.3-alpha"


#define COMMAND_SIZE 8
#define COMMANDS_TABLE_LEN 64   // 28 in the Dos engine
#define COMMANDS_TABLE_SIZE 512 // 224 in the Dos engine


struct EngineData
{
	// Game PSP, counterpart of the same structure in 'shared.inc'
	// https://en.wikipedia.org/wiki/Program_Segment_Prefix
	struct
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

		uint8_t commands_table[COMMANDS_TABLE_LEN]; // 0x0020

		// --- Roughly Dos-compatible PSP ends here ---

		// What follow replaces the 'uint16_t' Dos ifd arguments,
		// as these are intended to pass pointers and in modern
		// platform we have extra thicc addresses
		uintptr_t ifd_arg1;
		uintptr_t ifd_arg2;
		uintptr_t ifd_arg3;
		uintptr_t ifd_arg4;
	} psp;
};


// <!!!>

uintptr_t g_psp_offset;
uintptr_t g_ifd_args_offset;
void (*g_interrupt)();

static struct EngineData s_data;
extern int GameMain();

// </!!!>


static void sInterrupt()
{
	switch (s_data.psp.ifd_arg1)
	{
	case 0x01: // GamePrintString
		printf("%s", (char*)s_data.psp.ifd_arg2);
		break;
	case 0x02: // GamePrintNumber
		printf("%u\n", (uint16_t)s_data.psp.ifd_arg2);
		break;
	case 0x03: // GameLoadBackground
		printf("@GameLoadBackground: '%s'\n", (char*)s_data.psp.ifd_arg2);
		break;
	case 0x05: // GameUnloadEverything
		printf("@GameUnloadEverything\n");
		break;
	case 0x06: // GameFlushCommands
		printf("@GameFlushCommands\n");
		break;
	case 0x07: // GameLoadSprite
		printf("@GameLoadSprite: '%s'\n", (char*)s_data.psp.ifd_arg2);
		break;
	case 0x08: // GameFreeSprite
		printf("@GameFreeSprite\n");
		break;
	case 0x09: // GameExitRequest
		printf("@GameExitRequest\n");
		break;
	}
}


static void sInit(struct kaWindow* w, void* raw_data, struct jaStatus* st)
{
	(void)raw_data;
	(void)st;

	s_data.psp.max_commands = COMMANDS_TABLE_LEN;

	g_psp_offset = (uintptr_t)(&(s_data.psp));
	g_ifd_args_offset = (uintptr_t)(&(s_data.psp.ifd_arg1));
	g_interrupt = sInterrupt;

	// Kansai boilerplate
	kaSetWorld(w, jaMatrix4Orthographic(0.0f, 320.0f, 0.0f, 240.0f, 0.0f, 2.0f));
	kaSetCameraMatrix(w, jaMatrix4Identity(), jaVector3Clean());

	GameMain();
	GameMain();
	GameMain();
	GameMain();
}


static void sFrame(struct kaWindow* w, struct kaEvents e, float delta, void* raw_data, struct jaStatus* st)
{
	(void)e;
	(void)delta;
	(void)raw_data;
	(void)st;
	kaDrawSprite(w, (struct jaVector3){160.0f, 120.0f, 0.0f}, (struct jaVector3){320.0f, 240.f, 1.0f});
}


static void sClose(struct kaWindow* w, void* raw_data)
{
	(void)w;
	(void)raw_data;
}


int main()
{
	struct jaStatus st = {0};

	printf("%s v%s\n", NAME, VERSION);
	printf(" - LibJapan %i.%i.%i\n", jaVersionMajor(), jaVersionMinor(), jaVersionPatch());
	printf(" - LibKansai %i.%i.%i\n", kaVersionMajor(), kaVersionMinor(), kaVersionPatch());

	if (kaContextStart(NULL, &st) != 0)
		goto return_failure;

	if (kaWindowCreate(CAPTION, sInit, sFrame, NULL, NULL, sClose, NULL, &st) != 0)
		goto return_failure;

	while (1)
	{
		if (kaContextUpdate(&st) != 0)
			break;
	}

	if (st.code != JA_STATUS_SUCCESS)
		goto return_failure;

	// Bye!
	kaContextStop();
	return EXIT_SUCCESS;

return_failure:
	jaStatusPrint(NAME, st);
	kaContextStop();
	return EXIT_FAILURE;
}
