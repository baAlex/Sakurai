#!/bin/bash

game_files="./source/game/actor-globals.c
            ./source/game/actor-layout.c
            ./source/game/actor-traits.c
            ./source/game/engine-dos.c
            ./source/game/actor.c
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

mkdir -p "./objects/"

ia16-elf-gcc -std=gnu90 -DSAKURAI_DOS -Wall -Wextra -mno-callee-assume-ss-data-segment \
             -Os -masm=intel -nostdlib -ffreestanding -T "./resources/com.ld" \
             $game_files -o "./objects/game.com" \
&& fasm "./source/engine-dos/sakurai.asm" "./sakurai.exe"

ia16-elf-gcc -S -std=gnu90 -DSAKURAI_DOS -Wall -Wextra -mno-callee-assume-ss-data-segment \
             -Os -masm=intel -nostdlib -ffreestanding \
             "./source/game/engine-dos.c" -o "./objects/game.asm"

# -------------------

# « -T, use script as the linker script »
# (gcc.gnu.org/onlinedocs/gcc/Link-Options.html)

# -------------------

# 1) KEEP SS AND DS SEPARATED!
# github.com/tkchia/gcc-ia16/blob/gcc-6_3_0-ia16-tkchia/gcc/config/ia16/ia16.opt
# The engine do that, you now... memory segments :( . However GCC loves do crazy
# optimizations under the erroneous assumption that SS and DS lives in the same
# place.

# -------------------

# 2) ALWAYS INITIALIZE HEAP OBJECTS!
# This is a custom executable format and by default the linker assumes ELF.
# If I'm not wrong in ELF all uninitialised things are reduced to a number
# specified in the header, allowing the OS to allocate that amount of memory.
# Anyway, there is no OS here, all heap objects needs to be appended in
# the executable.
