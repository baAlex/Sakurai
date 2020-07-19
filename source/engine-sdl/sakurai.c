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


// The game require:
size_t g_psp_offset;
void (*g_interrupt)();

// We, the engine, require:
extern int GameMain();


struct GameData
{
	// Game PSP, the game expect it in the following format/order
	// https://en.wikipedia.org/wiki/Program_Segment_Prefix
	uint16_t empty_word;
	uint16_t frame_counter;
	uint16_t ms_counter;
	uint16_t max_commands;
	uint16_t ifd_arg1;
	uint16_t ifd_arg2;
	uint16_t ifd_arg3;
	uint16_t ifd_arg4;

	uint8_t input_x;
	uint8_t input_y;
	uint8_t input_a;
	uint8_t input_b;
	uint8_t input_up;
	uint8_t input_down;
	uint8_t input_left;
	uint8_t input_right;
	uint8_t input_select;
	uint8_t input_start;

	uint8_t commands_table[COMMANDS_TABLE_LEN];
};


static void sInterrupt()
{
	struct GameData* data = ((void*)g_psp_offset);

	switch (data->ifd_arg1)
	{
	case 0x01: // GamePrintString
		printf("@GamePrintString\n");
		break;
	case 0x02: // GamePrintNumber
		printf("@GamePrintNumber: %u\n", data->ifd_arg2);
		break;
	case 0x03: // GameLoadBackground
		printf("@GameLoadBackground\n");
		break;
	case 0x05: // GameUnloadEverything
		printf("@GameUnloadEverything\n");
		break;
	case 0x06: // GameFlushCommands
		printf("@GameFlushCommands\n");
		break;
	case 0x07: // GameLoadSprite
		printf("@GameLoadSprite\n");
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
	(void)st;

	// Game PSP
	struct GameData* data = raw_data;
	data->max_commands = COMMANDS_TABLE_LEN;

	g_psp_offset = &(data->empty_word); // My eyes!
	g_interrupt = sInterrupt;

	// Kansai boilerplate
	kaSetWorld(w, jaMatrix4Orthographic(0.0f, 320.0f, 0.0f, 240.0f, 0.0f, 2.0f));
	kaSetCameraMatrix(w, jaMatrix4Identity(), jaVector3Clean());

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
	struct GameData* data = NULL;

	printf("%s v%s\n", NAME, VERSION);
	printf(" - LibJapan %i.%i.%i\n", jaVersionMajor(), jaVersionMinor(), jaVersionPatch());
	printf(" - LibKansai %i.%i.%i\n", kaVersionMajor(), kaVersionMinor(), kaVersionPatch());

	if ((data = calloc(1, sizeof(struct GameData))) == NULL)
	{
		jaStatusSet(&st, NAME, JA_STATUS_MEMORY_ERROR, NULL);
		goto return_failure;
	}

	if (kaContextStart(NULL, &st) != 0)
		goto return_failure;

	if (kaWindowCreate(CAPTION, sInit, sFrame, NULL, NULL, sClose, data, &st) != 0)
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
