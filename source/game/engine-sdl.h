/*-----------------------------

 [engine-sdl.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ENGINE_SDL_H
#define ENGINE_SDL_H

#include <stddef.h>
#include <stdint.h>

extern uintptr_t g_psp_offset;
extern uintptr_t g_ifd_args_offset;
extern uintptr_t g_text_stack_offset;

#define CURRENT_FRAME (*(uint16_t*)(g_psp_offset + 0x0002))
#define CURRENT_MILLISECONDS (*(uint16_t*)(g_psp_offset + 0x0004))
#define MAX_COMMANDS (*(uint16_t*)(g_psp_offset + 0x0006))
#define INPUT_X (*(uint8_t*)(g_psp_offset + 0x0010))
#define INPUT_Y (*(uint8_t*)(g_psp_offset + 0x0011))
#define INPUT_PAD_U (*(uint8_t*)(g_psp_offset + 0x0014))
#define INPUT_PAD_D (*(uint8_t*)(g_psp_offset + 0x0015))
#define INPUT_PAD_L (*(uint8_t*)(g_psp_offset + 0x0016))
#define INPUT_PAD_R (*(uint8_t*)(g_psp_offset + 0x0017))
#define INPUT_SELECT (*(uint8_t*)(g_psp_offset + 0x0018))
#define INPUT_START (*(uint8_t*)(g_psp_offset + 0x0019))

void IntPrintText(const char* text);
void IntPrintNumber(uint16_t number);
void IntLoadBackground(const char* filename);
void IntLoadPalette(const char* filename);
uint8_t IntLoadSprite(const char* filename);
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
void CmdDrawText(uint8_t sprite, uint16_t x, uint16_t y, const char* text);
void CmdDrawHLine(uint8_t width, uint16_t x, uint16_t y, uint8_t color);
void CmdDrawVLine(uint8_t height, uint16_t x, uint16_t y, uint8_t color);

#endif
