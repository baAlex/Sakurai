/*

 $ bcc -0 -Md -x -i ./source/game.c -o game.com
 $ x86dis -L -e 0x00 -s intel < ./game.com

 $ clang -fsyntax-only -std=c90 -Wall -Wextra -Wconversion -m16 ./source/game.c \
   bcc -ansi -0 -Md -x -i ./source/game.c -o game.com

 bcc -O = Optimize code (PROTIP: illegible in asm form)


 MAIN() ALWAYS AT THE BEGINNING
 Seems that without the crt, bcc has some troubles

*/

#define INSTRUCTIONS_TABLE_LEN 16
#define INSTRUCTIONS_TABLE_START 0x0000
#define INSTRUCTIONS_TABLE_END 0x0080

#define CODE_HALT 0x00           /* Stops draw routine */
#define CODE_DRAW_BKG 0x01       /* Draw loaded background into buffer */
#define CODE_DRAW_PIXEL 0x02     /* Draw pixel into buffer */
#define CODE_LOAD_BKG 0x03       /* Load a background file */
#define CODE_DRAW_RECTANGLE 0x04 /* ... you got the idea */

typedef signed char int8_t;
typedef signed short int16_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

struct InstructionLoad
{
	uint8_t code;
	uint8_t unused1;
	uint16_t filename;
	uint16_t unused2;
	uint16_t unused3;
};

struct InstructionDraw
{
	uint8_t code;
	uint8_t color;
	uint8_t width;  /* Multiplied by 16 */
	uint8_t height; /* Multiplied by 16 */
	uint16_t x;
	uint16_t y;
};

union Instruction {
	uint8_t code;

	/* DRAW_BKG, DRAW_PIXEL, DRAW_RECTANGLE */
	struct InstructionDraw draw;

	/* LOAD_BKG */
	struct InstructionLoad load;
};

static uint16_t Random();
static int8_t Sin(uint8_t a);

static uint16_t s_frame = 0;
static uint16_t s_marsaglia = 1;
static int8_t s_sin_table[128] = {
    3,   6,   9,   12,  15,  18,  21,  24,  27,  30,  33,  36,  39,  42,  45,  48,  51,  54,  57,  59,  62,  65,
    67,  70,  73,  75,  78,  80,  82,  85,  87,  89,  91,  94,  96,  98,  100, 102, 103, 105, 107, 108, 110, 112,
    113, 114, 116, 117, 118, 119, 120, 121, 122, 123, 123, 124, 125, 125, 126, 126, 126, 126, 126, 127, 126, 126,
    126, 126, 126, 125, 125, 124, 123, 123, 122, 121, 120, 119, 118, 117, 116, 114, 113, 112, 110, 108, 107, 105,
    103, 102, 100, 98,  96,  94,  91,  89,  87,  85,  82,  80,  78,  75,  73,  70,  67,  65,  62,  59,  57,  54,
    51,  48,  45,  42,  39,  36,  33,  30,  27,  24,  21,  18,  15,  12,  9,   6,   3,   0,
};


int main()
{
	union Instruction* ins = (void*)(INSTRUCTIONS_TABLE_START);

	if ((s_frame % 192) != 0)
	{
		/* Some sparkling things */
		ins[0].code = CODE_DRAW_PIXEL;
		ins[0].draw.color = 30 + Random() % 8;
		ins[0].draw.x = 160 + (uint16_t)(Sin((s_frame) % 255)) + Random() % 4;
		ins[0].draw.y = 100 + (uint16_t)(Sin((s_frame << 2) % 255) >> 1) + Random() % 4;

		ins[1].code = CODE_DRAW_PIXEL;
		ins[1].draw.color = 8;
		ins[1].draw.x = 160 + (uint16_t)(Sin((s_frame + s_frame) % 255)) + Random() % 16;
		ins[1].draw.y = 100 + (uint16_t)(Sin((s_frame << 3) % 255) >> 1) + Random() % 16;

		/* Two rectangles */
		ins[2].code = CODE_DRAW_RECTANGLE;
		ins[2].draw.color = 50;
		ins[2].draw.x = 2;
		ins[2].draw.y = 2;
		ins[2].draw.width = 3;  /* Multiplied by 16 */
		ins[2].draw.height = 3; /* " */

		ins[3].code = CODE_DRAW_RECTANGLE;
		ins[3].draw.color = 51;
		ins[3].draw.x = 52;
		ins[3].draw.y = 2;
		ins[3].draw.width = 3;  /* Multiplied by 16 */
		ins[3].draw.height = 3; /* " */

		/* Everything done here */
		ins[4].code = CODE_HALT;
	}
	else
	{
		/* We need to load the specified background
		   and draw it into the entire buffer, both
		   slow operations */

		ins[0].code = CODE_LOAD_BKG;

		switch (Random() % 8)
		{
		case 0: ins[0].load.filename = (uint16_t) "assets\\bkg1.dat"; break;
		case 1: ins[0].load.filename = (uint16_t) "assets\\bkg2.dat"; break;
		case 2: ins[0].load.filename = (uint16_t) "assets\\bkg3.dat"; break;
		case 3: ins[0].load.filename = (uint16_t) "assets\\bkg4.dat"; break;
		case 4: ins[0].load.filename = (uint16_t) "assets\\bkg5.dat"; break;
		case 5: ins[0].load.filename = (uint16_t) "assets\\bkg6.dat"; break;
		case 6: ins[0].load.filename = (uint16_t) "assets\\bkg7.dat"; break;
		case 7: ins[0].load.filename = (uint16_t) "assets\\bkg8.dat";
		}

		ins[1].code = CODE_DRAW_BKG;
		ins[2].code = CODE_HALT;
	}

	s_frame++;
	return 0;
}


static uint16_t Random()
{
	/* http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html */
	s_marsaglia ^= s_marsaglia << 7;
	s_marsaglia ^= s_marsaglia >> 9;
	s_marsaglia ^= s_marsaglia << 8;
	return s_marsaglia;
}


static int8_t Sin(uint8_t a)
{
	int8_t r = s_sin_table[a % 128];
	return (a > 128) ? -r : r;
}
