/*-----------------------------

 [jvn.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef JVN_H
#define JVN_H

#include "japan-buffer.h"
#include "japan-status.h"
#include <stdint.h>

struct JvnImage
{
	uint8_t width;
	uint8_t height;
	uint8_t frames;
	uint8_t flags;
	uint8_t* spacing;
	uint8_t* data[];
};

int Jvn2Sgi(const char* filename);

struct JvnImage* JvnImageLoad(const char* filename, struct jaBuffer* buffer, struct jaStatus* st);
void JvnImageDelete(struct JvnImage* image);

#endif
