/*-----------------------------

 [context-private.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef CONTEXT_PRIVATE_H
#define CONTEXT_PRIVATE_H

#include "context.h"
#include <string.h>

#include "glad/glad.h" // Before SDL2
#include <SDL2/SDL.h>


#define ATTRIBUTE_POSITION 10
#define ATTRIBUTE_UV 11


struct Context
{
	// ---- SDL2 side ----

	SDL_Window* window;
	SDL_GLContext* gl_context;

	bool cfg_vsync;

	struct ContextEvents events;

	// ---- Agnostic side ----

	bool cfg_wireframe;
	enum Filter cfg_filter;

	// Context State
	struct jaMatrix4 projection;
	struct jaMatrix4 camera;
	struct jaVector3 camera_origin;

	const struct Program* current_program;
	const struct Vertices* current_vertices;
	const struct Texture* current_texture;

	GLint u_projection;        // For current program
	GLint u_camera_projection; // "
	GLint u_camera_origin;     // "
	GLint u_texture[8];        // "
	GLint u_highlight;         // "

	struct Vertices aabb_vertices;
	struct Index aabb_index;
};

#endif
