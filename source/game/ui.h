/*-----------------------------

 [ui.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UI_H
#define UI_H

#include "actor.h"
#include "engine.h"

void HudDraw(uint8_t portraits_sprite, struct Actor* a, struct Actor* b);

void MenuActionDraw_static(uint8_t portraits_sprite, struct Actor* current, struct Actor* a, struct Actor* b);
uint8_t MenuActionDraw_dynamic(uint8_t arrow_sprite, struct Actor* current, uint8_t selection);

void MenuTargetDraw_static(uint8_t portraits_sprite, struct Actor* a, struct Actor* b);
uint8_t MenuTargetDraw_dynamic(uint8_t arrow_sprite, uint8_t selection);

void MenuClean(); /* For MenuAction and MenuTarget */

#endif
