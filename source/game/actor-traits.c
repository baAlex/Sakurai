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


void ActionAddHP(struct Action* action, struct Actor* actor)
{
	if (actor->target->health > (100 - action->amount))
		actor->target->health = 100;
	else
		actor->target->health = actor->target->health + action->amount;

	if (action->magic_cost != 0)
		actor->magic = (actor->magic < action->magic_cost) ? 0 : (actor->magic - action->magic_cost);
}


void ActionSubtractHP(struct Action* action, struct Actor* actor)
{
	if (actor->target->health < action->amount)
		actor->target->health = 0;
	else
		actor->target->health = actor->target->health - action->amount;

	if (action->magic_cost != 0)
		actor->magic = (actor->magic < action->magic_cost) ? 0 : (actor->magic - action->magic_cost);
}


void ActionAddMP(struct Action* action, struct Actor* actor)
{
	if (actor->target->magic > (100 - action->amount))
		actor->target->magic = 100;
	else
		actor->target->magic = actor->target->magic + action->amount;

	if (action->magic_cost != 0)
		actor->magic = (actor->magic < action->magic_cost) ? 0 : (actor->magic - action->magic_cost);
}


void ActionSubtractMP(struct Action* action, struct Actor* actor)
{
	if (actor->target->magic < action->amount)
		actor->target->magic = 0;
	else
		actor->target->magic = actor->target->magic - action->amount;

	if (action->magic_cost != 0)
		actor->magic = (actor->magic < action->magic_cost) ? 0 : (actor->magic - action->magic_cost);
}


void ActionGeneric(struct Action* action, struct Actor* actor) /* Damage defined by the personality */
{
	(void)action;
	if (actor->target->health < actor->persona->generic_damage)
		actor->target->health = 0;
	else
		actor->target->health = actor->target->health - actor->persona->generic_damage;
}


void ActionHeal(struct Action* action, struct Actor* actor)
{
	uint8_t i = 0;
	(void)actor;

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD || (g_actor[i].persona->tags & TAG_PERSONA_ENEMY))
			continue;

		g_actor[i].health = (g_actor[i].health > (100 - action->amount)) ? 100 : (g_actor[i].health + action->amount);
	}
}


void ActionMeditate(struct Action* action, struct Actor* actor)
{
	uint8_t i = 0;
	(void)actor;

	for (i = 0; i < ON_SCREEN_ACTORS; i++)
	{
		if (g_actor[i].state == ACTOR_STATE_DEAD || (g_actor[i].persona->tags & TAG_PERSONA_ENEMY))
			continue;

		g_actor[i].magic = (g_actor[i].magic > (100 - action->amount)) ? 100 : (g_actor[i].magic + action->amount);
	}
}


void ActionShock(struct Action* action, struct Actor* actor)
{
	actor->target->effects = EFFECT_SLOW_RECOVER;
	ActionSubtractHP(action, actor); /* Except the effect, is a normal attack */
}


struct Action g_action[ACTIONS_NO];
struct Persona g_heroes[HEROES_PERSONALITIES_NO];
struct Persona g_enemies[ENEMIES_PERSONALITIES_NO];

void TraitsInitialize()
{
	/* Actions */
	g_action[ACTION_GENERIC].callback = ActionGeneric;
	g_action[ACTION_GENERIC].amount = 0; /* Not used */
	g_action[ACTION_GENERIC].charge_velocity = 4;
	g_action[ACTION_GENERIC].oscillation_velocity = 15;
	g_action[ACTION_GENERIC].magic_cost = 0;
	g_action[ACTION_GENERIC].tags = TAG_ACTION_NONE;

	g_action[ACTION_SIMPLE_KURO].callback = ActionSubtractHP;
	g_action[ACTION_SIMPLE_KURO].amount = 20;
	g_action[ACTION_SIMPLE_KURO].charge_velocity = 5;
	g_action[ACTION_SIMPLE_KURO].oscillation_velocity = 20;
	g_action[ACTION_SIMPLE_KURO].magic_cost = 0;
	g_action[ACTION_SIMPLE_KURO].tags = TAG_ACTION_NONE;

	g_action[ACTION_SIMPLE_SAO].callback = ActionSubtractHP;
	g_action[ACTION_SIMPLE_SAO].amount = 25;
	g_action[ACTION_SIMPLE_SAO].charge_velocity = 5;
	g_action[ACTION_SIMPLE_SAO].oscillation_velocity = 21;
	g_action[ACTION_SIMPLE_SAO].magic_cost = 0;
	g_action[ACTION_SIMPLE_SAO].tags = TAG_ACTION_NONE;

	g_action[ACTION_COMBINED].callback = ActionSubtractHP;
	g_action[ACTION_COMBINED].amount = 60;
	g_action[ACTION_COMBINED].charge_velocity = 4;
	g_action[ACTION_COMBINED].oscillation_velocity = 12;
	g_action[ACTION_COMBINED].magic_cost = 20;
	g_action[ACTION_COMBINED].tags = TAG_ACTION_NONE;

	g_action[ACTION_SHOCK].callback = ActionShock;
	g_action[ACTION_SHOCK].amount = 40;
	g_action[ACTION_SHOCK].charge_velocity = 4;
	g_action[ACTION_SHOCK].oscillation_velocity = 16;
	g_action[ACTION_SHOCK].magic_cost = 30;
	g_action[ACTION_SHOCK].tags = TAG_ACTION_NONE;

	g_action[ACTION_THUNDER].callback = ActionSubtractHP;
	g_action[ACTION_THUNDER].amount = 200;
	g_action[ACTION_THUNDER].charge_velocity = 2;
	g_action[ACTION_THUNDER].oscillation_velocity = 8;
	g_action[ACTION_THUNDER].magic_cost = 60;
	g_action[ACTION_THUNDER].tags = TAG_ACTION_NONE;

	g_action[ACTION_HEAL].callback = ActionHeal;
	g_action[ACTION_HEAL].amount = 50;
	g_action[ACTION_HEAL].charge_velocity = 7;
	g_action[ACTION_HEAL].oscillation_velocity = 24;
	g_action[ACTION_HEAL].magic_cost = 0;
	g_action[ACTION_HEAL].tags = TAG_ACTION_PARTY;

	g_action[ACTION_MEDITATE].callback = ActionMeditate;
	g_action[ACTION_MEDITATE].amount = 50;
	g_action[ACTION_MEDITATE].charge_velocity = 7;
	g_action[ACTION_MEDITATE].oscillation_velocity = 24;
	g_action[ACTION_MEDITATE].magic_cost = 0;
	g_action[ACTION_MEDITATE].tags = TAG_ACTION_PARTY;

	g_action[ACTION_FAIR_A].callback = ActionSubtractHP;
	g_action[ACTION_FAIR_A].amount = 20;
	g_action[ACTION_FAIR_A].charge_velocity = 4;
	g_action[ACTION_FAIR_A].oscillation_velocity = 20;
	g_action[ACTION_FAIR_A].magic_cost = 0;
	g_action[ACTION_FAIR_A].tags = TAG_ACTION_NONE;

	g_action[ACTION_FAIR_B].callback = ActionSubtractHP;
	g_action[ACTION_FAIR_B].amount = 25;
	g_action[ACTION_FAIR_B].charge_velocity = 4;
	g_action[ACTION_FAIR_B].oscillation_velocity = 16;
	g_action[ACTION_FAIR_B].magic_cost = 0;
	g_action[ACTION_FAIR_B].tags = TAG_ACTION_NONE;

	g_action[ACTION_PHIBIA].callback = ActionSubtractHP;
	g_action[ACTION_PHIBIA].amount = 5;
	g_action[ACTION_PHIBIA].charge_velocity = 6;
	g_action[ACTION_PHIBIA].oscillation_velocity = 22;
	g_action[ACTION_PHIBIA].magic_cost = 0;
	g_action[ACTION_PHIBIA].tags = TAG_ACTION_NONE;

	g_action[ACTION_DESTROYER].callback = ActionSubtractHP;
	g_action[ACTION_DESTROYER].amount = 30;
	g_action[ACTION_DESTROYER].charge_velocity = 2;
	g_action[ACTION_DESTROYER].oscillation_velocity = 10;
	g_action[ACTION_DESTROYER].magic_cost = 0;
	g_action[ACTION_DESTROYER].tags = TAG_ACTION_NONE;

	g_action[ACTION_KINGPIN].callback = ActionSubtractHP;
	g_action[ACTION_KINGPIN].amount = 40;
	g_action[ACTION_KINGPIN].charge_velocity = 1;
	g_action[ACTION_KINGPIN].oscillation_velocity = 8;
	g_action[ACTION_KINGPIN].magic_cost = 0;
	g_action[ACTION_KINGPIN].tags = TAG_ACTION_NONE;

	/* Heroes personalities */
	{
		g_heroes[PERSONALITY_KURO].name = "Kuro";
		g_heroes[PERSONALITY_KURO].sprite_filename = "assets/kuro.jvn";
		g_heroes[PERSONALITY_KURO].tags = TAG_PERSONA_NONE | TAG_PERSONA_LEVITATES;

		g_heroes[PERSONALITY_KURO].idle_velocity = 5;
		g_heroes[PERSONALITY_KURO].recover_velocity = 12;

		g_heroes[PERSONALITY_KURO].initial_health = 100;
		g_heroes[PERSONALITY_KURO].initial_magic = 0;

		g_heroes[PERSONALITY_KURO].actions_preference = 50;
		g_heroes[PERSONALITY_KURO].action_a = &g_action[ACTION_SIMPLE_KURO];
		g_heroes[PERSONALITY_KURO].action_b = &g_action[ACTION_SIMPLE_KURO];
	}
	{
		g_heroes[PERSONALITY_SAO].name = "Sayori";
		g_heroes[PERSONALITY_SAO].sprite_filename = "assets/sayori.jvn";
		g_heroes[PERSONALITY_SAO].tags = TAG_PERSONA_NONE;

		g_heroes[PERSONALITY_SAO].idle_velocity = 7;
		g_heroes[PERSONALITY_SAO].recover_velocity = 8;

		g_heroes[PERSONALITY_SAO].initial_health = 100;
		g_heroes[PERSONALITY_SAO].initial_magic = 30;

		g_heroes[PERSONALITY_SAO].actions_preference = 50;
		g_heroes[PERSONALITY_SAO].action_a = &g_action[ACTION_SIMPLE_SAO];
		g_heroes[PERSONALITY_SAO].action_b = &g_action[ACTION_SIMPLE_SAO];
	}

	/* Enemies personalities */
	{
		g_enemies[0].name = "Ferment";
		g_enemies[0].sprite_filename = "assets/ferment.jvn";
		g_enemies[0].tags = TAG_PERSONA_ENEMY;

		g_enemies[0].idle_velocity = 4;
		g_enemies[0].recover_velocity = 5;

		g_enemies[0].initial_health = 40;
		g_enemies[0].initial_magic = 0;

		g_enemies[0].generic_damage = 10;
		g_enemies[0].actions_preference = 80;
		g_enemies[0].action_a = &g_action[ACTION_FAIR_A];
		g_enemies[0].action_b = &g_action[ACTION_FAIR_B];
	}
	{
		g_enemies[1].name = "Wind Eye";
		g_enemies[1].sprite_filename = "assets/windeye.jvn";
		g_enemies[1].tags = TAG_PERSONA_ENEMY | TAG_PERSONA_LEVITATES;

		g_enemies[1].idle_velocity = 3;
		g_enemies[1].recover_velocity = 7;

		g_enemies[1].initial_health = 60;
		g_enemies[1].initial_magic = 0;

		g_enemies[1].generic_damage = 10;
		g_enemies[1].actions_preference = 50;
		g_enemies[1].action_a = &g_action[ACTION_FAIR_A];
		g_enemies[1].action_b = &g_action[ACTION_FAIR_B];
	}
	{
		g_enemies[2].name = "Kingpin";
		g_enemies[2].sprite_filename = "assets/kingpin.jvn";
		g_enemies[2].tags = TAG_PERSONA_ENEMY | TAG_PERSONA_DIFFICULT;

		g_enemies[2].idle_velocity = 2;
		g_enemies[2].recover_velocity = 8;

		g_enemies[2].initial_health = 180;
		g_enemies[2].initial_magic = 0;

		g_enemies[2].generic_damage = 10;
		g_enemies[2].actions_preference = 50;
		g_enemies[2].action_a = &g_action[ACTION_KINGPIN];
		g_enemies[2].action_b = &g_action[ACTION_KINGPIN];
	}
	{
		g_enemies[3].name = "Phibia";
		g_enemies[3].sprite_filename = "assets/phibia.jvn";
		g_enemies[3].tags = TAG_PERSONA_ENEMY | TAG_PERSONA_DIFFICULT;

		g_enemies[3].idle_velocity = 8;
		g_enemies[3].recover_velocity = 10;

		g_enemies[3].initial_health = 60;
		g_enemies[3].initial_magic = 0;

		g_enemies[3].generic_damage = 10;
		g_enemies[3].actions_preference = 50;
		g_enemies[3].action_a = &g_action[ACTION_PHIBIA];
		g_enemies[3].action_b = &g_action[ACTION_PHIBIA];
	}
	{
		g_enemies[4].name = "Destroyer";
		g_enemies[4].sprite_filename = "assets/destroyr.jvn";
		g_enemies[4].tags = TAG_PERSONA_ENEMY | TAG_PERSONA_DIFFICULT;

		g_enemies[4].idle_velocity = 2;
		g_enemies[4].recover_velocity = 5;

		g_enemies[4].initial_health = 120;
		g_enemies[4].initial_magic = 0;

		g_enemies[4].generic_damage = 10;
		g_enemies[4].actions_preference = 50;
		g_enemies[4].action_a = &g_action[ACTION_DESTROYER];
		g_enemies[4].action_b = &g_action[ACTION_DESTROYER];
	}
	{
		g_enemies[5].name = "Viridi";
		g_enemies[5].sprite_filename = "assets/viridi.jvn";
		g_enemies[5].tags = TAG_PERSONA_ENEMY | TAG_PERSONA_DIFFICULT;

		g_enemies[5].idle_velocity = 5;
		g_enemies[5].recover_velocity = 8;

		g_enemies[5].initial_health = 80;
		g_enemies[5].initial_magic = 0;

		g_enemies[5].generic_damage = 10;
		g_enemies[5].actions_preference = 50;
		g_enemies[5].action_a = &g_action[ACTION_FAIR_B];
		g_enemies[5].action_b = &g_action[ACTION_FAIR_B];
	}
	{
		g_enemies[6].name = "Ni";
		g_enemies[6].sprite_filename = "assets/ni.jvn";
		g_enemies[6].tags = TAG_PERSONA_ENEMY | TAG_PERSONA_DIFFICULT;

		g_enemies[6].idle_velocity = 5;
		g_enemies[6].recover_velocity = 7;

		g_enemies[6].initial_health = 100;
		g_enemies[6].initial_magic = 0;

		g_enemies[6].generic_damage = 30;
		g_enemies[6].actions_preference = 50;
		g_enemies[6].action_a = &g_action[ACTION_GENERIC];
		g_enemies[6].action_b = &g_action[ACTION_GENERIC];
	}
}
