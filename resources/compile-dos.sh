#!/bin/bash

game_files="./source/game/actor.c
            ./source/game/actor-layout.c
            ./source/game/actor-traits.c
            ./source/game/engine.c
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

mkdir -p "./objects"

bcc -0 -O -ansi -Md -x -i -o "./objects/game.com" $game_files -I./source/game/
fasm "./source/engine-dos/sakurai.asm" "./sakurai.exe"
