/*-----------------------------

 [game-commands.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef GAME_COMMANDS_H
#define GAME_COMMANDS_H

#include "japan-image.h"

void IterateGameCommands(void* cmd, size_t max_commands, const struct jaImage* background, struct jaImage* out);

#endif
