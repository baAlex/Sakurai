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


#define ARENA_Y 56
#define ARENA_HEIGHT 8 /* 128 px */
#define ENEMIES_X 144
#define ACTORS_SPACING_X 32
#define ACTORS_SPACING_Y 10
#define SAO_X 16
#define KURO_X 45


static uint16_t s_temp[ENEMIES_PERSONALITIES_NO + HEROES_PERSONALITIES_NO]; /* To keep things while iterating actors */
struct Actor s_actor_temp;


extern uint8_t EnemiesNumber(uint8_t battle_no);
extern uint8_t EnemyChances(uint8_t enemy_i, ufixed_t battle_no);


void ActorsInitialize(uint8_t battle_no)
{
	uint8_t i = 0;
	uint8_t e = 0;
	uint16_t sum = 0;

	g_live_enemies = EnemiesNumber(battle_no);

	TraitsInitialize();

	IntPrintText("Battle: ");
	IntPrintNumber(battle_no + 1);

	IntPrintText("Enemies number: ");
	IntPrintNumber(g_live_enemies);

	/* Enemies chances for this battle */
	IntPrintText("Enemies chances:\n");

	for (i = 0; i < ENEMIES_PERSONALITIES_NO; i++)
	{
		s_temp[i] = EnemyChances(i, UFixedMake(battle_no, 0));
		sum += s_temp[i]; /* To normalize them */

		IntPrintNumber(s_temp[i]);
	}

	/* Initialize heroes constant information, if is the first battle reset them */
	if (battle_no == 0)
		g_live_heroes = 2;

	g_actor[0].persona = &g_heroes[ACTOR_KURO];
	g_actor[0].x = KURO_X;
	g_actor[0].y = ARENA_Y;

	g_actor[1].persona = &g_heroes[ACTOR_SAO];
	g_actor[1].x = SAO_X;
	g_actor[1].y = ARENA_Y + (ACTORS_SPACING_Y * 2) + (ACTORS_SPACING_Y >> 1);

	for (i = 0; i < ON_SCREEN_HEROES; i++)
	{
		g_actor[i].target = NULL;
		g_actor[i].phase = (uint8_t)Random();
		g_actor[i].action = NULL;
		g_actor[i].idle_timer = 0;
		g_actor[i].charge_timer = 0;
		g_actor[i].recover_timer = 0;
		g_actor[i].effects = EFFECT_NONE;

		if (battle_no == 0)
		{
			/* Revive heroes for battle zero */
			g_actor[i].state = ACTOR_STATE_IDLE;
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
		else
		{
			/* For other battles just reset the state */
			if (g_actor[i].state != ACTOR_STATE_DEAD)
				g_actor[i].state = ACTOR_STATE_IDLE;
		}
	}

	/* Initialize enemies */
	{
		/* Set the state and chose a personality between the chances */
		for (i = ON_SCREEN_HEROES; i < ON_SCREEN_ACTORS; i++)
		{
			Clear(&g_actor[i], sizeof(struct Actor));

			/* Exceeded number of enemies for this battle,
			   set actor as already dead */
			if (i >= ON_SCREEN_HEROES + g_live_enemies)
			{
				g_actor[i].state = ACTOR_STATE_DEAD;
				continue;
			}

			g_actor[i].state = ACTOR_STATE_IDLE;
			g_actor[i].phase = (uint8_t)Random();
			g_actor[i].effects = EFFECT_NONE;
			g_actor[i].action = NULL;

		again:

			e = ((uint8_t)Random() % ENEMIES_PERSONALITIES_NO);
			if ((Random() % sum) < s_temp[e])
				g_actor[i].persona = &g_enemies[e];
			else
				goto again; /* TODO, limit this! */
		}

		/* Shuffle positions, including those of dead enemies */
		for (i = ON_SCREEN_HEROES; i < ON_SCREEN_ACTORS; i++)
		{
			e = ((uint8_t)Random() % (ON_SCREEN_ACTORS - ON_SCREEN_HEROES)) + ON_SCREEN_HEROES;
			Copy(&g_actor[e], &s_actor_temp, sizeof(struct Actor));
			Copy(&g_actor[i], &g_actor[e], sizeof(struct Actor));
			Copy(&s_actor_temp, &g_actor[i], sizeof(struct Actor));
		}

		/* One last iteration */
		Clear(s_temp, sizeof(uint16_t) * (ENEMIES_PERSONALITIES_NO + HEROES_PERSONALITIES_NO)); /* To reuse it */

		for (i = ON_SCREEN_HEROES; i < ON_SCREEN_ACTORS; i++)
		{
			/* Set screen position */
			if (i != ON_SCREEN_HEROES)
			{
				g_actor[i].x = g_actor[i - 1].x + ACTORS_SPACING_X;
				g_actor[i].y = g_actor[i - 1].y + ACTORS_SPACING_Y;
			}
			else
			{
				g_actor[i].x = ENEMIES_X;
				g_actor[i].y = ARENA_Y;
			}

			if (g_actor[i].state == ACTOR_STATE_DEAD)
				continue;

			/* Ensure that difficult enemies don't appear more than twice */
			e = EnemyPersonaIndex(g_actor[i].persona);
			s_temp[e] += 1;

			if ((g_actor[i].persona->tags & TAG_PERSONA_DIFFICULT) && s_temp[e] > 2)
			{
				IntPrintText("Replaced difficult enemy ");
				IntPrintNumber(e);

				/* Replace personality with one of the 'well balanced' enemies */
				g_actor[i].persona = &g_enemies[Random() % 2]; /* HARDCODED */
			}

			/* Finally set health and magic based on the personality */
			g_actor[i].health = g_actor[i].persona->initial_health;
			g_actor[i].magic = g_actor[i].persona->initial_magic;
		}
	}
}


void ActorsInitializeSprites()
{
	uint8_t i = 0;
	uint8_t index = 0;

	Clear(s_temp, sizeof(uint16_t) *
	                  (ENEMIES_PERSONALITIES_NO + HEROES_PERSONALITIES_NO)); /* To keep track of loaded sprites */

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		/* The personality indicates what sprite load */
		if (i >= HEROES_PERSONALITIES_NO)
			index = EnemyPersonaIndex(g_actor[i].persona) + HEROES_PERSONALITIES_NO;
		else
			index = HeroPersonaIndex(g_actor[i].persona);

		if (s_temp[index] == 0)
		{
			s_temp[index] = 1;
			g_actor[i].persona->sprite = IntLoadSprite(g_actor[i].persona->sprite_filename);
		}
	}
}


void ActorsDraw(uint8_t oscillate)
{
	uint8_t i = 0;
	uint8_t width = 0;
	uint16_t x = 0;

	CmdDrawRectangleBkg(20 /* 320 px */, ARENA_HEIGHT, 0, ARENA_Y); /* Clean arena */

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD)
			continue;

		else if (g_actor[i].state == ACTOR_STATE_IDLE)
		{
			/* A 'bloody' sprite frame if is recovering */
			if (g_actor[i].recover_timer == 0)
				CmdDrawSprite(g_actor[i].persona->sprite, g_actor[i].x, g_actor[i].y, 0);
			else
				CmdDrawSprite(g_actor[i].persona->sprite, g_actor[i].x, g_actor[i].y, 1);

			/* Time meter */
			CmdDrawRectanglePrecise(34, 3, g_actor[i].x, g_actor[i].y, 64);
			width = (g_actor[i].idle_timer >> 3); /* Max of 32 px */

			if (width > 0)
			{
				if (g_actor[i].recover_timer == 0) /* Darker timer if recovering */
					CmdDrawRectanglePrecise(width, 1, g_actor[i].x + 1, g_actor[i].y + 1, 8);
				else
					CmdDrawRectanglePrecise(width, 1, g_actor[i].x + 1, g_actor[i].y + 1, 5);
			}
		}

		else if (g_actor[i].state == ACTOR_STATE_CHARGE)
		{
			/* Same as before, now with an oscillating sprite */
			if (g_actor[i].recover_timer == 0)
			{
				if (oscillate == 1 && g_actor[i].action != NULL)
					g_actor[i].phase += g_actor[i].action->oscillation_velocity;

				x = (uint16_t)((int16_t)g_actor[i].x + ((int16_t)Sin(g_actor[i].phase) >> 5));
				CmdDrawSprite(g_actor[i].persona->sprite, x, g_actor[i].y, 0);
			}
			else
				CmdDrawSprite(g_actor[i].persona->sprite, g_actor[i].x, g_actor[i].y, 1);

			/* Time meter */
			CmdDrawRectanglePrecise(34, 3, g_actor[i].x, g_actor[i].y, 64);
			width = (g_actor[i].charge_timer >> 3); /* Max of 32 px */

			if (width > 0)
			{
				if (g_actor[i].recover_timer == 0)
					CmdDrawRectanglePrecise(width, 1, g_actor[i].x + 1, g_actor[i].y + 1, 41);
				else
					CmdDrawRectanglePrecise(width, 1, g_actor[i].x + 1, g_actor[i].y + 1, 52);
			}
		}

		else if (g_actor[i].state == ACTOR_STATE_HOLD)
		{
			CmdDrawSprite(g_actor[i].persona->sprite, g_actor[i].x, g_actor[i].y, 0);

			/* Time meter */
			CmdDrawRectanglePrecise(34, 3, g_actor[i].x, g_actor[i].y, 64);
			width = (g_actor[i].charge_timer >> 3); /* Max of 32 px */

			if (width > 0)
				CmdDrawRectanglePrecise(width, 1, g_actor[i].x + 1, g_actor[i].y + 1, 55);
		}

		else if (g_actor[i].state == ACTOR_STATE_ATTACK || g_actor[i].state == ACTOR_STATE_VICTORY)
			CmdDrawSprite(g_actor[i].persona->sprite, g_actor[i].x, g_actor[i].y, 0);
	}
}


static struct Actor* sFindTarget(struct Actor* actor)
{
	uint8_t i = 0;
	uint8_t step = 0;

	/* Find a hero for this enemy */
	if (actor->persona->tags & TAG_PERSONA_ENEMY)
	{
		i = (uint8_t)Random() % ON_SCREEN_HEROES;

		for (step = 0; step < ON_SCREEN_HEROES; step++)
		{
			if (g_actor[i].state != ACTOR_STATE_DEAD)
				return &g_actor[i];

			i = (i + 1) % ON_SCREEN_HEROES;
		}
	}

	/* Find an enemy for this hero */
	else
	{
		i = (uint8_t)Random() % (ON_SCREEN_ACTORS - ON_SCREEN_HEROES);

		for (step = 0; step < (ON_SCREEN_ACTORS - ON_SCREEN_HEROES); step++)
		{
			if (g_actor[ON_SCREEN_HEROES + i].state != ACTOR_STATE_DEAD)
				return &g_actor[ON_SCREEN_HEROES + i];

			i = (i + 1) % (ON_SCREEN_ACTORS - ON_SCREEN_HEROES);
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
	/* Chose a target */
	if ((actor->target = sFindTarget(actor)) != NULL)
	{
		actor->charge_timer = 0;
		actor->state = ACTOR_STATE_CHARGE;

		/* Chose an attack */
		if ((uint8_t)(Random() % 100) < actor->persona->actions_preference)
			actor->action = actor->persona->action_a;
		else
			actor->action = actor->persona->action_b;
	}
}


void ActorSetHold(struct Actor* actor)
{
	actor->charge_timer = 255; /* Reused */
	actor->state = ACTOR_STATE_HOLD;
}


static int sAttack(struct Actor* actor)
{
	uint8_t prev_health = 0;

	/* If our target is dead, find another */
	if (actor->target->state == ACTOR_STATE_DEAD)
	{
		if ((actor->target = sFindTarget(actor)) == NULL)
			return 1; /* No luck */
	}

	/* Apply the action */
	prev_health = actor->target->health;

	if (actor->target->state != ACTOR_STATE_HOLD)
		actor->action->callback(actor->action, actor);
	else
	{
		/* Similar to the penalty below */
		actor->target->charge_timer = (actor->target->charge_timer >> 1) + 1;
	}

	if (actor->target->health < prev_health)
	{
		/* Only set the recover time if there is real damage,
		   some actions don't hurt (but apply other effects) */
		actor->target->recover_timer = 255;

		if (actor->target->state == ACTOR_STATE_CHARGE)
		{
			/* If the target was 'charging', penalize it, this
			   to lower the game pace in a subtle way */

			/* Be caution that a value of zero requires set a new state,
			   something possible to do here, but unnecessary complex. Most
			   states changes happens on ActorLogic().

			   We avoid trigger a new state with the +1 */
			actor->target->charge_timer = (actor->target->charge_timer >> 1) + 1;
		}
	}

	if (actor->target->health == 0)
	{
		/* Well done, we killed it! */
		actor->target->state = ACTOR_STATE_DEAD;

		if ((actor->target->persona->tags & TAG_PERSONA_ENEMY))
			g_live_enemies -= 1;
		else
			g_live_heroes -= 1;

		IntPrintText("Death happens:\n - Live enemies: ");
		IntPrintNumber(g_live_enemies);
		IntPrintText(" - Live heroes: ");
		IntPrintNumber(g_live_heroes);
	}

	return 0;
}


void ActorLogic(struct Actor* actor)
{

	/* If hurt, we do nothing but wait */
	if (actor->recover_timer != 0)
	{
		/* There are cases when an actor gets hurts being in 'attack'
		state, an state that should last only for a single frame as the
		idea is to act as a signal allowing external modules intercept.
		Well, if hurt while in 'attack' state, this state now will last
		as long it takes the actor to recover.

		We avoid this going an state backward */
		if (actor->state == ACTOR_STATE_ATTACK)
			actor->state = ACTOR_STATE_CHARGE;

		/* Wait subtracting time */
		if (actor->recover_timer > actor->persona->recover_velocity)
		{
			if ((CURRENT_FRAME % 2) == 0)
			{
				if (actor->effects != EFFECT_SLOW_RECOVER)
					actor->recover_timer -= actor->persona->recover_velocity;
				else
					actor->recover_timer -= 2; /* HARDCODED */
			}

			return;
		}
		else
		{
			actor->recover_timer = 0;
			actor->effects = EFFECT_NONE;
		}
	}

	switch (actor->state)
	{
	case ACTOR_STATE_IDLE:
		if (actor->idle_timer < (255 - actor->persona->idle_velocity))
		{
			if ((CURRENT_FRAME % 2) == 0)
				actor->idle_timer += actor->persona->idle_velocity;
		}
		else
		{
			/* Check if there is something live to charge against */
			if (g_live_enemies != 0 && g_live_heroes != 0)
				sSetChargeState(actor);
			else
				actor->state = ACTOR_STATE_VICTORY;
		}
		break;

	case ACTOR_STATE_CHARGE:
		if (actor->charge_timer < (255 - actor->action->charge_velocity))
			actor->charge_timer += actor->action->charge_velocity;
		else
		{
			/* Check if there is something live to attack */
			if (g_live_enemies != 0 && g_live_heroes != 0)
				actor->state = ACTOR_STATE_ATTACK;
			else
				actor->state = ACTOR_STATE_VICTORY;
		}
		break;

	case ACTOR_STATE_ATTACK:
		/* Attack, if the attack fails means that there is no enemy left */
		if (sAttack(actor) == 0)
			sSetIdleState(actor);
		else
			actor->state = ACTOR_STATE_VICTORY;
		break;

	case ACTOR_STATE_HOLD:
		if (actor->charge_timer > 3)
		{
			if ((CURRENT_FRAME % 2) == 0)
				actor->charge_timer -= 3; /* HARDCODED */
		}
		else
		{
			if (g_live_enemies != 0 && g_live_heroes != 0)
				sSetIdleState(actor);
			else
				actor->state = ACTOR_STATE_VICTORY;
		}
		break;

	default: break;
	}
}
