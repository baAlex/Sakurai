/*-----------------------------

 [state.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef STATE_H
#define STATE_H

#define SPRITE_FONT1 1
#define SPRITE_FONT2 2
#define SPRITE_PORTRAITS 3
#define SPRITE_SAO 4
#define SPRITE_KURO 5
#define SPRITE_ARROW 6

void* StateBattle();
void* StateIntro();

void* StateTest1(); /* Text rendering */
void* StateTest2(); /* Background and sprites */
void* StateTest3(); /* HUD and menu*/

#endif
