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


void ActionAttack(struct Action* action, struct Actor* actor, struct Actor* target)
{
	(void)actor;
	target->health = (target->health < action->amount) ? 0 : (target->health - action->amount);
}

void ActionHeal(struct Action* action, struct Actor* actor, struct Actor* target)
{
	(void)actor;
	target->health = (target->health > (100 - action->amount)) ? 100 : (target->health + action->amount);
}


void TraitsInitialize()
{
	struct Persona* kuro = &g_persona[PERSONA_KURO];
	struct Persona* sao = &g_persona[PERSONA_SAO];

	/* Actions */
	g_action[0].name = "Simple attack";
	g_action[0].charge_velocity = 6;
	g_action[0].callback = ActionAttack;
	g_action[0].amount = 20;

	g_action[1].name = "Combined attack";
	g_action[1].charge_velocity = 3;
	g_action[1].callback = ActionAttack;
	g_action[1].amount = 60;

	g_action[2].name = "Heal";
	g_action[2].charge_velocity = 6;
	g_action[2].callback = ActionHeal;
	g_action[2].amount = 80;

	g_action[3].name = "Bite";
	g_action[3].charge_velocity = 6;
	g_action[3].callback = ActionAttack;
	g_action[3].amount = 10;

	g_action[4].name = "Claws";
	g_action[4].charge_velocity = 4;
	g_action[4].callback = ActionAttack;
	g_action[4].amount = 15;

	/* Heroes personalities */
	{
		kuro->name = "Kuro";
		kuro->tags = TAG_NONE | TAG_LEVITATES;

		kuro->idle_velocity = 4;
		kuro->recover_velocity = 15;
		kuro->initial_health = 100;
		kuro->initial_magic = 0;
	}
	{
		sao->name = "Sayori";
		sao->tags = TAG_NONE;

		sao->idle_velocity = 5;
		sao->recover_velocity = 10;
		sao->initial_health = 100;
		sao->initial_magic = 30;
	}

	/* "Well balanced" enemies personalities */
	{
		g_persona[2].name = "Ferment";
		g_persona[2].tags = TAG_ENEMY;

		g_persona[2].idle_velocity = MIN(kuro->idle_velocity, sao->idle_velocity);
		g_persona[2].recover_velocity = MAX(kuro->recover_velocity, sao->recover_velocity);
		g_persona[2].initial_health = 40;
		g_persona[2].initial_magic = 0;

		g_persona[2].actions_preference = 50;
		g_persona[2].action_a = &g_action[3]; /* Bite */
		g_persona[2].action_b = &g_action[3];
	}
	{
		g_persona[3].name = "Wind Eye";
		g_persona[3].tags = TAG_ENEMY | TAG_LEVITATES;

		g_persona[3].idle_velocity = MAX(kuro->idle_velocity, sao->idle_velocity);
		g_persona[3].recover_velocity = MAX(kuro->recover_velocity, sao->recover_velocity);
		g_persona[3].initial_health = 60;
		g_persona[3].initial_magic = 0;

		g_persona[3].actions_preference = 70;
		g_persona[3].action_a = &g_action[3]; /* Bite */
		g_persona[3].action_b = &g_action[4]; /* Claws */
	}

	/* Slow motion, bullet sponges */
	{
		g_persona[4].name = "Kingpin";
		g_persona[4].tags = TAG_ENEMY;

		g_persona[4].idle_velocity = MIN(kuro->idle_velocity, sao->idle_velocity) >> 1;
		g_persona[4].recover_velocity = MIN(kuro->recover_velocity, sao->recover_velocity) >> 1;
		g_persona[4].initial_health = 180;
		g_persona[4].initial_magic = 0;

		g_persona[4].actions_preference = 50;
		g_persona[4].action_a = &g_action[3]; /* Bite */
		g_persona[4].action_b = &g_action[3];
	}
	{
		g_persona[5].name = "Destroyer";
		g_persona[5].tags = TAG_ENEMY;

		g_persona[5].idle_velocity = MAX(kuro->idle_velocity, sao->idle_velocity) >> 1;
		g_persona[5].recover_velocity = MAX(kuro->recover_velocity, sao->recover_velocity) >> 1;
		g_persona[5].initial_health = 120;
		g_persona[5].initial_magic = 0;

		g_persona[5].actions_preference = 50;
		g_persona[5].action_a = &g_action[3]; /* Bite */
		g_persona[5].action_b = &g_action[3];
	}
}
