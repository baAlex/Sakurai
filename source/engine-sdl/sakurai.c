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

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "japan-matrix.h"
#include "japan-version.h"
#include "kansai-context.h"
#include "kansai-version.h"

#define NAME "Sakurai"
#define VERSION "0.3-alpha"
#define CAPTION "Sakurai v0.3-alpha"


#define SCREEN_WIDTH 320.0f // Used in OpenGL output
#define SCREEN_HEIGHT 240.0f
#define SCREEN_ASPECT (320.0f / 240.0f)

#define BUFFER_WIDTH 320 // Used in software raster
#define BUFFER_HEIGHT 200
#define BUFFER_LENGHT (320 * 200)

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

	int screenshot_counter;
	size_t game_last_update;

	float screen_scale;
	struct kaTexture screen_texture;
	struct kaProgram screen_program;

	struct jaImage* buffer;
	uint8_t background[BUFFER_LENGHT];
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
		FILE* fp = fopen((char*)s_data.psp.ifd_arg2, "r");

		if (fp != NULL)
		{
			// For testing purposes just dump it into the buffer
			fread(s_data.buffer->data, BUFFER_LENGHT, 1, fp);
			fclose(fp);
		}

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

	// Create an image to act as a buffer
	if ((s_data.buffer = jaImageCreate(JA_IMAGE_U8, BUFFER_WIDTH, BUFFER_HEIGHT, 1)) == NULL)
	{
		st->code = JA_STATUS_ERROR;
		return;
	}

	// And a texture as screen, an unfiltered texture
	// (is better let the shader deal with interpolations, avoids mipmap generation
	// and allows non power of two dimensions)
	{
		if (kaTextureInitImage(w, s_data.buffer, KA_FILTER_NONE, &s_data.screen_texture, st) != 0)
			return;

		// https://www.khronos.org/webgl/wiki/WebGL_and_OpenGL_Differences#Non-Power_of_Two_Texture_Support
		// FIXME, i need to set the warp to 'CLAMP_TO_EDGE'
	}

	kaSetTexture(w, 0, &s_data.screen_texture);

	// Finally a program
	{
		const char* vertex_code =
		    "#version 100\n"
		    "attribute vec3 vertex_position; attribute vec2 vertex_uv;"
		    "uniform mat4 world; uniform mat4 camera; uniform vec3 camera_position;"
		    "uniform vec3 local_position; uniform vec3 local_scale;"
		    "varying vec2 uv;"

		    "void main() { uv = vertex_uv;"
		    "gl_Position = world * camera * vec4(local_position + (vertex_position * local_scale), 1.0); }";

		const char* fragment_code = "#version 100\n"
		                            "uniform sampler2D texture0;"
		                            "varying lowp vec2 uv;"

		                            "void main() { gl_FragColor = texture2D(texture0, uv); }";

		if (kaProgramInit(w, vertex_code, fragment_code, &s_data.screen_program, st) != 0)
			return;

		kaSetProgram(w, &s_data.screen_program);
	}

	// First game call!
	GameMain();
}


static void sFrame(struct kaWindow* w, struct kaEvents e, float delta, void* raw_data, struct jaStatus* st)
{
	(void)e;
	(void)delta;
	(void)raw_data;
	(void)st;

	size_t current = kaGetTime(w);

	// Call a game frame every 41 ms
	if (current >= s_data.game_last_update + 41)
	{
		s_data.psp.ms_counter = (uint16_t)current;
		s_data.psp.frame_counter += 1;
		GameMain();

		kaTextureUpdate(w, s_data.buffer, 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, &s_data.screen_texture);
		s_data.game_last_update = kaGetTime(w);
	}

	// Update screen
	struct jaVector3 scale = {SCREEN_WIDTH * s_data.screen_scale, SCREEN_HEIGHT * s_data.screen_scale, 1.0f};
	kaDrawSprite(w, (struct jaVector3){0.0f, 0.0f, 0.0f}, scale);
}


static void sResize(struct kaWindow* w, int width, int height, void* raw_data, struct jaStatus* st)
{
	(void)raw_data;
	(void)st;

	// A value to fit the screen in the window
	s_data.screen_scale =
	    ((float)width / (float)height > SCREEN_ASPECT) ? (float)height / SCREEN_HEIGHT : (float)width / SCREEN_WIDTH;

	// Keeping it in the middle
	kaSetWorld(w, jaMatrix4Orthographic(-((float)width / 2.0f), ((float)width / 2.0f), -((float)height / 2.0f),
	                                    ((float)height / 2.0f), 0.0f, 2.0f));
}


static void sFunction(struct kaWindow* w, int f, void* raw_data, struct jaStatus* st)
{
	(void)w;
	(void)raw_data;

	if (f == 11)
		kaSwitchFullscreen(w);

	else if (f == 12)
	{
		char tstr[64];
		char filename[256];

		time_t t;
		time(&t);

		strftime(tstr, 64, "%Y-%m-%e, %H:%M:%S", localtime(&t));
		sprintf(filename, "%s - %s (%i).sgi", NAME, tstr, s_data.screenshot_counter);

		s_data.screenshot_counter += 1;

		if (jaImageSaveSgi(s_data.buffer, filename, st) != 0)
			return;
	}
}


static void sClose(struct kaWindow* w, void* raw_data)
{
	(void)raw_data;

	kaTextureFree(w, &s_data.screen_texture);
	kaProgramFree(w, &s_data.screen_program);
	jaImageDelete(s_data.buffer);
}


int main()
{
	struct jaStatus st = {0};

	printf("%s v%s\n", NAME, VERSION);
	printf(" - LibJapan %i.%i.%i\n", jaVersionMajor(), jaVersionMinor(), jaVersionPatch());
	printf(" - LibKansai %i.%i.%i\n", kaVersionMajor(), kaVersionMinor(), kaVersionPatch());

	if (kaContextStart(NULL, &st) != 0)
		goto return_failure;

	if (kaWindowCreate(CAPTION, sInit, sFrame, sResize, sFunction, sClose, NULL, &st) != 0)
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
