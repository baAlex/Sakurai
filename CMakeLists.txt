cmake_minimum_required(VERSION 3.10)

project("Sakurai" C)

if ((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_C_COMPILER_ID MATCHES "GNU"))
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -std=c11 -Wall -Wextra -Wconversion -pedantic -DJA_DEBUG")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -mtune=generic")
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi /DJA_DEBUG /D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /D_CRT_SECURE_NO_WARNINGS")
endif()

set(SOURCES
	"./source/engine-sdl/sakurai.c"
	"./source/engine-sdl/glad/glad.c"
)

add_executable(sakurai ${SOURCES})

# LibJapan
option(JA_BUILD_SHARED OFF)
option(JA_BUILD_TEST OFF)

add_subdirectory("./source/engine-sdl/libjapan")
target_link_libraries(sakurai "japan-static")
target_include_directories(sakurai PRIVATE "./source/engine-sdl/libjapan/include")

# SDL2
if (DEFINED VCPKG_TOOLCHAIN)
	find_package(SDL2)
	target_link_libraries(sakurai SDL2::SDL2 SDL2::SDL2main)
else()
	find_package(SDL2)
	target_link_libraries(sakurai ${SDL2_LIBRARIES})
	target_include_directories(sakurai PRIVATE ${SDL2_INCLUDE_DIR})
endif()

# Math (Linux)
if (NOT CMAKE_C_COMPILER_ID MATCHES "MSVC")
target_link_libraries(sakurai "m")
endif()

