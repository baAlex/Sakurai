/*-----------------------------

 [game-glue.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef GAME_GLUE_H
#define GAME_GLUE_H

#include "japan-image.h"
#include "kansai-context.h"

enum GameInterruptionType
{
	GAME_PRINT_STRING = 0x01,      // GamePrintString
	GAME_PRINT_NUMBER = 0x02,      // GamePrintNumber
	GAME_LOAD_BACKGROUND = 0x03,   // GameLoadBackground
	GAME_LOAD_PALETTE = 0x04,      // GameLoadPalette
	GAME_UNLOAD_EVERYTHING = 0x05, // GameUnloadEverything
	GAME_LOAD_SPRITE = 0x07,       // GameLoadSprite
	GAME_FREE_SPRITE = 0x08,       // GameFreeSprite
	GAME_EXIT_REQUEST = 0x09       // GameExitRequest
};

struct GameInterruption
{
	enum GameInterruptionType type;

	union
	{
		const char* string;   // GamePrintString
		const char* filename; // GameLoadBackground, GameLoadPalette, GameLoadSprite
		unsigned number;      // GamePrintNumber
	};
};

int GlueStart(void (*interruption_callback)(struct GameInterruption, uintptr_t*, void*), void* callback_data);
void GlueStop();

void GlueFrame(struct kaEvents e, size_t ms, const struct jaImage* buffer_background, struct jaImage* buffer_out);

#endif
