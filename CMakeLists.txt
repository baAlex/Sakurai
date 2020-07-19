cmake_minimum_required(VERSION 3.10)

project("Sakurai" C)

if ((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_C_COMPILER_ID MATCHES "GNU"))
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -std=c11 -Wall -Wextra -Wconversion -pedantic")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -mtune=generic")
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi /D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /D_CRT_SECURE_NO_WARNINGS")
endif()

set(SOURCES
	"./source/engine-sdl/sakurai.c"
	"./source/game/actor.c"
	"./source/game/actor-layout.c"
	"./source/game/actor-traits.c"
	"./source/game/engine.c"
	"./source/game/fixed.c"
	"./source/game/main.c"
	"./source/game/state-battle.c"
	"./source/game/state-intro.c"
	"./source/game/state-pause.c"
	"./source/game/state-screenshots.c"
	"./source/game/state-test1.c"
	"./source/game/state-test2.c"
	"./source/game/state-test3.c"
	"./source/game/ui.c"
	"./source/game/utilities.c"
)

add_executable("sakurai" ${SOURCES})

# LibJapan
option(JA_BUILD_SHARED OFF)
option(JA_BUILD_TEST OFF)

add_subdirectory("./source/engine-sdl/libjapan")
target_link_libraries("sakurai" "japan-static")

target_include_directories("sakurai" PRIVATE "./source/engine-sdl/libjapan/include")

# LibKansai
option(KA_BUILD_SHARED OFF)
option(KA_BUILD_SKETCHES OFF)

add_subdirectory("./source/engine-sdl/libkansai")
target_link_libraries("sakurai" "kansai-static")

target_include_directories("kansai-static" PRIVATE "./source/engine-sdl/libjapan/include")
target_include_directories("sakurai" PRIVATE "./source/engine-sdl/libkansai/include")
