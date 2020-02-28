/*

	$ bcc -0 -Md -x -i ./source/battle.c -o game.com
	$ x86dis -L -e 0x00 -s intel < ./game.com

	$ clang -fsyntax-only -std=c90 -Wall -Wextra -Wconversion -m16 ./source/battle.c \
	  bcc -0 -Md -x -i ./source/battle.c -o game.com

	bcc -O = Optimize code (ilegible in asm form)


	MAIN() ALWAYS AT THE BEGINNING
	Seems that without the crt, bcc has some troubles

*/


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

static uint8_t Test1();
static uint16_t Test2();

struct Table
{
	uint16_t foo;
	uint16_t bar;
	uint8_t bar2;
	uint8_t pad;
};

struct Table s_table = {0xAA, 0xBB, 0xCC, 0x00};


int main()
{
	s_table.foo = 99;
	s_table.bar = 100;
	s_table.bar2 = Test1();

	return (int)(&s_table);
}


static uint8_t Test1()
{
	return 13;
}


static uint16_t Test2()
{
	return 14;
}

