/*-----------------------------

 [utilities.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include "engine.h"

void Seed(uint16_t value);
uint16_t Random();
int8_t Sin(uint8_t x);
void Clear(void* dest, uint16_t size);
void Copy(const void* org, void* dest, uint16_t size);
char* NumberToString(uint8_t no, char* out);

uint8_t KeyToggle(uint8_t i, uint8_t* state);
uint8_t KeyRepeat(uint8_t i, uint8_t* state);

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define CLAMP(x, min_v, max_v) MIN(MAX((x), (min_v)), (max_v))

#endif
