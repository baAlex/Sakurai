/*-----------------------------

 [ui.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UI_H
#define UI_H

#include "engine.h"

void DrawHUD(uint8_t portraits_slot);

void DrawActionUI_static(uint8_t portraits_slot, uint8_t actor_index);
uint8_t DrawActionUI_dynamic(uint8_t selection, uint8_t item_slot, uint8_t actor_index);

void DrawTargetUI_static(uint8_t portraits_slot);
uint8_t DrawTargetUI_dynamic(uint8_t selection, uint8_t item_slot);

void CleanUI();

#endif
