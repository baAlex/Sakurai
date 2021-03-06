/*-----------------------------

 [state.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef STATE_H
#define STATE_H

#include "engine.h"

void* StatePrepareIntro();
void* StatePrepareBattle(uint8_t battle_no);
void* StatePreparePause(uint8_t spr_font1, uint8_t spr_font2, uint8_t spr_items, void* resume_to);

/* For development purposes: */
void* StateScreenshots(); /* Staged screenshoots :) */
void* StateTest1();       /* Text rendering */
void* StateTest2();       /* Background and sprites */
void* StateTest3();       /* HUD and menu*/
void* StateTest4();       /* Exit menu*/

#endif
