/*-----------------------------

 [ui.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UI_H
#define UI_H

#include "engine.h"

void DrawHUD(uint8_t portraits_slot);

void DrawStaticUI(uint8_t portraits_slot);
void DrawDynamicUI(uint8_t selection, uint8_t item_slot);

void CleanStaticUI();

#endif
