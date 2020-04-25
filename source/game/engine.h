/*-----------------------------

 [engine.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ENGINE_H
#define ENGINE_H

typedef signed char int8_t;
typedef signed short int16_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define UINT8_MAX 255
#define UINT16_MAX 65535

#define NULL 0xFFFF

#define COMMANDS_TABLE_LEN 24
#define COMMANDS_TABLE_OFFSET 0x0000
#define FRAME_COUNTER_OFFSET 0x00C0
#define INT_FD_ARG1 0x00C2
#define INT_FD_ARG2 0x00C4
#define INT_FD_ARG3 0x00C6
#define INT_FD_ARG4 0x00C8

#define CODE_HALT 0x00           /* Stops draw routine */
#define CODE_DRAW_BKG 0x01       /* Draw loaded background into buffer */
#define CODE_DRAW_PIXEL 0x02     /* Draw pixel into buffer */
#define CODE_DRAW_RECTANGLE 0x04 /* ... you got the idea */
#define CODE_DRAW_RECTANGLE_BKG 0x05
#define CODE_DRAW_RECTANGLE_PRECISE 0x06
#define CODE_DRAW_SPRITE 0x07
#define CODE_DRAW_TEXT 0x08

struct CommandDrawShape
{
	uint8_t code;
	uint8_t color;
	uint8_t width;  /* Multiplied by 16 */
	uint8_t height; /* Multiplied by 16 */
	uint16_t x;
	uint16_t y;
};

struct CommandGameDrawSprite
{
	uint8_t code;
	uint8_t slot;
	uint8_t frame;
	uint8_t unused;
	uint16_t x;
	uint16_t y;
};

struct CommandGameDrawText
{
	uint8_t code;
	uint8_t slot;
	uint16_t text;
	uint16_t x;
	uint16_t y;
};

union Command {
	uint8_t code;

	/* DRAW_PIXEL, DRAW_RECTANGLE, DRAW_RECTANGLE_BKG
	DRAW_RECTANGLE_PRECISE */
	struct CommandDrawShape draw_shape;

	/* DRAW_SPRITE */
	struct CommandGameDrawSprite draw_sprite;

	/* CODE_DRAW_TEXT */
	struct CommandGameDrawText draw_text;
};

#endif
