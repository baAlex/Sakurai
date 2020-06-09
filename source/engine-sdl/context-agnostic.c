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

 [context-agnostic.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "context-private.h"


/*-----------------------------

 Context state
-----------------------------*/
inline void SetProgram(struct Context* context, const struct Program* program)
{
	if (program != context->current_program)
	{
		context->current_program = program;
		context->u_projection = glGetUniformLocation(program->glptr, "projection");
		context->u_camera_projection = glGetUniformLocation(program->glptr, "camera_projection");
		context->u_camera_origin = glGetUniformLocation(program->glptr, "camera_origin");
		context->u_highlight = glGetUniformLocation(program->glptr, "highlight");

		context->u_texture[0] = glGetUniformLocation(program->glptr, "texture0");
		context->u_texture[1] = glGetUniformLocation(program->glptr, "texture1");
		context->u_texture[2] = glGetUniformLocation(program->glptr, "texture2");
		context->u_texture[3] = glGetUniformLocation(program->glptr, "texture3");
		context->u_texture[4] = glGetUniformLocation(program->glptr, "texture4");
		context->u_texture[5] = glGetUniformLocation(program->glptr, "texture5");
		context->u_texture[6] = glGetUniformLocation(program->glptr, "texture6");
		context->u_texture[7] = glGetUniformLocation(program->glptr, "texture7");

		glUseProgram(program->glptr);

		glUniformMatrix4fv(context->u_projection, 1, GL_FALSE, &context->projection.e[0][0]);
		glUniformMatrix4fv(context->u_camera_projection, 1, GL_FALSE, &context->camera.e[0][0]);
		glUniform3fv(context->u_camera_origin, 1, (float*)&context->camera_origin);
		glUniform1i(context->u_texture[0], 0);
		glUniform1i(context->u_texture[1], 1);
		glUniform1i(context->u_texture[2], 2);
		glUniform1i(context->u_texture[3], 3);
		glUniform1i(context->u_texture[4], 4);
		glUniform1i(context->u_texture[5], 5);
		glUniform1i(context->u_texture[6], 6);
		glUniform1i(context->u_texture[7], 7);
	}
}


inline void SetVertices(struct Context* context, const struct Vertices* vertices)
{
	if (vertices != context->current_vertices)
	{
		context->current_vertices = vertices;

		glBindBuffer(GL_ARRAY_BUFFER, vertices->glptr);
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), NULL);
		glVertexAttribPointer(ATTRIBUTE_UV, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), ((float*)NULL) + 3);
	}
}


inline void SetTexture(struct Context* context, int unit, const struct Texture* texture)
{
	if (texture != context->current_texture)
	{
		context->current_texture = texture;

		glActiveTexture((GLenum)(GL_TEXTURE0 + unit));
		glBindTexture(GL_TEXTURE_2D, texture->glptr);
	}
}


inline void SetProjection(struct Context* context, struct jaMatrix4 matrix)
{
	memcpy(&context->projection, &matrix, sizeof(struct jaMatrix4));

	if (context->current_program != NULL)
		glUniformMatrix4fv(context->u_projection, 1, GL_FALSE, &context->projection.e[0][0]);
}


inline void SetCameraLookAt(struct Context* context, struct jaVector3 target, struct jaVector3 origin)
{
	context->camera_origin = origin;
	context->camera = jaMatrix4LookAt(origin, target, (struct jaVector3){0.0f, 0.0f, 1.0f});

	if (context->current_program != NULL)
	{
		glUniformMatrix4fv(context->u_camera_projection, 1, GL_FALSE, &context->camera.e[0][0]);
		glUniform3fv(context->u_camera_origin, 1, (float*)&context->camera_origin);
	}
}


inline void SetCameraMatrix(struct Context* context, struct jaMatrix4 matrix, struct jaVector3 origin)
{
	context->camera_origin = origin;
	context->camera = matrix;

	if (context->current_program != NULL)
	{
		glUniformMatrix4fv(context->u_camera_projection, 1, GL_FALSE, &context->camera.e[0][0]);
		glUniform3fv(context->u_camera_origin, 1, (float*)&context->camera_origin);
	}
}


inline void Draw(struct Context* context, const struct Index* index)
{
	(void)context;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index->glptr);
	glDrawElements((context->cfg_wireframe == false) ? GL_TRIANGLES : GL_LINES, (GLsizei)index->length,
	               GL_UNSIGNED_SHORT, NULL);
}


inline void DrawAABB(struct Context* context, struct jaAABBox box, struct jaVector3 pos)
{
	const struct Vertices* old_vertices = context->current_vertices;
	{
		SetVertices(context, &context->aabb_vertices);
		Draw(context, &context->aabb_index);
	}
	SetVertices(context, old_vertices);
}


/*-----------------------------

 OpenGL abstractions
-----------------------------*/
static inline int sCompileShader(GLuint shader, struct jaStatus* st)
{
	GLint success = GL_FALSE;

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		jaStatusSet(st, "ProgramInit", JA_STATUS_ERROR, NULL);
		glGetShaderInfoLog(shader, JA_STATUS_EXPL_LEN, NULL, st->explanation);
		return 1;
	}

	return 0;
}


int ProgramInit(const char* vertex_code, const char* fragment_code, struct Program* out, struct jaStatus* st)
{
	GLint success = GL_FALSE;
	GLuint vertex = 0;
	GLuint fragment = 0;

	jaStatusSet(st, "ProgramInit", JA_STATUS_SUCCESS, NULL);

	// Compile shaders
	if ((vertex = glCreateShader(GL_VERTEX_SHADER)) == 0 || (fragment = glCreateShader(GL_FRAGMENT_SHADER)) == 0)
	{
		jaStatusSet(st, "ProgramInit", JA_STATUS_ERROR, "Creating GL shader\n");
		goto return_failure;
	}

	glShaderSource(vertex, 1, &vertex_code, NULL);
	glShaderSource(fragment, 1, &fragment_code, NULL);

	if (sCompileShader(vertex, st) != 0 || sCompileShader(fragment, st) != 0)
		goto return_failure;

	// Create program
	if ((out->glptr = glCreateProgram()) == 0)
	{
		jaStatusSet(st, "ProgramInit", JA_STATUS_ERROR, "Creating GL program\n");
		goto return_failure;
	}

	glAttachShader(out->glptr, vertex);
	glAttachShader(out->glptr, fragment);

	glBindAttribLocation(out->glptr, ATTRIBUTE_POSITION, "vertex_position"); // Before link!
	glBindAttribLocation(out->glptr, ATTRIBUTE_UV, "vertex_uv");

	// Link
	glLinkProgram(out->glptr);
	glGetProgramiv(out->glptr, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		jaStatusSet(st, "ProgramInit", JA_STATUS_ERROR, NULL);
		glGetProgramInfoLog(out->glptr, JA_STATUS_EXPL_LEN, NULL, st->explanation);
		goto return_failure;
	}

	// Bye!
	glDeleteShader(vertex); // Set shader to be deleted when glDeleteProgram() happens
	glDeleteShader(fragment);
	return 0;

return_failure:
	if (vertex != 0)
		glDeleteShader(vertex);
	if (fragment != 0)
		glDeleteShader(fragment);
	if (out->glptr != 0)
		glDeleteProgram(out->glptr);

	return 1;
}


inline void ProgramFree(struct Program* program)
{
	if (program->glptr != 0)
	{
		glDeleteProgram(program->glptr);
		program->glptr = 0;
	}
}


int VerticesInit(const struct Vertex* data, uint16_t length, struct Vertices* out, struct jaStatus* st)
{
	GLint reported_size = 0;
	GLint old_bind = 0;

	jaStatusSet(st, "VerticesInit", JA_STATUS_SUCCESS, NULL);

	glGenBuffers(1, &out->glptr);

	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_bind);
	glBindBuffer(GL_ARRAY_BUFFER, out->glptr); // Before ask if is!

	if (glIsBuffer(out->glptr) == GL_FALSE)
	{
		jaStatusSet(st, "VerticesInit", JA_STATUS_ERROR, "Creating GL buffer");
		goto return_failure;
	}

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(struct Vertex) * length), data, GL_STREAM_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &reported_size);

	if ((size_t)reported_size != (sizeof(struct Vertex) * length))
	{
		jaStatusSet(st, "VerticesInit", JA_STATUS_ERROR, "Attaching data");
		goto return_failure;
	}

	out->length = length;

	// Bye!
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)old_bind);
	return 0;

return_failure:
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)old_bind);

	if (out->glptr != 0)
		glDeleteBuffers(1, &out->glptr);

	return 1;
}


inline void VerticesFree(struct Vertices* vertices)
{
	if (vertices->glptr != 0)
	{
		glDeleteBuffers(1, &vertices->glptr);
		vertices->glptr = 0;
	}
}


int IndexInit(const uint16_t* data, size_t length, struct Index* out, struct jaStatus* st)
{
	GLint reported_size = 0;
	GLint old_bind = 0;

	jaStatusSet(st, "IndexInit", JA_STATUS_SUCCESS, NULL);

	glGenBuffers(1, &out->glptr);

	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_bind);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out->glptr); // Before ask if is!

	if (glIsBuffer(out->glptr) == GL_FALSE)
	{
		jaStatusSet(st, "IndexInit", JA_STATUS_ERROR, "Creating GL buffer");
		goto return_failure;
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(sizeof(uint16_t) * length), data, GL_STREAM_DRAW);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &reported_size);

	if ((size_t)reported_size != (sizeof(uint16_t) * length))
	{
		jaStatusSet(st, "IndexInit", JA_STATUS_ERROR, "Attaching data");
		goto return_failure;
	}

	out->length = length;

	// Bye!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)old_bind);
	return 0;

return_failure:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)old_bind);

	if (out->glptr != 0)
		glDeleteBuffers(1, &out->glptr);

	return 1;
}


inline void IndexFree(struct Index* index)
{
	if (index->glptr != 0)
	{
		glDeleteBuffers(1, &index->glptr);
		index->glptr = 0;
	}
}


int TextureInitImage(const struct Context* context, const struct jaImage* image, struct Texture* out,
                     struct jaStatus* st)
{
	GLint old_bind = 0;

	jaStatusSet(st, "TextureInitImage", JA_STATUS_SUCCESS, NULL);

	if (image->format != JA_IMAGE_U8)
	{
		jaStatusSet(st, "TextureInitImage", JA_STATUS_ERROR, "Only 8 bits per component images supported");
		return 1;
	}

	glGenTextures(1, &out->glptr);

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &old_bind);
	glBindTexture(GL_TEXTURE_2D, out->glptr); // Before ask if is!

	if (glIsTexture(out->glptr) == GL_FALSE)
	{
		jaStatusSet(st, "TextureInitImage", JA_STATUS_ERROR, "Creating GL texture");
		return 1;
	}

	switch (context->cfg_filter)
	{
	case FILTER_BILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case FILTER_TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case FILTER_PIXEL_BILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case FILTER_PIXEL_TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case FILTER_NONE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	}

	switch (image->channels)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)image->width, (GLsizei)image->height, 0, GL_LUMINANCE,
		             GL_UNSIGNED_BYTE, image->data);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (GLsizei)image->width, (GLsizei)image->height, 0,
		             GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, image->data);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)image->width, (GLsizei)image->height, 0, GL_RGB,
		             GL_UNSIGNED_BYTE, image->data);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)image->width, (GLsizei)image->height, 0, GL_RGBA,
		             GL_UNSIGNED_BYTE, image->data);
		break;
	default: break;
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, (GLuint)old_bind);
	return 0;
}


int TextureInitFilename(const struct Context* context, const char* image_filename, struct Texture* out,
                        struct jaStatus* st)
{
	struct jaImage* image = NULL;
	jaStatusSet(st, "TextureInitFilename", JA_STATUS_SUCCESS, NULL);

	if ((image = jaImageLoad(image_filename, st)) == NULL)
		return 1;

	if (TextureInitImage(context, image, out, st) != 0)
	{
		jaImageDelete(image);
		return 1;
	}

	jaImageDelete(image);
	return 0;
}


inline void TextureFree(struct Texture* texture)
{
	if (texture->glptr != 0)
	{
		glDeleteTextures(1, &texture->glptr);
		texture->glptr = 0;
	}
}
