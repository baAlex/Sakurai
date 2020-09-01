#!/bin/bash
# For reference purposes

game_files="./source/game/main.c
            ./source/game/actor.c
            ./source/game/actor-layout.c
            ./source/game/actor-traits.c
            ./source/game/engine.c
            ./source/game/fixed.c
            ./source/game/state-battle.c
            ./source/game/state-intro.c
            ./source/game/state-pause.c
            ./source/game/state-screenshots.c
            ./source/game/state-test1.c
            ./source/game/state-test2.c
            ./source/game/state-test3.c
            ./source/game/ui.c
            ./source/game/utilities.c"

engine_files="./source/engine-sdl/cache.c
              ./source/engine-sdl/draw.c
              ./source/engine-sdl/game-glue.c
              ./source/engine-sdl/jvn.c
              ./source/engine-sdl/sakurai.c
              ./source/engine-sdl/libjapan/source/buffer.c
              ./source/engine-sdl/libjapan/source/dictionary.c
              ./source/engine-sdl/libjapan/source/endianness.c
              ./source/engine-sdl/libjapan/source/list.c
              ./source/engine-sdl/libjapan/source/matrix.c
              ./source/engine-sdl/libjapan/source/status.c
              ./source/engine-sdl/libjapan/source/string.c
              ./source/engine-sdl/libjapan/source/tree.c
              ./source/engine-sdl/libjapan/source/utilities.c
              ./source/engine-sdl/libjapan/source/vector.c
              ./source/engine-sdl/libjapan/source/version.c
              ./source/engine-sdl/libjapan/source/configuration/arguments.c
              ./source/engine-sdl/libjapan/source/configuration/configuration.c
              ./source/engine-sdl/libjapan/source/configuration/file.c
              ./source/engine-sdl/libjapan/source/image/format-sgi.c
              ./source/engine-sdl/libjapan/source/image/image.c
              ./source/engine-sdl/libjapan/source/sound/format-au.c
              ./source/engine-sdl/libjapan/source/sound/format-wav.c
              ./source/engine-sdl/libjapan/source/sound/laws.c
              ./source/engine-sdl/libjapan/source/sound/sound.c
              ./source/engine-sdl/libjapan/source/token/encode-ascii.c
              ./source/engine-sdl/libjapan/source/token/encode-utf8.c
              ./source/engine-sdl/libjapan/source/token/token.c
              ./source/engine-sdl/libkansai/source/aabounding.c
              ./source/engine-sdl/libkansai/source/context-agnostic.c
              ./source/engine-sdl/libkansai/source/context-sdl2.c
              ./source/engine-sdl/libkansai/source/random.c
              ./source/engine-sdl/libkansai/source/utilities.c
              ./source/engine-sdl/libkansai/source/version.c
              ./source/engine-sdl/libkansai/source/glad/glad.c"

flags="-mtune=generic -O3 -flto -std=c11 -Wall -Wextra -Wint-conversion -Wpointer-to-int-cast"

gcc $game_files $engine_files $flags \
-I./source/engine-sdl/libjapan/include \
-I./source/engine-sdl/libkansai/include \
-I/usr/include/SDL2 -lSDL2 -lm -o "./sakurai-posix"
