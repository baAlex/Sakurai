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
	"./source/game/state-intro.c"
	"./source/game/state-test1.c"
	"./source/game/state-test2.c"
	"./source/game/state-test3.c"
	"./source/game/state-test4.c"
	"./source/game/ui.c"
	"./source/game/utilities.c"
)

add_executable("sakurai" ${SOURCES})

# Math (Linux)
if (NOT CMAKE_C_COMPILER_ID MATCHES "MSVC")
target_link_libraries("sakurai" "m")
endif()

