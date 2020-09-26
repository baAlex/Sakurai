#!/bin/bash

game_files="./source/game/actor-globals.c
            ./source/game/actor-layout.c
            ./source/game/actor-traits.c
            ./source/game/actor.c
            ./source/game/engine-sdl.c
            ./source/game/fixed.c
            ./source/game/main.c
            ./source/game/state-battle.c
            ./source/game/state-intro.c
            ./source/game/state-pause.c
            ./source/game/state-screenshots.c
            ./source/game/state-test1.c
            ./source/game/state-test2.c
            ./source/game/state-test3.c
            ./source/game/state-test4.c
            ./source/game/ui.c
            ./source/game/utilities.c"

for f in $game_files
do
echo $f
clang-tidy -header-filter=.* -checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-* "$f" -- -std=c90 -Wall -Wextra -Wconversion -pedantic
done
