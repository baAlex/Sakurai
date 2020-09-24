/*-----------------------------

 [ui.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UI_H
#define UI_H

#include "actor.h"
#include "engine.h"

void UiDialog(uint8_t font_sprite, uint16_t start_ms, const char* character, const char* lines[]);

void UiBanner(uint8_t font_sprite, const char* text);
void UiHUD(uint8_t portraits_sprite, uint8_t font_sprite, const struct Actor* actor_a, const struct Actor* actor_b);

void UiPanelAction_static(uint8_t portraits_sprite, uint8_t font_sprite, const struct Persona* persona,
                          const struct Actor* hud_a, const struct Actor* hud_b);
uint8_t UiPanelAction_dynamic(uint8_t arrow_sprite, uint8_t font_sprite, const struct Persona* persona,
                              uint8_t selection);

void UiPanelTarget_static(uint8_t portraits_sprite, uint8_t font_sprite, const struct Actor* hud_a,
                          const struct Actor* hud_b);
uint8_t UiPanelTarget_dynamic(uint8_t arrow_sprite, uint8_t selection);

void UiPanelClean(); /* For PanelAction and PanelTarget */

void UiMenuPause_static(uint8_t font_sprite1, uint8_t font_sprite2);
uint8_t UiMenuPause_dynamic(uint8_t arrow_sprite, uint8_t selection);

#define PAUSE_W 6 /* 96 px */
#define PAUSE_H 4 /* 64 px */
#define PAUSE_X 112
#define PAUSE_Y 68

#endif
