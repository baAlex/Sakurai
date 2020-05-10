/*-----------------------------

 [sakurai.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef SAKURAI_H
#define SAKURAI_H


/* Improvised Std */

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define NULL 0x0000


/* Engine internals */

#define FRAME_COUNTER_OFFSET 0x00C0

#define INPUT_X_OFFSET 0x00CA
#define INPUT_Y_OFFSET 0x00CB
#define INPUT_A_OFFSET 0x00CC
#define INPUT_B_OFFSET 0x00CD
#define INPUT_UP_OFFSET 0x00CE
#define INPUT_DOWN_OFFSET 0x00CF
#define INPUT_LEFT_OFFSET 0x00D0
#define INPUT_RIGHT_OFFSET 0x00D1
#define INPUT_SELECT_OFFSET 0x00D2
#define INPUT_START_OFFSET 0x00D3

#define CURRENT_FRAME (*(uint16_t*)FRAME_COUNTER_OFFSET)
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
void IntLoadSprite(char* filename, uint16_t slot);
void IntUnloadAll();


/* Commands */

void CmdDrawBackground();
void CmdDrawPixel(uint16_t x, uint16_t y, uint8_t color);
void CmdDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y);
void CmdDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawSprite(uint8_t slot, uint16_t x, uint16_t y, uint8_t frame);
void CmdDrawText(uint8_t slot, uint16_t x, uint16_t y, char* text);
void CmdHalt();

#endif
