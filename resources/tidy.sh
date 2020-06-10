#!/bin/bash

files="./source/game/actor.c
       ./source/game/actor-layout.c
       ./source/game/actor-traits.c
       ./source/game/engine.c
       ./source/game/fixed.c
       ./source/game/main.c
       ./source/game/state-intro.c
       ./source/game/state-test1.c
       ./source/game/state-test2.c
       ./source/game/state-test3.c
       ./source/game/state-test4.c
       ./source/game/ui.c
       ./source/game/utilities.c"

for f in $files
do

clang-tidy -header-filter=.* -checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-* "$f" -- -std=c90 -Wall -Wextra -Wconversion -pedantic -I./source/engine-sdl/libjapan/include

done
