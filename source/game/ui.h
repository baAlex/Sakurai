/*-----------------------------

 [ui.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UI_H
#define UI_H

#include "actor.h"
#include "engine.h"

void HudDraw(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor_a, struct Actor* actor_b);

void MenuActionDraw_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor, struct Actor* hud_a, struct Actor* hud_b);
uint8_t MenuActionDraw_dynamic(uint8_t arrow_sprite, uint8_t font_sprite, struct Actor* actor, uint8_t selection);

void MenuTargetDraw_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* hud_a, struct Actor* hud_b);
uint8_t MenuTargetDraw_dynamic(uint8_t arrow_sprite, uint8_t selection);

void MenuClean(); /* For MenuAction and MenuTarget */

#endif
