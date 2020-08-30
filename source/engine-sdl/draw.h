/*-----------------------------

 [draw.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>

#include "japan-image.h"
#include "jvn.h"

void DrawPixel(uint16_t x, uint16_t y, uint8_t color, struct jaImage* out);
void DrawHLine(uint8_t w, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out);
void DrawVLine(uint8_t h, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out);
void DrawBkg(const struct jaImage* bkg, struct jaImage* out);
void DrawRectangleBkg(uint8_t w, uint8_t h, uint16_t x, uint16_t y, const struct jaImage* bkg, struct jaImage* out);
void DrawRectangle(uint8_t w, uint8_t h, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out);
void DrawRectanglePrecise(uint8_t w, uint8_t h, uint16_t x, uint16_t y, uint8_t color, struct jaImage* out);
void DrawSprite(struct JvnImage* sprite, uint16_t x, uint16_t y, uint8_t frame, struct jaImage* out);
void DrawText(struct JvnImage* sprite, uint16_t x, uint16_t y, const char* text, struct jaImage* out);
void DrawColorizeRGBX(const uint8_t* palette, const struct jaImage* indexed, struct jaImage* rgbx_out);

#endif
