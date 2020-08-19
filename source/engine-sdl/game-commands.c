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

 [game-commands.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "game-commands.h"
#include <stdint.h>
#include <string.h>


static inline void sGameDrawPixel(uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
		*p = color;
}


static inline void sGameDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	for (size_t i = 0; i < (size_t)(width << 4); i++)
	{
		if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
			*p = color;

		p += 1;
	}
}


static inline void sGameDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	for (size_t i = 0; i < (size_t)(height << 4); i++)
	{
		if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
			*p = color;

		p += out->width;
	}
}


static inline void sGameDrawBkg(const struct jaImage* bkg, struct jaImage* out)
{
	memcpy(out->data, bkg->data, bkg->size);
}


static inline void sGameDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y,
                                         const struct jaImage* bkg, struct jaImage* out)
{
	x -= 1; // TODO, error in the assembly engine (maybe is in DrawSprite)

	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];
	uint8_t* src = &((uint8_t*)bkg->data)[x + out->width * y];

	for (size_t row = 0; row < (size_t)(height << 4); row++)
	{
		for (size_t col = 0; col < (size_t)(width << 4); col++)
		{
			if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
				*p = *src;

			p += 1;
			src += 1;
		}

		p += out->width - (width << 4);
		src += out->width - (width << 4);
	}
}


static inline void sGameDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color,
                                      struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	for (size_t row = 0; row < (size_t)(height << 4); row++)
	{
		for (size_t col = 0; col < (size_t)(width << 4); col++)
		{
			if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
				*p = color;

			p += 1;
		}

		p += out->width - (width << 4);
	}
}


static inline void sGameDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color,
                                             struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	for (size_t row = 0; row < (size_t)height; row++)
	{
		for (size_t col = 0; col < (size_t)width; col++)
		{
			if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
				*p = color;

			p += 1;
		}

		p += out->width - width;
	}
}


static inline void sGameDrawSprite(uint8_t sprite, uint16_t x, uint16_t y, uint8_t frame, struct jaImage* out)
{
	(void)sprite;
	(void)frame;

	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];
	*p = 0;
}


static inline void sGameDrawText(uint8_t sprite, uint16_t x, uint16_t y, const char* text, struct jaImage* out)
{
	(void)sprite;
	(void)text;

	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];
	*p = 0;
}


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
			sGameDrawBkg(bkg, out);
			break;
		case 0x02: // CODE_DRAW_RECTANGLE_BKG
			sGameDrawRectangleBkg(cmd->width, cmd->height, cmd->x, cmd->y, bkg, out);
			break;
		case 0x03: // CODE_DRAW_SPRITE
			sGameDrawSprite(cmd->sprite, cmd->x, cmd->y, cmd->frame, out);
			break;
		case 0x04: // CODE_DRAW_RECTANGLE
			sGameDrawRectangle(cmd->width, cmd->height, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x05: // CODE_DRAW_RECTANGLE_PRECISE
			sGameDrawRectanglePrecise(cmd->width, cmd->height, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x06: // CODE_DRAW_TEXT
			sGameDrawText(cmd->sprite, cmd->x, cmd->y, "Nope", out);
			break;
		case 0x07: // CODE_DRAW_H_LINE
			sGameDrawHLine(cmd->width, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x08: // CODE_DRAW_V_LINE
			sGameDrawVLine(cmd->width, cmd->x, cmd->y, cmd->color, out);
			break;
		case 0x09: // CODE_DRAW_PIXEL
			sGameDrawPixel(cmd->x, cmd->y, cmd->color, out);
			break;
		default: break;
		}
	}
}
