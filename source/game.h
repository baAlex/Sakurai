/*-----------------------------

 [game.h]
 - Alexander Brandt 2020
-----------------------------*/

typedef signed char int8_t;
typedef signed short int16_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define NULL 0xFFFF

#define INSTRUCTIONS_TABLE_LEN 16
#define INSTRUCTIONS_TABLE_START 0x0000
#define INSTRUCTIONS_TABLE_END 0x0080

#define CODE_HALT 0x00           /* Stops draw routine */
#define CODE_DRAW_BKG 0x01       /* Draw loaded background into buffer */
#define CODE_DRAW_PIXEL 0x02     /* Draw pixel into buffer */
#define CODE_LOAD_BKG 0x03       /* Load a background file */
#define CODE_DRAW_RECTANGLE 0x04 /* ... you got the idea */
#define CODE_DRAW_RECTANGLE_BKG 0x05

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


#define ACTORS_NUMBER 6

struct Actor
{
	uint16_t x;
	uint16_t previous_x;

	uint8_t action_type;
	uint8_t action_to_actor;
	uint8_t idle_time;
	uint8_t preparation_time;

	uint8_t health;
	uint8_t magic;
	uint8_t unused1;
	uint8_t unused2;

	uint8_t animation;
	uint8_t frame;
	uint8_t previous_frame;
	uint8_t unused3;
};


/* ---- */


static uint16_t Random();
static int8_t Sin(uint8_t a);
static union Instruction* NewInstruction(uint8_t code);


static uint16_t s_frame = 0;
static uint8_t s_instructions_counter = 0;

static uint16_t s_marsaglia = 1;
static int8_t s_sin_table[128] = {
    3,   6,   9,   12,  15,  18,  21,  24,  27,  30,  33,  36,  39,  42,  45,  48,  51,  54,  57,  59,  62,  65,
    67,  70,  73,  75,  78,  80,  82,  85,  87,  89,  91,  94,  96,  98,  100, 102, 103, 105, 107, 108, 110, 112,
    113, 114, 116, 117, 118, 119, 120, 121, 122, 123, 123, 124, 125, 125, 126, 126, 126, 126, 126, 127, 126, 126,
    126, 126, 126, 125, 125, 124, 123, 123, 122, 121, 120, 119, 118, 117, 116, 114, 113, 112, 110, 108, 107, 105,
    103, 102, 100, 98,  96,  94,  91,  89,  87,  85,  82,  80,  78,  75,  73,  70,  67,  65,  62,  59,  57,  54,
    51,  48,  45,  42,  39,  36,  33,  30,  27,  24,  21,  18,  15,  12,  9,   6,   3,   0,
};


/* BCC is somewhat stupid initializing static variables */
static struct Actor s_actor[ACTORS_NUMBER] = {
    /* Our heroes */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /* Enemies */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

static uint16_t s_base_x[ACTORS_NUMBER] = {
    /* Our heroes */
    38, 8,
    /* Enemies */
    180, 202, 225, 248};

static uint16_t s_base_y[ACTORS_NUMBER] = {
    /* Our heroes */
    60, 100,
    /* Enemies */
    60, 73, 86, 100};

static uint16_t s_phase[ACTORS_NUMBER] = {333, 222, 115, 257, 865, 612};
