#!/bin/bash

files="./source/game/actor.c
       ./source/game/actor-traits.c
       ./source/game/engine.c
       ./source/game/main.c
       ./source/game/state-test1.c
       ./source/game/state-test2.c
       ./source/game/state-test3.c
       ./source/game/ui.c
       ./source/game/utilities.c"


for f in $files
do

clang -c -std=c90 -Wall -Wextra -Wconversion -pedantic "$f" -o "${f%.*}.tmp.o"
clang-tidy -header-filter=.* -checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-* "$f" -- -std=c90 -Wall -Wextra -Wconversion -pedantic
rm "${f%.*}.tmp.o"

done
