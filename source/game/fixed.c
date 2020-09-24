/*-----------------------------

MIT License

Copyright (c) 2020 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [fixed.c]
 - Alexander Brandt 2020

Ivancescu Gabriel (2007). Fixed Point Arithmetic and Tricks
http://x86asm.net/articles/fixed-point-arithmetic-and-tricks/

Abrash Michael (October 1992). Pooh and the Space Station. In Dr. Dobb's Journal.
Psss: the file "FIXED.ASM" in the zip "xsharp21.zip".
https://www.drdobbs.com/database/graphics-programming/184408845?pgno=6


 $ clang -DFIXED_STANDALONE -std=c90 -Wall -Wextra -Wconversion -pedantic ./source/game/fixed.c -o fixed
 $ ia16-elf-gcc -DSAKURAI_DOS -DFIXED_STANDALONE -masm=intel ./source/game/fixed.c -o fixed.com
-----------------------------*/

#include "fixed.h"
#include "utilities.h"


ufixed_t UFixedMultiply(ufixed_t a, ufixed_t b)
{
#if defined(SAKURAI_DOS)

	ufixed_t ret;
	asm volatile("xor dx, dx\n\
	              mul cx\n\
	              mov al, ah\n\
	              mov ah, dl\n"
	             : /* Output */ "=ax"(ret)
	             : /* Input */[ a ] "ax"(a), [ b ] "cx"(b)
	             : /* Clobbers */ "dx");

	return ret;

#else
	return (a * b) >> 8;
#endif
}


ufixed_t UFixedDivide(ufixed_t a, ufixed_t b)
{
#if defined(SAKURAI_DOS)

	ufixed_t ret;
	asm volatile("xor dh, dh\n\
	              mov dl, ah\n\
	              mov ah, al\n\
	              xor al, al\n\
	              div cx\n"
	             : /* Output */ "=ax"(ret)
	             : /* Input */[ a ] "ax"(a), [ b ] "cx"(b)
	             : /* Clobbers */ "dx");

	return ret;

#else
	return (ufixed_t)(((uint32_t)a << 8) / b);
#endif
}


ufixed_t UFixedStep(ufixed_t edge0, ufixed_t edge1, ufixed_t v)
{
	edge1 = edge1 - edge0;
	edge0 = v - edge0;

	edge0 = UFixedDivide(edge0, edge1);
	edge0 = CLAMP(edge0, 0x0000, UFixedMake(1, 0));

	return edge0;
}


#ifdef FIXED_STANDALONE
#include <stdio.h>


static void sPrint(ufixed_t f)
{
#if defined(SAKURAI_DOS)
	printf("%02X %02X (%04X)\n", FixedWhole(f), FixedFraction(f), f);
#else
	printf("%02X %02X (%f)\n", FixedWhole(f), FixedFraction(f), FixedToFloat(f));
#endif
}


int main()
{
	ufixed_t one, two, three, six;
	ufixed_t half, quarter, three_quarters;
	ufixed_t percent75, percent50, percent25;

	printf("\nMultiply\n");

	one = UFixedMake(1, 0);
	sPrint(one);
	two = UFixedMake(2, 0);
	sPrint(two);
	three = one + two;
	sPrint(three);
	six = UFixedMultiply(three, two);
	sPrint(six);

	printf("\n");

	half = UFixedMake(0, 128);
	sPrint(half);
	quarter = UFixedMake(0, 64);
	sPrint(quarter);
	three_quarters = half + quarter;
	sPrint(three_quarters);

	printf("\n");

	percent75 = UFixedMultiply(UFixedMake(100, 0), three_quarters);
	sPrint(percent75);
	percent50 = UFixedMultiply(UFixedMake(100, 0), half);
	sPrint(percent50);
	percent25 = UFixedMultiply(UFixedMake(100, 0), quarter);
	sPrint(percent25);

	printf("\nDivide\n");

	half = UFixedDivide(one, UFixedMake(2, 0));
	sPrint(half);
	quarter = UFixedDivide(one, UFixedMake(4, 0));
	sPrint(quarter);

	printf("\nStep\n");

	one = UFixedStep(UFixedMake(0, 0), UFixedMake(100, 0), 100);
	sPrint(one);

	one = UFixedStep(UFixedMake(0, 0), UFixedMake(100, 0), 200);
	sPrint(one);

	half = UFixedStep(UFixedMake(0, 0), UFixedMake(100, 0), 50);
	sPrint(half);

	return 0;
}
#endif
