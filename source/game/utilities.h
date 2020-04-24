/*-----------------------------

 [utilities.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include "engine.h"

#define CURRENT_FRAME (*(uint16_t*)FRAME_COUNTER_OFFSET)

uint16_t Random();
int8_t Sin(uint8_t a);

union Command* NewCommand(uint8_t code);
void CleanCommands();

void PrintString(uint16_t string);
void PrintNumber(uint16_t number);
void LoadBackground(uint16_t filename);
void LoadSprite(uint16_t filename, uint16_t slot);

#endif
