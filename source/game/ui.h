/*-----------------------------

 [ui.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UI_H
#define UI_H

#include "actor.h"
#include "engine.h"

void UiDialog(uint8_t font_sprite, uint16_t start_ms, char* character, char** lines);

void UiBanner(uint8_t font_sprite, char* text);
void UiHUD(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* actor_a, struct Actor* actor_b);

void UiPanelAction_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Persona* persona, struct Actor* hud_a, struct Actor* hud_b);
uint8_t UiPanelAction_dynamic(uint8_t arrow_sprite, uint8_t font_sprite, struct Persona* persona, uint8_t selection);

void UiPanelTarget_static(uint8_t portraits_sprite, uint8_t font_sprite, struct Actor* hud_a, struct Actor* hud_b);
uint8_t UiPanelTarget_dynamic(uint8_t arrow_sprite, uint8_t selection);

void UiPanelClean(); /* For PanelAction and PanelTarget */

#endif
