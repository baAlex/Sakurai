/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

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

 [context-sdl2.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "context-private.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_MIN_WIDTH 320
#define WINDOW_MIN_HEIGHT 240


static int s_sdl_references = 0;


struct Context* ContextCreate(const struct jaConfiguration* cfg, const char* caption, struct jaStatus* st)
{
	struct Context* context = NULL;
	SDL_version sdl_version = {0};

	jaStatusSet(st, "ContextCreate", JA_STATUS_SUCCESS, NULL);

	if ((context = malloc(sizeof(struct Context))) == NULL)
	{
		jaStatusSet(st, "ContextCreate", JA_STATUS_MEMORY_ERROR, NULL);
		goto return_failure;
	}

	memset(context, 0, sizeof(struct Context));

	// Print version
	SDL_GetVersion(&sdl_version);
	printf(" - LibSDL2 %i.%i.%i\n", sdl_version.major, sdl_version.minor, sdl_version.patch);

	// SDL Initialization
	s_sdl_references += 1;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		s_sdl_references -= 1;

		fprintf(stderr, "\n%s\n", SDL_GetError());
		jaStatusSet(st, "ContextCreate", JA_STATUS_ERROR, "SDL_Init()");
		goto return_failure;
	}

	if ((context->window = SDL_CreateWindow(caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
	                                        WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) == NULL)
	{
		fprintf(stderr, "\n%s\n", SDL_GetError());
		jaStatusSet(st, "ContextCreate", JA_STATUS_ERROR, "SDL_CreateWindow()");
		goto return_failure;
	}

	if ((context->gl_context = SDL_GL_CreateContext(context->window)) == NULL)
	{
		fprintf(stderr, "\n%s\n", SDL_GetError());
		jaStatusSet(st, "ContextCreate", JA_STATUS_ERROR, "SDL_GL_CreateContext()");
		goto return_failure;
	}

	SDL_SetWindowMinimumSize(context->window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
	SDL_GL_SetSwapInterval((context->cfg_vsync == true) ? 1 : 0);

	// Initialize GLAD (after context creation)
	if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
	{
		jaStatusSet(st, "ContextCreate", JA_STATUS_ERROR, "gladLoad()");
		goto return_failure;
	}

	printf(" - %s\n", glGetString(GL_VENDOR));
	printf(" - %s\n", glGetString(GL_RENDERER));
	printf(" - %s\n\n", glGetString(GL_VERSION));

	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glEnableVertexAttribArray(ATTRIBUTE_POSITION);
	glEnableVertexAttribArray(ATTRIBUTE_UV);

	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(context->window);

	// Bye!
	context->events.window_size.x = WINDOW_WIDTH;
	context->events.window_size.y = WINDOW_HEIGHT;

	return context;

return_failure:
	if (context != NULL)
		ContextDelete(context);

	return NULL;
}


void ContextDelete(struct Context* context)
{
	if (context->gl_context != NULL)
		SDL_GL_DeleteContext(context->gl_context);

	if (context->window != NULL)
		SDL_DestroyWindow(context->window);

	free(context);

	if (s_sdl_references > 0)
	{
		s_sdl_references -= 1;

		if (s_sdl_references == 0)
			SDL_Quit();
	}
}


int ContextUpdate(struct Context* context, struct ContextEvents* out_events, struct jaStatus* st)
{
	SDL_Event e = {0};

	// Update screen
	SDL_GL_SwapWindow(context->window);
	glClear(GL_COLOR_BUFFER_BIT);

	// Receive input
	context->events.close = false;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_CLOSE)
				context->events.close = true;
		}
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.scancode)
			{
			case SDL_SCANCODE_A: context->events.a = true; break;
			case SDL_SCANCODE_S: context->events.b = true; break;
			case SDL_SCANCODE_Z: context->events.x = true; break;
			case SDL_SCANCODE_X: context->events.y = true; break;
			case SDL_SCANCODE_Q: context->events.lb = true; break;
			case SDL_SCANCODE_W: context->events.rb = true; break;
			default: break;
			}
		}
		else if (e.type == SDL_KEYUP)
		{
			switch (e.key.keysym.scancode)
			{
			case SDL_SCANCODE_A: context->events.a = false; break;
			case SDL_SCANCODE_S: context->events.b = false; break;
			case SDL_SCANCODE_Z: context->events.x = false; break;
			case SDL_SCANCODE_X: context->events.y = false; break;
			case SDL_SCANCODE_Q: context->events.lb = false; break;
			case SDL_SCANCODE_W: context->events.rb = false; break;
			default: break;
			}
		}
	}

	// Bye!
	if (out_events != NULL)
		memcpy(out_events, &context->events, sizeof(struct ContextEvents));

	return 0;
}


int TakeScreenshot(const struct Context* context, const char* filename, struct jaStatus* st)
{
	struct jaImage* image = NULL;
	GLenum error;

	if ((image = jaImageCreate(JA_IMAGE_U8, (size_t)context->events.window_size.x,
	                           (size_t)context->events.window_size.y + 1, 4)) == NULL)
	{
		jaStatusSet(st, "TakeScreenshot", JA_STATUS_MEMORY_ERROR, NULL);
		goto return_failure;
	}

	glReadPixels(0, 0, context->events.window_size.x, context->events.window_size.y, GL_RGBA, GL_UNSIGNED_BYTE,
	             image->data);

	if ((error = glGetError()) != GL_NO_ERROR)
	{
		// TODO, glReadPixels has tons of corners where it can fail.
		jaStatusSet(st, "TakeScreenshot", JA_STATUS_ERROR, NULL);
		goto return_failure;
	}

	for (size_t i = 0;; i++)
	{
		if (i >= (image->height - 1) / 2)
			break;

		// The image has an extra row
		memcpy((uint8_t*)image->data + (image->width * 4) * (image->height - 1),
		       (uint8_t*)image->data + (image->width * 4) * i, (image->width * 4));
		memcpy((uint8_t*)image->data + (image->width * 4) * i,
		       (uint8_t*)image->data + (image->width * 4) * (image->height - 2 - i), (image->width * 4));
		memcpy((uint8_t*)image->data + (image->width * 4) * (image->height - 2 - i),
		       (uint8_t*)image->data + (image->width * 4) * (image->height - 1), (image->width * 4));
	}

	image->height -= 1; // ;)

	if ((jaImageSaveSgi(image, filename, st)) != 0)
		goto return_failure;

	jaImageDelete(image);
	return 0;

return_failure:
	if (image != NULL)
		jaImageDelete(image);

	return 1;
}
