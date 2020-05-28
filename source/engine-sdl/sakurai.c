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
#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "japan-status.h"

#include <SDL2/SDL.h>


#define NAME "Sakurai"
#define VERSION "0.2"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_MIN_WIDTH 320
#define WINDOW_MIN_HEIGHT 240


int main()
{
	struct jaStatus st = {0};

	// Hello world!
	SDL_version sdl_version = {0};
	SDL_GetVersion(&sdl_version);

	printf("%s v%s\n", NAME, VERSION);
	printf("- LibJapan %i.%i.%i\n", JAPAN_VERSION_MAJOR, JAPAN_VERSION_MINOR, JAPAN_VERSION_PATCH);
	printf("- LibSDL2 %i.%i.%i\n", sdl_version.major, sdl_version.minor, sdl_version.patch);

	// Initialize SDL
	int sdl_initialized = 0;
	SDL_Window* window = NULL;
	SDL_GLContext* context = NULL;

	if ((sdl_initialized = SDL_Init(SDL_INIT_VIDEO)) != 0)
	{
		fprintf(stderr, "\n%s\n", SDL_GetError());
		jaStatusSet(&st, "main", JA_STATUS_ERROR, "SDL_Init()");
		goto return_failure;
	}

	if ((window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
	                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) == NULL)
	{
		fprintf(stderr, "\n%s\n", SDL_GetError());
		jaStatusSet(&st, "main", JA_STATUS_ERROR, "SDL_CreateWindow()");
		goto return_failure;
	}

	if ((context = SDL_GL_CreateContext(window)) == NULL)
	{
		fprintf(stderr, "\n%s\n", SDL_GetError());
		jaStatusSet(&st, "main", JA_STATUS_ERROR, "SDL_GL_CreateContext()");
		goto return_failure;
	}

	SDL_SetWindowMinimumSize(window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
	SDL_GL_SetSwapInterval(1);

	// Initialize GLAD, after context creation
	if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
	{
		jaStatusSet(&st, "main", JA_STATUS_ERROR, "gladLoad()");
		goto return_failure;
	}

	printf("\n%s\n", glGetString(GL_VENDOR));
	printf("%s\n", glGetString(GL_RENDERER));
	printf("%s\n\n", glGetString(GL_VERSION));

	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);

	// Main loop
	SDL_Event e = {0};
	bool exit = false;

	while (1)
	{
		// Receive input
		if (SDL_WaitEvent(&e) == 0) // Blocks!
		{
			fprintf(stderr, "\n%s\n", SDL_GetError());
			jaStatusSet(&st, "main", JA_STATUS_ERROR, "SDL_WaitEvent()");
			goto return_failure;
		}

		do
		{
			if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_CLOSE)
					exit = true;
			}
		} while (SDL_PollEvent(&e) != 0);

		// Exit?
		if (exit == true)
			break;

		// Update screen
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
	}

	// Bye!
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("Bye!\n");
	return EXIT_SUCCESS;

return_failure:
	if (context != NULL)
		SDL_GL_DeleteContext(context);
	if (window != NULL)
		SDL_DestroyWindow(window);
	if (sdl_initialized != 0)
		SDL_Quit();

	jaStatusPrint(NAME, st);
	return EXIT_FAILURE;
}
