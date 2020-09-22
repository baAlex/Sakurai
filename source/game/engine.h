/*-----------------------------

 [engine.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ENGINE_H
#define ENGINE_H


#if defined(SAKURAI_DOS)
	#include "engine-dos.h"
#else


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
#if defined(__BCC__) && defined(__MSDOS__)
	#define FRAME_COUNTER_OFFSET 0x0002
	#define MS_COUNTER_OFFSET 0x0004
	#define MAX_COMMANDS_OFFSET 0x0006
	#define INPUT_X_OFFSET 0x0010
	#define INPUT_Y_OFFSET 0x0011
	#define INPUT_PAD_U_OFFSET 0x0014
	#define INPUT_PAD_D_OFFSET 0x0015
	#define INPUT_PAD_L_OFFSET 0x0016
	#define INPUT_PAD_R_OFFSET 0x0017
	#define INPUT_SELECT_OFFSET 0x0018
	#define INPUT_START_OFFSET 0x0019
#else
	extern uintptr_t g_psp_offset;
	#define FRAME_COUNTER_OFFSET (g_psp_offset + 0x0002)
	#define MS_COUNTER_OFFSET (g_psp_offset + 0x0004)
	#define MAX_COMMANDS_OFFSET (g_psp_offset + 0x0006)
	#define INPUT_X_OFFSET (g_psp_offset + 0x0010)
	#define INPUT_Y_OFFSET (g_psp_offset + 0x0011)
	#define INPUT_PAD_U_OFFSET (g_psp_offset + 0x0014)
	#define INPUT_PAD_D_OFFSET (g_psp_offset + 0x0015)
	#define INPUT_PAD_L_OFFSET (g_psp_offset + 0x0016)
	#define INPUT_PAD_R_OFFSET (g_psp_offset + 0x0017)
	#define INPUT_SELECT_OFFSET (g_psp_offset + 0x0018)
	#define INPUT_START_OFFSET (g_psp_offset + 0x0019)
#endif

#define CURRENT_FRAME (*(uint16_t*)FRAME_COUNTER_OFFSET)
#define CURRENT_MILLISECONDS (*(uint16_t*)MS_COUNTER_OFFSET)
#define MAX_COMMANDS (*(uint16_t*)MAX_COMMANDS_OFFSET)
#define INPUT_X (*(uint8_t*)INPUT_X_OFFSET)
#define INPUT_Y (*(uint8_t*)INPUT_Y_OFFSET)
#define INPUT_PAD_U (*(uint8_t*)INPUT_PAD_U_OFFSET)
#define INPUT_PAD_D (*(uint8_t*)INPUT_PAD_D_OFFSET)
#define INPUT_PAD_L (*(uint8_t*)INPUT_PAD_L_OFFSET)
#define INPUT_PAD_R (*(uint8_t*)INPUT_PAD_R_OFFSET)
#define INPUT_SELECT (*(uint8_t*)INPUT_SELECT_OFFSET)
#define INPUT_START (*(uint8_t*)INPUT_START_OFFSET)


/* Interruptions */
void IntPrintText(char* text);
void IntPrintNumber(uint16_t number);
void IntLoadBackground(char* filename);
void IntLoadPalette(char* filename);
uint8_t IntLoadSprite(char* filename);
void IntUnloadAll();
void IntFlushCommands();
void IntExitRequest();


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
#endif
