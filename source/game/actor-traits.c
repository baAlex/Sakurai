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


void ActionAttack(struct Action* action, struct Actor* actor)
{
	struct Actor* target = actor->target;
	target->health = (target->health < action->amount) ? 0 : (target->health - action->amount);
}

void ActionHeal(struct Action* action, struct Actor* actor)
{
	struct Actor* target = actor->target;
	target->health = (target->health > (100 - action->amount)) ? 100 : (target->health + action->amount);
}


void TraitsInitialize()
{
	/* Actions */
	g_action[0].name = "Simple attack";
	g_action[0].charge_velocity = 5;
	g_action[0].oscillation_velocity = 20;
	g_action[0].callback = ActionAttack;
	g_action[0].amount = 20;

	g_action[1].name = "Combined attack";
	g_action[1].charge_velocity = 3;
	g_action[1].oscillation_velocity = 10;
	g_action[1].callback = ActionAttack;
	g_action[1].amount = 60;

	g_action[2].name = "Heal";
	g_action[2].charge_velocity = 6;
	g_action[2].oscillation_velocity = 40;
	g_action[2].callback = ActionHeal;
	g_action[2].amount = 80;

	g_action[3].name = "Bite";
	g_action[3].charge_velocity = 5;
	g_action[3].oscillation_velocity = 20;
	g_action[3].callback = ActionAttack;
	g_action[3].amount = 10;

	g_action[4].name = "Claws";
	g_action[4].charge_velocity = 4;
	g_action[4].oscillation_velocity = 15;
	g_action[4].callback = ActionAttack;
	g_action[4].amount = 15;

	/* Heroes personalities */
	{
		g_heroes[HERO_KURO].name = "Kuro";
		g_heroes[HERO_KURO].sprite_filename = "assets\\kuro.jvn";
		g_heroes[HERO_KURO].tags = TAG_NONE | TAG_LEVITATES;

		g_heroes[HERO_KURO].idle_velocity = 5;
		g_heroes[HERO_KURO].recover_velocity = 13;

		g_heroes[HERO_KURO].initial_health = 100;
		g_heroes[HERO_KURO].initial_magic = 0;

		g_heroes[HERO_KURO].actions_preference = 50;
		g_heroes[HERO_KURO].action_a = &g_action[1]; /* Combined attack */
		g_heroes[HERO_KURO].action_b = &g_action[1];
	}
	{
		g_heroes[HERO_SAO].name = "Sayori";
		g_heroes[HERO_SAO].sprite_filename = "assets\\sayori.jvn";
		g_heroes[HERO_SAO].tags = TAG_NONE;

		g_heroes[HERO_SAO].idle_velocity = 7;
		g_heroes[HERO_SAO].recover_velocity = 9;

		g_heroes[HERO_SAO].initial_health = 100;
		g_heroes[HERO_SAO].initial_magic = 30;

		g_heroes[HERO_SAO].actions_preference = 50;
		g_heroes[HERO_SAO].action_a = &g_action[0]; /* Simple attack */
		g_heroes[HERO_SAO].action_b = &g_action[0];
	}

	/* "Well balanced" enemies personalities */
	{
		g_enemies[0].name = "Ferment";
		g_enemies[0].sprite_filename = "assets\\enemy-a.jvn";
		g_enemies[0].tags = TAG_ENEMY;

		g_enemies[0].idle_velocity = 4;
		g_enemies[0].recover_velocity = 5;

		g_enemies[0].initial_health = 40;
		g_enemies[0].initial_magic = 0;

		g_enemies[0].actions_preference = 50;
		g_enemies[0].action_a = &g_action[3]; /* Bite */
		g_enemies[0].action_b = &g_action[3];
	}
	{
		g_enemies[1].name = "Wind Eye";
		g_enemies[1].sprite_filename = "assets\\enemy-b.jvn";
		g_enemies[1].tags = TAG_ENEMY | TAG_LEVITATES;

		g_enemies[1].idle_velocity = 3;
		g_enemies[1].recover_velocity = 7;

		g_enemies[1].initial_health = 60;
		g_enemies[1].initial_magic = 0;

		g_enemies[1].actions_preference = 70;
		g_enemies[1].action_a = &g_action[3]; /* Bite */
		g_enemies[1].action_b = &g_action[3];
	}

	/* Slow motion, bullet sponges */
	{
		g_enemies[2].name = "Kingpin";
		g_enemies[2].sprite_filename = "assets\\enemy-c.jvn";
		g_enemies[2].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[2].idle_velocity = 1;
		g_enemies[2].recover_velocity = 3;

		g_enemies[2].initial_health = 180;
		g_enemies[2].initial_magic = 0;

		g_enemies[2].actions_preference = 50;
		g_enemies[2].action_a = &g_action[3]; /* Bite */
		g_enemies[2].action_b = &g_action[3];
	}
	{
		g_enemies[3].name = "Destroyer";
		g_enemies[3].sprite_filename = "assets\\enemy-d.jvn";
		g_enemies[3].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[3].idle_velocity = MAX(g_heroes[HERO_KURO].idle_velocity, g_heroes[HERO_SAO].idle_velocity) >> 1;
		g_enemies[3].recover_velocity =
		    MAX(g_heroes[HERO_KURO].recover_velocity, g_heroes[HERO_SAO].recover_velocity) >> 1;

		g_enemies[3].initial_health = 120;
		g_enemies[3].initial_magic = 0;

		g_enemies[3].actions_preference = 50;
		g_enemies[3].action_a = &g_action[3]; /* Bite */
		g_enemies[3].action_b = &g_action[3];
	}

	/* Fast and delicate one */
	{
		g_enemies[4].name = "Phibia";
		g_enemies[4].sprite_filename = "assets\\enemy-e.jvn";
		g_enemies[4].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[4].idle_velocity = MAX(g_heroes[HERO_KURO].idle_velocity, g_heroes[HERO_SAO].idle_velocity);
		g_enemies[4].recover_velocity = MAX(g_heroes[HERO_KURO].recover_velocity, g_heroes[HERO_SAO].recover_velocity);

		g_enemies[4].initial_health = 50;
		g_enemies[4].initial_magic = 0;

		g_enemies[4].actions_preference = 50;
		g_enemies[4].action_a = &g_action[3]; /* Bite */
		g_enemies[4].action_b = &g_action[3];
	}

	/* Who knows */
	{
		g_enemies[5].name = "Viridi";
		g_enemies[5].sprite_filename = "assets\\enemy-f.jvn";
		g_enemies[5].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[5].idle_velocity = MIN(g_heroes[HERO_KURO].idle_velocity, g_heroes[HERO_SAO].idle_velocity) >> 1;
		g_enemies[5].recover_velocity =
		    MIN(g_heroes[HERO_KURO].recover_velocity, g_heroes[HERO_SAO].recover_velocity) >> 1;

		g_enemies[5].initial_health = 180;
		g_enemies[5].initial_magic = 0;

		g_enemies[5].actions_preference = 50;
		g_enemies[5].action_a = &g_action[3]; /* Bite */
		g_enemies[5].action_b = &g_action[3];
	}
	{
		g_enemies[6].name = "Ni";
		g_enemies[6].sprite_filename = "assets\\enemy-g.jvn";
		g_enemies[6].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[6].idle_velocity = MAX(g_heroes[HERO_KURO].idle_velocity, g_heroes[HERO_SAO].idle_velocity) >> 1;
		g_enemies[6].recover_velocity =
		    MAX(g_heroes[HERO_KURO].recover_velocity, g_heroes[HERO_SAO].recover_velocity) >> 1;

		g_enemies[6].initial_health = 120;
		g_enemies[6].initial_magic = 0;

		g_enemies[6].actions_preference = 50;
		g_enemies[6].action_a = &g_action[3]; /* Bite */
		g_enemies[6].action_b = &g_action[3];
	}
}
