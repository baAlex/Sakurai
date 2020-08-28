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

 [draw.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "draw.h"
#include <string.h>


inline void DrawPixel(uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
		*p = color;
}


inline void DrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	for (size_t i = 0; i < (size_t)(width << 4); i++)
	{
		if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
			*p = color;

		p += 1;
	}
}


inline void DrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
{
	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];

	for (size_t i = 0; i < (size_t)(height << 4); i++)
	{
		if (p < ((uint8_t*)out->data) + out->size) // Draw inside image bounds
			*p = color;

		p += out->width;
	}
}


inline void DrawBkg(const struct jaImage* bkg, struct jaImage* out)
{
	memcpy(out->data, bkg->data, bkg->size);
}


inline void DrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y, const struct jaImage* bkg,
                             struct jaImage* out)
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


inline void DrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out)
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


inline void DrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color,
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


inline void DrawSprite(uint8_t sprite, uint16_t x, uint16_t y, uint8_t frame, struct jaImage* out)
{
	(void)sprite;
	(void)frame;

	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];
	*p = 0;
}


inline void DrawText(uint8_t sprite, uint16_t x, uint16_t y, const char* text, struct jaImage* out)
{
	(void)sprite;
	(void)text;

	uint8_t* p = &((uint8_t*)out->data)[x + out->width * y];
	*p = 0;
}


void DrawColorizeRGBX(const uint8_t* palette, const struct jaImage* indexed, struct jaImage* rgbx_out)
{
	uint8_t* in = indexed->data;
	uint8_t* end_in = in + indexed->size;
	uint8_t* out = rgbx_out->data;
	uint8_t* end_out = out + rgbx_out->size;

	for (; in < end_in && out < end_out; in++)
	{
		*(out) = palette[*in * 3];
		*(out + 1) = palette[*in * 3 + 1];
		*(out + 2) = palette[*in * 3 + 2];
		*(out + 3) = 255;
		out += 4;
	}
}
