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

 [utilities.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "utilities.h"

static uint16_t s_random_state = 1;
static int8_t s_sin_table[128] = {
    3,   6,   9,   12,  15,  18,  21,  24,  27,  30,  33,  36,  39,  42,  45,  48,  51,  54,  57,  59,  62,  65,
    67,  70,  73,  75,  78,  80,  82,  85,  87,  89,  91,  94,  96,  98,  100, 102, 103, 105, 107, 108, 110, 112,
    113, 114, 116, 117, 118, 119, 120, 121, 122, 123, 123, 124, 125, 125, 126, 126, 126, 126, 126, 127, 126, 126,
    126, 126, 126, 125, 125, 124, 123, 123, 122, 121, 120, 119, 118, 117, 116, 114, 113, 112, 110, 108, 107, 105,
    103, 102, 100, 98,  96,  94,  91,  89,  87,  85,  82,  80,  78,  75,  73,  70,  67,  65,  62,  59,  57,  54,
    51,  48,  45,  42,  39,  36,  33,  30,  27,  24,  21,  18,  15,  12,  9,   6,   3,   0};


void Seed(uint16_t value)
{
	s_random_state = value;
}


uint16_t Random()
{
	/* http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html */
	s_random_state ^= s_random_state << 7;
	s_random_state ^= s_random_state >> 9;
	s_random_state ^= s_random_state << 8;
	return s_random_state;
}


int8_t __attribute__((optimize("O0"))) Sin(uint8_t x)
{
	/* Optimizations off because GCC reports:
	   Internal compiler error: in elimination_costs_in_insn, at reload1.c:3624 */

	return (x > 128) ? -(s_sin_table[x % 128]) : s_sin_table[x % 128];
}


void Clear(void* dest, uint16_t size)
{
	void* end;

	if ((size % 2) != 0)
	{
		*((uint8_t*)dest) = 0;
		dest = (uint8_t*)dest + 1;
		size -= 1;
	}

	if (size == 0)
		return;

	end = (uint8_t*)dest + size;

	while (dest != end)
	{
		*((uint16_t*)dest) = 0;
		dest = (uint16_t*)dest + 1;
	}
}


void Copy(const void* org, void* dest, uint16_t size)
{
	void* end;

	if ((size % 2) != 0)
	{
		*((uint8_t*)dest) = *((uint8_t*)org);
		dest = (uint8_t*)dest + 1;
		org = (uint8_t*)org + 1;
		size -= 1;
	}

	if (size == 0)
		return;

	end = (uint8_t*)dest + size;

	while (dest != end)
	{
		*((uint16_t*)dest) = *((uint16_t*)org);
		dest = (uint16_t*)dest + 1;
		org = (uint16_t*)org + 1;
	}
}


char* NumberToString(uint8_t no, char* out)
{
	/* https://stackoverflow.com/a/32871108 */
	char* c = out + 3;

	do
	{
		c -= 1;
		*c = 0x30 + (no % 10); /* NOLINT: bugprone-narrowing-conversions */
		no /= 10;

	} while (no != 0);

	return c;
}


uint8_t KeyToggle(uint8_t in, uint8_t* state)
{
	if (in == 0)
		*state = 1;
	else if (*state == 1)
	{
		*state = 0;
		return 1;
	}

	return 0;
}


uint8_t KeyRepeat(uint8_t in, uint8_t* state)
{
	if (in == 0)
		*state = 4;
	else
	{
		*state += 1;

		if (*state >= 4)
		{
			*state = 0;
			return 1;
		}
	}

	return 0;
}
