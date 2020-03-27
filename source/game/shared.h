/*-----------------------------

 [shared.h]
 - Alexander Brandt 2020
-----------------------------*/

typedef signed char int8_t;
typedef signed short int16_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define UINT8_MAX 255
#define UINT16_MAX 65535

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define NULL 0xFFFF

#define COMMANDS_TABLE_LEN 24
#define COMMANDS_TABLE_OFFSET 0x0000
#define FRAME_COUNTER_OFFSET 0x00C0
#define INT_FD_ARG1 0x00C2
#define INT_FD_ARG2 0x00C4
#define INT_FD_ARG3 0x00C6
#define INT_FD_ARG4 0x00C8

#define CODE_HALT 0x00           /* Stops draw routine */
#define CODE_DRAW_BKG 0x01       /* Draw loaded background into buffer */
#define CODE_DRAW_PIXEL 0x02     /* Draw pixel into buffer */
#define CODE_DRAW_RECTANGLE 0x04 /* ... you got the idea */
#define CODE_DRAW_RECTANGLE_BKG 0x05
#define CODE_DRAW_RECTANGLE_PRECISE 0x06
#define CODE_DRAW_SPRITE 0x07

struct CommandDrawShape
{
	uint8_t code;
	uint8_t color;
	uint8_t width;  /* Multiplied by 16 */
	uint8_t height; /* Multiplied by 16 */
	uint16_t x;
	uint16_t y;
};

struct CommandGameDrawSprite
{
	uint8_t code;
	uint8_t slot;
	uint8_t frame;
	uint8_t mode;
	uint16_t x;
	uint16_t y;
};

union Command {
	uint8_t code;

	/* DRAW_PIXEL, DRAW_RECTANGLE, DRAW_RECTANGLE_BKG
	   DRAW_RECTANGLE_PRECISE */
	struct CommandDrawShape draw_shape;

	/* DRAW_SPRITE */
	struct CommandGameDrawSprite draw_sprite;
};


#define ACTORS_NUMBER 6
#define TYPES_NUMBER 8

#define TYPE_HERO_A 0
#define TYPE_HERO_B 1
#define TYPE_A 2
#define TYPE_B 3
#define TYPE_C 4
#define TYPE_D 5
#define TYPE_E 6
#define TYPE_F 7
#define TYPE_DEAD 0xFF
#define TYPE_NONE 0xFF

#define STATE_IDLE 0
#define STATE_CHARGE 1
#define STATE_BOUNDED 2
#define STATE_ATTACK 3

struct Actor
{
	uint16_t x;

	uint8_t target;
	uint8_t attack_type;

	uint8_t state;
	uint8_t next_state;

	uint8_t idle_time;
	uint8_t charge_time;
	uint8_t bounded_time;
	uint8_t type;

	int8_t health;
	uint8_t phase;
};


/* ---- */


uint16_t Random();
int8_t Sin(uint8_t a);

union Command* NewCommand(uint8_t code);
void CleanCommands();

void PrintString(uint16_t string);
void PrintNumber(uint16_t number);
void LoadBackground(uint16_t filename);
void LoadSprite(uint16_t filename, uint16_t slot);
