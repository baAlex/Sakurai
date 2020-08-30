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
#include <string.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "japan-image.h"
#include "japan-matrix.h"
#include "japan-version.h"
#include "kansai-context.h"
#include "kansai-version.h"

#include "cache.h"
#include "draw.h"
#include "game-glue.h"
#include "jvn.h"


#define NAME "Sakurai"
#define VERSION "0.3-alpha"
#define CAPTION "Sakurai v0.3-alpha"

#define SCREEN_WIDTH 320.0f // Used in OpenGL output
#define SCREEN_HEIGHT 240.0f
#define SCREEN_ASPECT (320.0f / 240.0f)

#define BUFFER_WIDTH 320 // Used in software raster
#define BUFFER_HEIGHT 200
#define BUFFER_LENGHT (320 * 200)

#define PALETTE_LEN 256             // Colors
#define PALETTE_SIZE 768            // Bytes
#define CACHE_SIZE 24 * 1024 * 1024 // Bytes


static char* s_vertex_code = "#version 100\n"
                             "attribute vec3 vertex_position; attribute vec2 vertex_uv;"
                             "uniform mat4 world; uniform mat4 local; uniform mat4 camera;"
                             "varying vec2 uv;"

                             "void main() { uv = vertex_uv;"
                             "gl_Position = world * local * camera * vec4(vertex_position, 1.0); }";

static char* s_fragment_code = "#version 100\n"
                               "uniform sampler2D texture0;"
                               "varying lowp vec2 uv;"

                               "void main() { gl_FragColor = texture2D(texture0, uv); }";


struct SakuraiData
{
	size_t last_update;

	uint8_t palette[PALETTE_SIZE];
	struct Cache* cache;

	struct jaImage* buffer_indexed;    // Indexed
	struct jaImage* buffer_background; // Indexed
	struct jaImage* buffer_color;      // RGBA

	struct kaTexture screen_texture;
	struct kaProgram screen_program;

	struct jaBuffer temp;
};


void sGameInterruption(struct GameInterruption i, uintptr_t* ret, void* raw_data)
{
	struct SakuraiData* data = raw_data;
	FILE* fp = NULL;
	struct JvnImage* sprite = NULL;
	struct CacheItem* item = NULL;

	switch (i.type)
	{
	case GAME_PRINT_STRING: printf("%s", i.string); break;
	case GAME_PRINT_NUMBER: printf("%u\n", i.number); break;
	case GAME_EXIT_REQUEST: printf("@ExitRequest\n"); break;

	case GAME_UNLOAD_EVERYTHING: CacheMarkAll(data->cache); break;

	case GAME_LOAD_BACKGROUND:
		if ((fp = fopen(i.filename, "rb")) != NULL) // TODO, report errors!
		{
			fread(data->buffer_background->data, BUFFER_LENGHT, 1, fp);
			fclose(fp);
		}
		break;

	case GAME_LOAD_SPRITE:
		if ((item = CacheFind(data->cache, i.filename)) != NULL)
		{
			*ret = (uintptr_t)item->ptr;
		}
		else if ((sprite = JvnImageLoad(i.filename, &data->temp, NULL)) != NULL) // TODO, report errors!
		{
			item = CacheAdd(data->cache, i.filename, 1, (void*)JvnImageDelete);
			item->ptr = sprite;

			*ret = (uintptr_t)item->ptr;
		}
		break;

	case GAME_FREE_SPRITE: printf("@FreeSprite\n"); break;
	}
}


static void sSakuraiInit(struct kaWindow* w, void* raw_data, struct jaStatus* st)
{
	struct SakuraiData* data = raw_data;
	FILE* fp = NULL;

	if (GlueStart(sGameInterruption, data) != 0)
	{
		jaStatusSet(st, "SakuraiInit", JA_STATUS_ERROR, "Glue code initialization", NULL);
		return;
	}

	// Create images to act as buffers
	if ((data->buffer_indexed = jaImageCreate(JA_IMAGE_U8, BUFFER_WIDTH, BUFFER_HEIGHT, 1)) == NULL ||
	    (data->buffer_background = jaImageCreate(JA_IMAGE_U8, BUFFER_WIDTH, BUFFER_HEIGHT, 1)) == NULL ||
	    (data->buffer_color = jaImageCreate(JA_IMAGE_U8, BUFFER_WIDTH, BUFFER_HEIGHT, 4)) == NULL)
	{
		jaStatusSet(st, "SakuraiInit", JA_STATUS_MEMORY_ERROR, "Image buffers", NULL);
		return;
	}

	memset(data->buffer_indexed->data, 0, data->buffer_indexed->size);
	memset(data->buffer_background->data, 0, data->buffer_background->size);
	memset(data->buffer_color->data, 0, data->buffer_color->size);

	// A texture as screen
	if (kaTextureInitImage(w, data->buffer_color, KA_FILTER_NONE, KA_CLAMP, &data->screen_texture, st) != 0)
		return;

	kaSetTexture(w, 0, &data->screen_texture);

	// A program
	if (kaProgramInit(w, s_vertex_code, s_fragment_code, &data->screen_program, st) != 0)
		return;

	kaSetProgram(w, &data->screen_program);

	// A palette
	if ((fp = fopen("assets/palette.raw", "rb")) == NULL)
	{
		jaStatusSet(st, "SakuraiInit", JA_STATUS_FS_ERROR, "Palette", NULL);
		return;
	}

	fread(data->palette, PALETTE_SIZE, 1, fp);
	fclose(fp);

	for (size_t i = 0; i < PALETTE_SIZE; i++) // Convert from 6 bits
		data->palette[i] = data->palette[i] << 2;

	// And a cache
	if ((data->cache = CacheCreate(CACHE_SIZE)) == NULL)
	{
		jaStatusSet(st, "SakuraiInit", JA_STATUS_MEMORY_ERROR, "Cache", NULL);
		return;
	}
}


static void sSakuraiFrame(struct kaWindow* w, struct kaEvents e, float delta, void* raw_data, struct jaStatus* st)
{
	struct SakuraiData* data = raw_data;
	(void)delta;
	(void)st;

	size_t current_ms = kaGetTime(w);

	if (current_ms >= data->last_update + 41) // 41 ms, hardcoded
	{
		// Call a game frame
		GlueFrame(e, current_ms, data->buffer_background, data->buffer_indexed);

		// Done for this frame
		DrawColorizeRGBX(data->palette, data->buffer_indexed, data->buffer_color);
		kaTextureUpdate(w, data->buffer_color, 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, &data->screen_texture);

		data->last_update = kaGetTime(w);
	}

	// Update screen
	kaDrawDefault(w);
}


static void sSakuraiResize(struct kaWindow* w, int width, int height, void* raw_data, struct jaStatus* st)
{
	(void)raw_data;
	(void)st;

	// Camera matrix, just centre the origin in the window middle
	kaSetCameraMatrix(w,
	                  jaMatrix4Orthographic(-((float)width / 2.0f), ((float)width / 2.0f), -((float)height / 2.0f),
	                                        ((float)height / 2.0f), 0.0f, 2.0f),
	                  jaVector3Clean());

	// Calculate a scale respecting the aspect ratio
	float scale =
	    ((float)width / (float)height > SCREEN_ASPECT) ? (float)height / SCREEN_HEIGHT : (float)width / SCREEN_WIDTH;

	// Scale the local matrix for the screen
	kaSetLocal(w, jaMatrix4ScaleAnsio(jaMatrix4Identity(),
	                                  (struct jaVector3){SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale, 1.0f}));
}


static void sSakuraiFunctionKey(struct kaWindow* w, int f, void* raw_data, struct jaStatus* st)
{
	(void)w;
	(void)raw_data;
	(void)st;

	if (f == 11)
		kaSwitchFullscreen(w);
}


static void sSakuraiClose(struct kaWindow* w, void* raw_data)
{
	struct SakuraiData* data = raw_data;

	GlueStop();

	jaImageDelete(data->buffer_indexed);
	jaImageDelete(data->buffer_background);
	jaImageDelete(data->buffer_color);

	kaTextureFree(w, &data->screen_texture);
	kaProgramFree(w, &data->screen_program);

	CacheDelete(data->cache);
	jaBufferClean(&data->temp);
}


int main(int argc, const char* argv[])
{
	struct jaStatus st = {0};
	struct SakuraiData* data = NULL;
	SDL_version sdl_ver;

	// Developers, developers, developers
	if (argc > 2 && strcmp("jvn2sgi", argv[1]) == 0)
		return Jvn2Sgi(argv[2]);

	if (argc > 1 && strcmp("test-cache", argv[1]) == 0)
		return CacheTest();

	// Game as normal
	SDL_GetVersion(&sdl_ver);

	printf("%s v%s\n", NAME, VERSION);
	printf(" - LibJapan %i.%i.%i\n", jaVersionMajor(), jaVersionMinor(), jaVersionPatch());
	printf(" - LibKansai %i.%i.%i\n", kaVersionMajor(), kaVersionMinor(), kaVersionPatch());
	printf(" - SDL2 %i.%i.%i\n", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);

	if ((data = calloc(1, sizeof(struct SakuraiData))) == NULL)
		goto return_failure;

	if (kaContextStart(NULL, &st) != 0)
		goto return_failure;

	if (kaWindowCreate(CAPTION, sSakuraiInit, sSakuraiFrame, sSakuraiResize, sSakuraiFunctionKey, sSakuraiClose, data,
	                   &st) != 0)
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
