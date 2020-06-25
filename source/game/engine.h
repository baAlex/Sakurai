/*-----------------------------

 [engine.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ENGINE_H
#define ENGINE_H


/* Improvised Std */
#if defined(__BCC__) && defined(__MSDOS__)

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define NULL 0x0000

#else
#include <stddef.h>
#include <stdint.h>
#endif


/* Engine internals ('Game PSP' from the engine perspective) */
#define FRAME_COUNTER_OFFSET 0x0002
#define MS_COUNTER_OFFSET 0x0004
#define MAX_COMMANDS_OFFSET 0x0006

#define INPUT_X_OFFSET 0x0010
#define INPUT_Y_OFFSET 0x0011
#define INPUT_A_OFFSET 0x0012
#define INPUT_B_OFFSET 0x0013
#define INPUT_UP_OFFSET 0x0014
#define INPUT_DOWN_OFFSET 0x0015
#define INPUT_LEFT_OFFSET 0x0016
#define INPUT_RIGHT_OFFSET 0x0017
#define INPUT_SELECT_OFFSET 0x0018
#define INPUT_START_OFFSET 0x0019

#define CURRENT_FRAME (*(uint16_t*)FRAME_COUNTER_OFFSET)
#define CURRENT_MILLISECONDS (*(uint16_t*)MS_COUNTER_OFFSET)
#define MAX_COMMANDS (*(uint16_t*)MAX_COMMANDS_OFFSET)
#define INPUT_X (*(uint8_t*)INPUT_X_OFFSET)
#define INPUT_Y (*(uint8_t*)INPUT_Y_OFFSET)
#define INPUT_A (*(uint8_t*)INPUT_A_OFFSET)
#define INPUT_B (*(uint8_t*)INPUT_B_OFFSET)
#define INPUT_UP (*(uint8_t*)INPUT_UP_OFFSET)
#define INPUT_DOWN (*(uint8_t*)INPUT_DOWN_OFFSET)
#define INPUT_LEFT (*(uint8_t*)INPUT_LEFT_OFFSET)
#define INPUT_RIGHT (*(uint8_t*)INPUT_RIGHT_OFFSET)
#define INPUT_SELECT (*(uint8_t*)INPUT_SELECT_OFFSET)
#define INPUT_START (*(uint8_t*)INPUT_START_OFFSET)


/* Interruptions */
void IntPrintText(char* text);
void IntPrintNumber(uint16_t number);
void IntLoadBackground(char* filename);
uint8_t IntLoadSprite(char* filename);
void IntFreeSprite(uint8_t sprite);
void IntUnloadAll();
void IntFlushCommands();


/* Commands */
void CmdDrawBackground();
void CmdDrawPixel(uint16_t x, uint16_t y, uint8_t color);
void CmdDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y);
void CmdDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawSprite(uint8_t sprite, uint16_t x, uint16_t y, uint8_t frame);
void CmdDrawText(uint8_t sprite, uint16_t x, uint16_t y, char* text);
void CmdDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdHalt();

#endif
