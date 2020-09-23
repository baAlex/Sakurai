/*-----------------------------

 [actor-globals.c]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ACTOR_GLOBALS_H
#define ACTOR_GLOBALS_H

#include "actor-traits.h"
#include "actor.h"

struct Action g_action[ACTIONS_NO] = {0};
struct Persona g_heroes[HEROES_PERSONALITIES_NO] = {0};
struct Persona g_enemies[ENEMIES_PERSONALITIES_NO] = {0};

uint8_t g_live_heroes = {0};
uint8_t g_live_enemies = {0};
struct Actor g_actor[ON_SCREEN_ACTORS] = {0};

#endif
