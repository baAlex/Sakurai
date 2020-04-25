/*-----------------------------

 [utilities.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include "engine.h"

#define CURRENT_FRAME (*(uint16_t*)FRAME_COUNTER_OFFSET)
#define INPUT_X (*(uint8_t*)INPUT_X_OFFSET)
#define INPUT_Y (*(uint8_t*)INPUT_Y_OFFSET)
#define INPUT_A (*(uint8_t*)INPUT_A_OFFSET)
#define INPUT_B (*(uint8_t*)INPUT_B_OFFSET)
#define INPUT_UP (*(uint8_t*)INPUT_UP_OFFSET)
#define INPUT_DOWN (*(uint8_t*)INPUT_DOWN_OFFSET)
#define INPUT_LEFT (*(uint8_t*)INPUT_LEFT_OFFSET)
#define INPUT_RIGHT (*(uint8_t*)INPUT_RIGHT_OFFSET)
#define INPUT_SELECT (*(uint8_t*)INPUT_SELECT_OFFSET)
#define INPUT_START (*(uint8_t*)INPUT_START_OFFSET)

uint16_t Random();
int8_t Sin(uint8_t a);

union Command* NewCommand(uint8_t code);
void CleanCommands();

void PrintString(uint16_t string);
void PrintNumber(uint16_t number);
void LoadBackground(uint16_t filename);
void LoadSprite(uint16_t filename, uint16_t slot);
void UnloadEverything();

#endif
