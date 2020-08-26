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
	size_t width;
	size_t height;
	size_t frames;
	size_t flags;
	uint8_t* data[];
};

int Jvn2Sgi(const char* filename);

struct JvnImage* JvnImageLoad(const char* filename, struct jaBuffer* buffer, struct jaStatus* st);
void JvnImageDelete(struct JvnImage* image);

#endif
