/*-----------------------------

 [engine-dos.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ENGINE_DOS_H
#define ENGINE_DOS_H

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define NULL 0x0000

#define CURRENT_FRAME (*(uint16_t*)0x0002)
#define CURRENT_MILLISECONDS (*(uint16_t*)0x0004)
#define MAX_COMMANDS (*(uint16_t*)0x0006)
#define INPUT_X_OFFSET (*(uint8_t*)0x0010)
#define INPUT_Y_OFFSET (*(uint8_t*)0x0011)
#define INPUT_PAD_U_OFFSET (*(uint8_t*)0x0014)
#define INPUT_PAD_D_OFFSET (*(uint8_t*)0x0015)
#define INPUT_PAD_L_OFFSET (*(uint8_t*)0x0016)
#define INPUT_PAD_R_OFFSET (*(uint8_t*)0x0017)
#define INPUT_SELECT_OFFSET (*(uint8_t*)0x0018)
#define INPUT_START_OFFSET (*(uint8_t*)0x0019)

void IntPrintText(const char* text);
void IntPrintNumber(uint16_t number);
void IntLoadBackground(char* filename);
void IntLoadPalette(char* filename);
uint8_t IntLoadSprite(char* filename);
void IntUnloadAll();
void IntFlushCommands();
void IntExitRequest();

void CmdHalt();
void CmdDrawBackground();
void CmdDrawPixel(uint16_t x, uint16_t y, uint8_t color);
void CmdDrawRectangle(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawRectangleBkg(uint8_t width, uint8_t height, uint16_t x, uint16_t y);
void CmdDrawRectanglePrecise(uint8_t width, uint8_t height, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawSprite(uint8_t sprite, uint16_t x, uint16_t y, uint8_t frame);
void CmdDrawText(uint8_t sprite, uint16_t x, uint16_t y, char* text);
void CmdDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color);

#endif
