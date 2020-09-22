#!/bin/bash

game_files="./source/game/engine-dos.c
            ./source/game/state-test1.c
            ./source/game/main.c
            ./source/game/utilities.c"

mkdir -p "./objects/"

ia16-elf-gcc -DSAKURAI_DOS -Wall -Wextra -mno-callee-assume-ss-data-segment \
             -Os -masm=intel -nostdlib -ffreestanding -T "./resources/com.ld" \
             $game_files -o "./objects/game.com" \
&& fasm "./source/engine-dos/sakurai.asm" "./sakurai.exe"

# -------------------

# « -T, use script as the linker script »
# (gcc.gnu.org/onlinedocs/gcc/Link-Options.html)

# github.com/tkchia/gcc-ia16/blob/gcc-6_3_0-ia16-tkchia/gcc/config/ia16/ia16.opt
# Keep SS and DS separated!
