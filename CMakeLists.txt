cmake_minimum_required(VERSION 3.15)

include(./toolchain.cmake)

project(elfloader3 LANGUAGES C ASM CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wno-narrowing -g)
add_link_options(-static)

# find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
# find_library(SDL2_LIBRARY NAME SDL2)

add_executable(elfloader3
	src/main.cpp
	src/stubs.cpp
	src/utils.cpp
	src/charset.cpp
	src/SieFs.cpp
	
	src/gui/Theme.cpp
	src/gui/Painter.cpp
	src/gui/MaskPainter.cpp
	
	# SWI
	src/swi.cpp
	src/swi/libc.cpp
	src/swi/fs.cpp
	src/swi/time.cpp
	src/swi/ll.cpp
	src/swi/lock.cpp
	src/swi/gui.cpp
	src/swi/csm.cpp
	src/swi/sys.cpp
	src/swi/ws.cpp
	src/swi/ws_printf.cpp
	
	# vova7890 & Ganster elfloader
	src/elfloader/debug.c
	src/elfloader/env.c
	src/elfloader/loadelf.c
	src/elfloader/loader.c
	src/elfloader/loadlib.c
	src/elfloader/log.c
)

target_include_directories(elfloader3 PUBLIC third_party/micro-gl/include)
target_include_directories(elfloader3 SYSTEM PUBLIC src)

# target_include_directories(elfloader3 PUBLIC ${SDL2_INCLUDE_DIR})
# target_link_libraries(elfloader3 ${SDL2_LIBRARY})
