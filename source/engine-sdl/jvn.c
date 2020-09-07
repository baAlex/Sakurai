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

 [jvn.c]
 - Alexander Brandt 2020
-----------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "japan-endianness.h"
#include "japan-image.h"
#include "jvn.h"


struct JvnHeader
{
	uint16_t file_size;
	uint16_t width;
	uint16_t height;
	uint16_t unused;
	uint16_t data_offset;
	uint16_t frames;
};

enum Instruction
{
	I86_UNKNOWN,
	I86_ADD_DI,
	I86_ADD_SI,
	I86_SUB_DI,
	I86_SUB_SI,
	I86_MOVSB,
	I86_MOVSW,
	I86_MOVSD,
	I86_MOV_AL,
	I86_RETF
};


static enum Instruction sDecode(uint16_t* out_imm, enum jaEndianness sys_endian, FILE* fp, struct jaStatus* st)
{
	// http://ref.x86asm.net/coder32.html
	// http://ref.x86asm.net/coder32.html#modrm_byte_16
	// http://ref.x86asm.net/#column_o

	uint8_t op = 0;
	uint8_t prefix = 0;

again:

	if (fread(&op, 1, 1, fp) != 1)
		goto return_eof;

	// Found a size override-prefix (there is no OP with code 0x66)
	if (op == 0x66)
	{
		prefix = op;
		goto again;
	}

	// ADD/SUB [OP, REG, IMM8] (r/m16/32/64 = imm8)
	if (op == 0x83)
	{
		uint8_t reg = 0;
		uint8_t imm8 = 0;

		if (fread(&reg, 1, 1, fp) != 1 || fread(&imm8, 1, 1, fp) != 1)
			goto return_eof;

		*out_imm = imm8;

		switch (reg)
		{
		case 0xC6: return I86_ADD_SI;
		case 0xC7: return I86_ADD_DI;
		case 0xEE: return I86_SUB_SI;
		case 0xEF: return I86_SUB_DI;
		default: return I86_UNKNOWN;
		}
	}

	// ADD/SUB [OP, REG, IMM16] (r/m16/32/64 = imm16/32)
	else if (op == 0x81)
	{
		uint8_t reg = 0;
		uint16_t imm16 = 0;

		if (fread(&reg, 1, 1, fp) != 1 || fread(&imm16, sizeof(uint16_t), 1, fp) != 1)
			goto return_eof;

		*out_imm = jaEndianToU16(imm16, JA_ENDIAN_LITTLE, sys_endian);

		switch (reg)
		{
		case 0xC6: return I86_ADD_SI;
		case 0xC7: return I86_ADD_DI;
		case 0xEE: return I86_SUB_SI;
		case 0xEF: return I86_SUB_DI;
		default: return I86_UNKNOWN;
		}
	}

	// MOVSB [OP]
	else if (op == 0xA4)
		return I86_MOVSB;

	// MOVSW/MOVSD [PREFIX, OP]
	else if (op == 0xA5)
		return (prefix == 0) ? I86_MOVSW : I86_MOVSD;

	// MOV [OP, IMM8] (r8 = imm8)
	else if (op == 0xB0)
	{
		uint16_t imm8 = 0;

		if (fread(&imm8, 1, 1, fp) != 1)
			goto return_eof;

		*out_imm = imm8;
		return I86_MOV_AL;
	}

	// RETF [OP]
	else if (op == 0xCB)
		return I86_RETF;

	// At this point any return is an error
	jaStatusSet(st, "JvnImageLoad", JA_STATUS_UNEXPECTED_DATA, "unknown instruction, offset 0x%04lX", ftell(fp));
	return I86_UNKNOWN;
return_eof:
	jaStatusSet(st, "JvnImageLoad", JA_STATUS_UNEXPECTED_EOF, "near code offset 0x%04lX", ftell(fp));
	return I86_UNKNOWN;
}


static int sDraw(uint8_t* out, uint8_t* out_spacing, const uint8_t* program_data, FILE* fp, struct jaStatus* st)
{
	enum Instruction ins;
	uint16_t imm = 0;
	uint16_t si = 0;
	uint16_t di = 0;

	while (1)
	{
		if ((ins = sDecode(&imm, jaEndianSystem(), fp, st)) == I86_UNKNOWN)
			return 1;

		if (ins == I86_ADD_DI)
			di += imm;
		else if (ins == I86_ADD_SI)
			si += imm;
		else if (ins == I86_SUB_DI)
			di -= imm;
		else if (ins == I86_SUB_SI)
			si -= imm;
		else if (ins == I86_MOVSB)
		{
			out[di % (320 * 200)] = program_data[si];
			di += 1;
			si += 1;
		}
		else if (ins == I86_MOVSW)
		{
			out[di % (320 * 200)] = program_data[si];
			out[(di + 1) % (320 * 200)] = program_data[si + 1];
			di += 2;
			si += 2;
		}
		else if (ins == I86_MOVSD)
		{
			out[di % (320 * 200)] = program_data[si];
			out[(di + 1) % (320 * 200)] = program_data[si + 1];
			out[(di + 2) % (320 * 200)] = program_data[si + 2];
			out[(di + 3) % (320 * 200)] = program_data[si + 3];
			di += 4;
			si += 4;
		}
		else if (ins == I86_MOV_AL)
		{
			*out_spacing = (uint8_t)imm;
		}
		else if (ins == I86_RETF)
			break;
	}

	return 0;
}


struct JvnImage* JvnImageLoad(const char* filename, struct jaBuffer* buffer, struct jaStatus* st)
{
	FILE* fp = NULL;
	struct JvnImage* image = NULL;
	struct JvnHeader header;

	uint8_t* program_data = NULL;

	jaStatusSet(st, "JvnImageLoad", JA_STATUS_SUCCESS, NULL);

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "JvnImageLoad", JA_STATUS_FS_ERROR, "'%s'", filename);
		goto return_failure;
	}

	// Read header
	if (fread(&header, sizeof(struct JvnHeader), 1, fp) != 1)
	{
		jaStatusSet(st, "JvnImageLoad", JA_STATUS_UNEXPECTED_EOF, "near header ('%s')", filename);
		goto return_failure;
	}

	header.width = jaEndianToU16(header.width, JA_ENDIAN_LITTLE, JA_ENDIAN_SYSTEM);
	header.height = jaEndianToU16(header.height, JA_ENDIAN_LITTLE, JA_ENDIAN_SYSTEM);
	header.frames = jaEndianToU16(header.frames, JA_ENDIAN_LITTLE, JA_ENDIAN_SYSTEM);
	header.file_size = jaEndianToU16(header.file_size, JA_ENDIAN_LITTLE, JA_ENDIAN_SYSTEM);
	header.data_offset = jaEndianToU16(header.data_offset, JA_ENDIAN_LITTLE, JA_ENDIAN_SYSTEM);

	header.data_offset += 8; // To make it absolute from the Load-Header

	// Allocations
	{
		// Image
		if ((image = calloc(1, sizeof(struct JvnImage) + (sizeof(uint8_t*) * header.frames) +
		                           (sizeof(uint8_t) * header.frames) +
		                           (header.width * header.height * header.frames))) == NULL)
			goto return_failure;

		image->width = (uint8_t)header.width;
		image->height = (uint8_t)header.height;
		image->frames = (uint8_t)header.frames;

		for (uint8_t f = 0; f < header.frames; f++)
		{
			image->data[f] = (uint8_t*)(image) + sizeof(struct JvnImage) + sizeof(uint8_t*) * header.frames;
			image->data[f] += image->width * image->height * f;
		}

		image->spacing = image->data[header.frames - 1] + image->width * image->height;

		// Temporary screen and buffer for program data (colors)
		if (jaBufferResizeZero(buffer, (320 * 200) + (header.file_size - header.data_offset)) == NULL)
			goto return_failure;

		program_data = (uint8_t*)(buffer->data) + (320 * 200);
	}

	// Read program data (colors at the end)
	fseek(fp, header.data_offset, SEEK_SET);

	for (uint16_t i = 0; i < (header.file_size - header.data_offset); i++)
	{
		if (fread(program_data + i, 1, 1, fp) != 1)
		{
			jaStatusSet(st, "JvnImageLoad", JA_STATUS_UNEXPECTED_EOF, "near data ('%s')", filename);
			goto return_failure;
		}
	}

	// Read program code
	for (uint16_t f = 0; f < header.frames; f++)
	{
		// Set frame offset
		{
			uint16_t frame_offset = 0;
			fseek(fp, 12 + (f << 1), SEEK_SET); // 12 = Where the frames offset table starts

			if (fread(&frame_offset, sizeof(uint16_t), 1, fp) != 1)
			{
				jaStatusSet(st, "JvnImageLoad", JA_STATUS_UNEXPECTED_EOF, "near frames offset table ('%s')", filename);
				goto return_failure;
			}

			frame_offset = jaEndianToU16(frame_offset, JA_ENDIAN_LITTLE, JA_ENDIAN_SYSTEM);
			fseek(fp, frame_offset - 2, SEEK_CUR);
		}

		// Draw (yay!)
		memset(buffer->data, 0, (320 * 200));

		if (sDraw(buffer->data, &image->spacing[f], program_data, fp, st) != 0)
			goto return_failure;

		for (uint8_t r = 0; r < image->height; r++)
			memcpy(image->data[f] + (image->width * r), (uint8_t*)buffer->data + (320 * r), image->width);
	}

	// Bye!
	fclose(fp);
	return image;

return_failure:
	if (fp != NULL)
		fclose(fp);
	if (image != NULL)
		free(image);
	return NULL;
}


void JvnImageDelete(struct JvnImage* image)
{
	free(image);
}


int Jvn2Sgi(const char* filename)
{
	struct jaBuffer b = {0};
	struct jaStatus st = {0};

	struct JvnImage* img = NULL;
	struct jaImage ja_img = {0};
	char name[256];

	// Load sprite
	if ((img = JvnImageLoad(filename, &b, &st)) == NULL)
	{
		jaStatusPrint(NULL, st);
		return EXIT_FAILURE;
	}

	// Save all frames
	ja_img.width = img->width;
	ja_img.height = img->height;
	ja_img.channels = 1;
	ja_img.format = JA_IMAGE_U8;
	ja_img.size = img->width * img->height;

	for (uint8_t f = 0; f < img->frames; f++)
	{
		ja_img.data = img->data[f];
		sprintf(name, "./%s.%03u.sgi", filename, (unsigned)f);
		jaImageSaveSgi(&ja_img, name, NULL);
	}

	// Bye!
	JvnImageDelete(img);
	jaBufferClean(&b);
	return EXIT_SUCCESS;
}
