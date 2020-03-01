/*

	$ bcc -0 -Md -x -i ./source/battle.c -o game.com
	$ x86dis -L -e 0x00 -s intel < ./game.com

	$ clang -fsyntax-only -std=c90 -Wall -Wextra -Wconversion -m16 ./source/battle.c \
	  bcc -0 -Md -x -i ./source/battle.c -o game.com

	bcc -O = Optimize code (ilegible in asm form)


	MAIN() ALWAYS AT THE BEGINNING
	Seems that without the crt, bcc has some troubles

*/

#define DTABLE_LEN 16
#define DTABLE_START 0x0000
#define DTABLE_END 0x0080

#define CODE_HALT 0x00
#define CODE_DRAW_BKG 0x01 /* Draw the loaded background into the buffer, slow as hell */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

struct DInstruction
{
	uint8_t code;
	uint8_t argument;

	uint8_t unused1;
	uint8_t unused2;

	uint16_t x;
	uint16_t y;
};

static int s_frame = 0;


int main()
{
	struct DInstruction* ins = (void*)(DTABLE_START);

	ins[0].code = (s_frame != 0) ? CODE_HALT : CODE_DRAW_BKG;
	ins[1].code = CODE_HALT;

	s_frame++;

	/*for(; ins < (struct DInstruction*)(DTABLE_END); ins++)
	{
		ins->code = 0xAB;
		ins->argument = 0xAB;
		ins->x = 0xAB;
		ins->y = 0xAB;
	}*/

	return 0;
}

