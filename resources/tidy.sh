#!/bin/bash

files="./source/game/utilities.c
       ./source/game/engine.c
       ./source/game/ui.c"


for f in $files
do

clang -c -std=c90 -Wall -Wextra -Wconversion -pedantic "$f" -o "${f%.*}.tmp.o"
clang-tidy -checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-* "$f" -- -std=c90 -Wall -Wextra -Wconversion -pedantic
rm "${f%.*}.tmp.o"

done
