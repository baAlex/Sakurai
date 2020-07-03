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


void ActionSubtractHP(struct Action* action, struct Actor* actor)
{
	struct Actor* target = actor->target;
	target->health = (target->health < action->amount) ? 0 : (target->health - action->amount);
}


void TraitsInitialize()
{
	/* Actions */
	g_action[0].name = "Dummy";
	g_action[0].charge_velocity = 4;
	g_action[0].oscillation_velocity = 20;
	g_action[0].callback = ActionSubtractHP;
	g_action[0].amount = 20;

	/* Heroes personalities */
	{
		g_heroes[PERSONALITY_KURO].name = "Kuro";
		g_heroes[PERSONALITY_KURO].sprite_filename = "assets/kuro.jvn";
		g_heroes[PERSONALITY_KURO].tags = TAG_NONE | TAG_LEVITATES;

		g_heroes[PERSONALITY_KURO].idle_velocity = 5;
		g_heroes[PERSONALITY_KURO].recover_velocity = 12;

		g_heroes[PERSONALITY_KURO].initial_health = 100;
		g_heroes[PERSONALITY_KURO].initial_magic = 0;

		g_heroes[PERSONALITY_KURO].actions_preference = 50;
		g_heroes[PERSONALITY_KURO].action_a = &g_action[0];
		g_heroes[PERSONALITY_KURO].action_b = &g_action[0];
	}
	{
		g_heroes[PERSONALITY_SAO].name = "Sayori";
		g_heroes[PERSONALITY_SAO].sprite_filename = "assets/sayori.jvn";
		g_heroes[PERSONALITY_SAO].tags = TAG_NONE;

		g_heroes[PERSONALITY_SAO].idle_velocity = 7;
		g_heroes[PERSONALITY_SAO].recover_velocity = 8;

		g_heroes[PERSONALITY_SAO].initial_health = 100;
		g_heroes[PERSONALITY_SAO].initial_magic = 30;

		g_heroes[PERSONALITY_SAO].actions_preference = 50;
		g_heroes[PERSONALITY_SAO].action_a = &g_action[0];
		g_heroes[PERSONALITY_SAO].action_b = &g_action[0];
	}

	/* Enemies personalities */
	{
		g_enemies[0].name = "Ferment";
		g_enemies[0].sprite_filename = "assets/ferment.jvn";
		g_enemies[0].tags = TAG_ENEMY;

		g_enemies[0].idle_velocity = 4;
		g_enemies[0].recover_velocity = 5;

		g_enemies[0].initial_health = 40;
		g_enemies[0].initial_magic = 0;

		g_enemies[0].actions_preference = 50;
		g_enemies[0].action_a = &g_action[0];
		g_enemies[0].action_b = &g_action[0];
	}
	{
		g_enemies[1].name = "Wind Eye";
		g_enemies[1].sprite_filename = "assets/windeye.jvn";
		g_enemies[1].tags = TAG_ENEMY | TAG_LEVITATES;

		g_enemies[1].idle_velocity = 3;
		g_enemies[1].recover_velocity = 7;

		g_enemies[1].initial_health = 60;
		g_enemies[1].initial_magic = 0;

		g_enemies[1].actions_preference = 50;
		g_enemies[1].action_a = &g_action[0];
		g_enemies[1].action_b = &g_action[0];
	}

	{
		g_enemies[2].name = "Kingpin";
		g_enemies[2].sprite_filename = "assets/kingpin.jvn";
		g_enemies[2].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[2].idle_velocity = 1;
		g_enemies[2].recover_velocity = 3;

		g_enemies[2].initial_health = 180;
		g_enemies[2].initial_magic = 0;

		g_enemies[2].actions_preference = 50;
		g_enemies[2].action_a = &g_action[0];
		g_enemies[2].action_b = &g_action[0];
	}

	{
		g_enemies[3].name = "Phibia";
		g_enemies[3].sprite_filename = "assets/phibia.jvn";
		g_enemies[3].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[3].idle_velocity = 8;
		g_enemies[3].recover_velocity = 11;

		g_enemies[3].initial_health = 60;
		g_enemies[3].initial_magic = 0;

		g_enemies[3].actions_preference = 50;
		g_enemies[3].action_a = &g_action[0];
		g_enemies[3].action_b = &g_action[0];
	}

	{
		g_enemies[4].name = "Destroyer";
		g_enemies[4].sprite_filename = "assets/destroyr.jvn";
		g_enemies[4].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[4].idle_velocity = 2;
		g_enemies[4].recover_velocity = 4;

		g_enemies[4].initial_health = 120;
		g_enemies[4].initial_magic = 0;

		g_enemies[4].actions_preference = 50;
		g_enemies[4].action_a = &g_action[0];
		g_enemies[4].action_b = &g_action[0];
	}

	{
		g_enemies[5].name = "Viridi";
		g_enemies[5].sprite_filename = "assets/viridi.jvn";
		g_enemies[5].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[5].idle_velocity = 6;
		g_enemies[5].recover_velocity = 8;

		g_enemies[5].initial_health = 80;
		g_enemies[5].initial_magic = 0;

		g_enemies[5].actions_preference = 50;
		g_enemies[5].action_a = &g_action[0];
		g_enemies[5].action_b = &g_action[0];
	}

	{
		g_enemies[6].name = "Ni";
		g_enemies[6].sprite_filename = "assets/ni.jvn";
		g_enemies[6].tags = TAG_ENEMY | TAG_DIFFICULT;

		g_enemies[6].idle_velocity = 5;
		g_enemies[6].recover_velocity = 7;

		g_enemies[6].initial_health = 100;
		g_enemies[6].initial_magic = 0;

		g_enemies[6].actions_preference = 50;
		g_enemies[6].action_a = &g_action[0];
		g_enemies[6].action_b = &g_action[0];
	}
}
