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

 [actor-traits.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "actor-traits.h"
#include "utilities.h"

#define SIMPLE_ATTACK_DAMAGE 20
#define COMBINED_ATTACK_DAMAGE 60


void ActionBite(struct Actor* actor, struct Actor* target)
{
	(void)actor;
	target->health = (target->health < 10) ? 0 : (target->health - 10);
}

void ActionClaws(struct Actor* actor, struct Actor* target)
{
	(void)actor;
	target->health = (target->health < 15) ? 0 : (target->health - 15);
}

void ActionSimple(struct Actor* actor, struct Actor* target)
{
	(void)actor;
	target->health = (target->health < SIMPLE_ATTACK_DAMAGE) ? 0 : (target->health - SIMPLE_ATTACK_DAMAGE);
}

void ActionHeal(struct Actor* actor, struct Actor* target) /* Counterpart of Simple attack */
{
	(void)actor;
	target->health = (target->health > (100 - SIMPLE_ATTACK_DAMAGE)) ? 100 : (target->health + SIMPLE_ATTACK_DAMAGE);
}

void ActionCombined(struct Actor* actor, struct Actor* target)
{
	(void)actor;
	target->health = (target->health < COMBINED_ATTACK_DAMAGE) ? 0 : (target->health - COMBINED_ATTACK_DAMAGE);
}


void ActorsTraitsInitialize()
{
	struct Persona* kuro = &g_persona[PERSONA_KURO];
	struct Persona* sao = &g_persona[PERSONA_SAO];

	/* Actions */
	g_action[0].name = "Simple attack";
	g_action[0].charge_velocity = 6;
	g_action[0].callback = ActionSimple;

	g_action[1].name = "Heal";
	g_action[1].charge_velocity = 6; /* Counterpart of Simple attack */
	g_action[1].callback = ActionHeal;

	g_action[2].name = "Combined attack";
	g_action[2].charge_velocity = 3;
	g_action[2].callback = ActionCombined;

	g_action[3].name = "Bite";
	g_action[3].charge_velocity = 6;
	g_action[3].callback = ActionBite;

	g_action[4].name = "Claws";
	g_action[4].charge_velocity = 4;
	g_action[4].callback = ActionClaws;

	/* Player personalities */
	{
		kuro->name = "Kuro";
		kuro->tags = TAG_NONE | TAG_LEVITATES;

		kuro->idle_velocity = 4;
		kuro->recover_velocity = 15;
		kuro->health = 100;
	}
	{
		sao->name = "Sayori";
		sao->tags = TAG_NONE;

		sao->idle_velocity = 5;
		sao->recover_velocity = 10;
		sao->health = 100;
	}

	/* "Well balanced" enemies personalities */
	{
		g_persona[2].name = "Ferment";
		g_persona[2].tags = TAG_ENEMY;

		g_persona[2].idle_velocity = MIN(kuro->idle_velocity, sao->idle_velocity);
		g_persona[2].recover_velocity = MAX(kuro->recover_velocity, sao->recover_velocity);
		g_persona[2].health = SIMPLE_ATTACK_DAMAGE * 2;

		g_persona[2].actions_preference = 50;
		g_persona[2].action_a = &g_action[3]; /* Bite */
		g_persona[2].action_b = &g_action[3];
	}
	{
		g_persona[3].name = "Wind Eye";
		g_persona[3].tags = TAG_ENEMY | TAG_LEVITATES;

		g_persona[3].idle_velocity = MAX(kuro->idle_velocity, sao->idle_velocity);
		g_persona[3].recover_velocity = MAX(kuro->recover_velocity, sao->recover_velocity);
		g_persona[3].health = SIMPLE_ATTACK_DAMAGE * 3;

		g_persona[3].actions_preference = 70;
		g_persona[3].action_a = &g_action[3]; /* Bite */
		g_persona[3].action_b = &g_action[4]; /* Claws */
	}
}
