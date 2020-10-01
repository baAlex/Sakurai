
cmake_minimum_required(VERSION 3.12)
project("Sakurai" C)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()

add_executable("sakurai"
	"./source/engine-sdl/cache.c"
	"./source/engine-sdl/draw.c"
	"./source/engine-sdl/game-glue.c"
	"./source/engine-sdl/jvn.c"
	"./source/engine-sdl/sakurai.c"
	"./source/game/actor.c"
	"./source/game/actor-globals.c"
	"./source/game/actor-layout.c"
	"./source/game/actor-traits.c"
	"./source/game/engine-sdl.c"
	"./source/game/fixed.c"
	"./source/game/main.c"
	"./source/game/state-battle.c"
	"./source/game/state-intro.c"
	"./source/game/state-pause.c"
	"./source/game/state-screenshots.c"
	"./source/game/state-test1.c"
	"./source/game/state-test2.c"
	"./source/game/state-test3.c"
	"./source/game/state-test4.c"
	"./source/game/ui.c"
	"./source/game/utilities.c"
)

# LibJapan
option(JAPAN_SHARED OFF)
option(JAPAN_BUILD_TEST OFF)

add_subdirectory("./source/engine-sdl/libjapan")
target_link_libraries("sakurai" PRIVATE "japan-static")
target_include_directories("sakurai" PRIVATE "./source/engine-sdl/libjapan/include")

# LibKansai
option(KANSAI_SHARED OFF)
option(KANSAI_BUILD_SKETCHES OFF)

add_subdirectory("./source/engine-sdl/libkansai")
target_link_libraries("sakurai" PRIVATE "kansai-static")
target_include_directories("sakurai" PRIVATE "./source/engine-sdl/libkansai/include")
