/*-----------------------------

 [fixed.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef FIXED_H
#define FIXED_H

#include "engine.h"

typedef uint16_t ufixed_t;

#define FIXED_DECIMAL_075 0xC0
#define FIXED_DECIMAL_05 0x80
#define FIXED_DECIMAL_025 0x40

#define UFixedMake(whole, fraction) (ufixed_t)(((uint16_t)(fraction)) | ((uint16_t)(whole) << 8))
#define FixedWhole(f) (uint8_t)((uint16_t)(f) >> 8)
#define FixedFraction(f) (uint8_t)((uint16_t)(f)&0x00FF)

ufixed_t UFixedMultiply(ufixed_t a, ufixed_t b);
ufixed_t UFixedDivide(ufixed_t a, ufixed_t b);
ufixed_t UFixedStep(ufixed_t edge0, ufixed_t edge1, ufixed_t v);

#ifdef FIXED_STANDALONE
#if !defined(__BCC__) && !defined(__MSDOS__)
#define FixedFromFloat(no) (int16_t)((float)no * 256.0 + 0.5)
#define FixedToFloat(no) ((float)no) / 256.0
#endif
#endif

#endif
