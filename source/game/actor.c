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

 [actor.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "actor.h"
#include "fixed.h"
#include "utilities.h"


extern uint8_t EnemiesNumber(uint8_t battle_no);
extern uint8_t EnemyChances(uint8_t enemy_i, ufixed_t battle_no);


/* Reused all over the file to iterate personalities*/
static uint16_t s_temp[ENEMIES_NO + HEROES_NO];

/* Used to move suffle actors */
struct Actor s_actor_temp;


uint8_t ActorsInitialize(uint8_t battle_no)
{
	uint8_t i = 0;
	uint8_t e = 0;
	uint8_t enemies_no;
	uint16_t sum = 0;

	enemies_no = EnemiesNumber(battle_no);

	TraitsInitialize();

	/* Enemies chances for this battle */
	IntPrintText("Enemies chances:\n");

	for (i = 0; i < ENEMIES_NO; i++)
	{
		s_temp[i] = EnemyChances(i, UFixedMake(battle_no, 0));
		sum += s_temp[i]; /* To normalizate */

		IntPrintNumber(s_temp[i]);
	}

	/* Initialize heroes constant information, reset heroes health
	and magic only if is the first battle */
	g_actor[0].persona = &g_heroes[HERO_KURO];
	g_actor[0].x = 45;
	g_actor[0].y = 60;

	g_actor[1].persona = &g_heroes[HERO_SAO];
	g_actor[1].x = 16;
	g_actor[1].y = 90;

	for (i = 0; i < HEROES_NO; i++)
	{
		g_actor[i].target = NULL;
		g_actor[i].phase = (uint8_t)Random();
		g_actor[i].action = NULL;
		g_actor[i].idle_timer = 0;
		g_actor[i].charge_timer = 0;
		g_actor[i].recover_timer = 0;

		if (battle_no == 0)
		{
			/* Revive heroes for battle zero */
			g_actor[i].state = ACTOR_STATE_IDLE;
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
		else
		{
			/* Just reset state for other battles */
			if (g_actor[i].state != ACTOR_STATE_DEAD)
				g_actor[i].state = ACTOR_STATE_IDLE;
		}
	}

	/* Initialize enemies, the remainder actors */
	{
		/* Set the state and chose a personality between the chances */
		for (i = HEROES_NO; i < ACTORS_NO; i++)
		{
			Clear(&g_actor[i], sizeof(struct Actor));

			if (i >= HEROES_NO + enemies_no) /* Exceeded number of enemies for this battle */
			{
				g_actor[i].state = ACTOR_STATE_DEAD;
				continue;
			}

			g_actor[i].state = ACTOR_STATE_IDLE;
			g_actor[i].phase = (uint8_t)Random();

		again:

			e = ((uint8_t)Random() % ENEMIES_NO);
			if ((Random() % sum) <= s_temp[e])
				g_actor[i].persona = &g_enemies[e];
			else
				goto again; /* TODO, limit this to some tries */
		}

		/* Shuffle positions, including those of enemies set as STATE_DEAD */
		for (i = HEROES_NO; i < ACTORS_NO; i++)
		{
			e = ((uint8_t)Random() % (ACTORS_NO - HEROES_NO)) + HEROES_NO;
			Copy(&g_actor[e], &s_actor_temp, sizeof(struct Actor));
			Copy(&g_actor[i], &g_actor[e], sizeof(struct Actor));
			Copy(&s_actor_temp, &g_actor[i], sizeof(struct Actor));
		}

		/* One last iteration */
		Clear(s_temp, sizeof(uint16_t) * (ENEMIES_NO + HEROES_NO)); /* To reuse it */

		for (i = HEROES_NO; i < ACTORS_NO; i++)
		{
			/* Set screen position */
			if (i != HEROES_NO)
			{
				g_actor[i].x = g_actor[i - 1].x + 32;
				g_actor[i].y = g_actor[i - 1].y + 10;
			}
			else
			{
				g_actor[i].x = 144;
				g_actor[i].y = 60;
			}

			if (g_actor[i].state == ACTOR_STATE_DEAD)
				continue;

			/* Ensure that difficult enemies don't appear more than twice */
			e = EnemyPersonaIndex(g_actor[i].persona);
			s_temp[e] += 1;

			if ((g_actor[i].persona->tags & TAG_DIFFICULT) && s_temp[e] > 2)
			{
				IntPrintText("Replaced difficult enemy ");
				IntPrintNumber(e);
				g_actor[i].persona = &g_enemies[Random() % 2]; /* HARDCODED */
			}

			/* Finally set health and magic based on the personality */
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
	}

	return enemies_no;
}


void ActorsInitializeSprites()
{
	/* TODO, temporary until I write a 'resources' module */

	uint8_t i = 0;
	uint8_t index = 0;

	Clear(s_temp, sizeof(uint16_t) * (ENEMIES_NO + HEROES_NO)); /* To reuse it */

	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		if (i >= HEROES_NO)
			index = EnemyPersonaIndex(g_actor[i].persona) + HEROES_NO;
		else
			index = HeroPersonaIndex(g_actor[i].persona);

		if (s_temp[index] == 0)
		{
			s_temp[index] = 1;
			IntLoadSprite(g_actor[i].persona->sprite_filename, 16 + index);
		}

		g_actor[i].sprite = 16 + index;
	}
}


void ActorsDraw()
{
	uint8_t i = 0;
	uint8_t width = 0;
	uint8_t color = 0;
	uint16_t x = 0;

	/* Clean area */
	CmdDrawRectangleBkg(20 /* 320 px */, 8 /* 128 px */, 0, 60);

	/* Draw actors */
	for (i = 0; i < ACTORS_NO; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		/* Most states just require an static sprite */
		if (g_actor[i].state != ACTOR_STATE_CHARGE)
		{
			color = 8;
			x = g_actor[i].x;

			if (g_actor[i].state == ACTOR_STATE_IDLE)
				width = (g_actor[i].idle_timer >> 3); /* Max of 32 px */
			else
				width = 0;
		}

		/* Charge requires an oscillating sprite */
		else
		{
			color = 41;
			width = (g_actor[i].charge_timer >> 3); /* Max of 32 px */

			if (g_actor[i].recover_timer == 0)
				g_actor[i].phase += g_actor[i].action->oscillation_velocity;

			x = (uint16_t)((int16_t)g_actor[i].x + ((int16_t)Sin(g_actor[i].phase) >> 5));
		}

		if (g_actor[i].recover_timer > 0) /* Overwrite whatever state is */
			color = 60;

		/* Draw sprite */
		CmdDrawSprite(g_actor[i].sprite, x, g_actor[i].y, 0);

		/* Draw time meter */
		if (g_actor[i].state != ACTOR_STATE_VICTORY)
		{
			CmdDrawRectanglePrecise(34, 3, g_actor[i].x, g_actor[i].y, 16);

			if (width > 0)
				CmdDrawRectanglePrecise(width, 1, g_actor[i].x + 1, g_actor[i].y + 1, color);
		}
	}
}


static struct Actor* sFindTarget(struct Actor* actor)
{
	uint8_t i = 0;
	uint8_t step = 0;

	/* Find a hero for this enemy */
	if (actor->persona->tags & TAG_ENEMY)
	{
		i = (uint8_t)Random() % HEROES_NO;

		for (step = 0; step < HEROES_NO; step++)
		{
			if (g_actor[i].state != ACTOR_STATE_DEAD)
				return &g_actor[i];

			i = (i + 1) % HEROES_NO;
		}
	}

	/* Find an enemy for this hero */
	else
	{
		i = (uint8_t)Random() % (ACTORS_NO - HEROES_NO);

		for (step = 0; step < (ACTORS_NO - HEROES_NO); step++)
		{
			if (g_actor[HEROES_NO + i].state != ACTOR_STATE_DEAD)
				return &g_actor[HEROES_NO + i];

			i = (i + 1) % (ACTORS_NO - HEROES_NO);
		}
	}

	return NULL; /* No suitable target alive */
}


static void sSetIdleState(struct Actor* actor)
{
	actor->idle_timer = 0;
	actor->state = ACTOR_STATE_IDLE;
}


static void sSetChargeState(struct Actor* actor)
{
	uint8_t i = 0;

	actor->charge_timer = 0;
	actor->state = ACTOR_STATE_CHARGE;

	/* Chose an attack */
	if ((uint8_t)(Random() % 100) < actor->persona->actions_preference)
		actor->action = actor->persona->action_a;
	else
		actor->action = actor->persona->action_b;

	/* Chose a target */
	if ((actor->target = sFindTarget(actor)) == NULL)
	{
		/* No target alive, is a victory then! */
		actor->state = ACTOR_STATE_VICTORY;
	}
}


static int sAttack(struct Actor* actor)
{
	/* If our target is dead, find another */
	if (actor->target->state == ACTOR_STATE_DEAD)
	{
		if ((actor->target = sFindTarget(actor)) == NULL)
			return 1; /* No luck */
	}

	/* Apply the action */
	actor->target->recover_timer = 255; /* TODO: use an tag in the action */

	actor->action->callback(actor->action, actor);

	if (actor->target->health == 0)
		actor->target->state = ACTOR_STATE_DEAD;

	return 0;
}


void ActorLogic(struct Actor* actor)
{
	if (actor->recover_timer != 0)
	{
		if (actor->recover_timer > actor->persona->recover_velocity)
		{
			actor->recover_timer -= actor->persona->recover_velocity;
			return;
		}
		else
			actor->recover_timer = 0;
	}

	switch (actor->state)
	{
	case ACTOR_STATE_IDLE:
		if (actor->idle_timer < (255 - actor->persona->idle_velocity))
			actor->idle_timer += actor->persona->idle_velocity;
		else
			sSetChargeState(actor); /* Can set victory */
		break;

	case ACTOR_STATE_CHARGE:
		if (actor->charge_timer < (255 - actor->action->charge_velocity))
			actor->charge_timer += actor->action->charge_velocity;
		else
			/* Attack don't require any special setting
			except those already set by sSetChargeState() */
			actor->state = ACTOR_STATE_ATTACK;
		break;

	case ACTOR_STATE_ATTACK:
		if (sAttack(actor) == 0)
			sSetIdleState(actor);
		else
		{ /* If wasn't possible procede with the attack, means
			 that there is nobody to attack */
			actor->state = ACTOR_STATE_VICTORY;
		}
		break;

	default: break;
	}
}
