cmake_minimum_required(VERSION 3.15)

include(${CMAKE_CURRENT_BINARY_DIR}/toolchain.cmake)

project(elfloader3 LANGUAGES C ASM CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_executable(elfloader3
	src/main.cpp
	src/stb.cpp
	src/stubs.cpp
	src/utils.cpp
	src/charset.cpp
	src/IPC.cpp
	src/SieFs.cpp
	src/Loop.cpp
	
	src/gui/Theme.cpp
	src/gui/Painter.cpp
	
	# SWI
	src/swi.cpp
	src/swi/libc.cpp
	src/swi/fs.cpp
	src/swi/time.cpp
	src/swi/ll.cpp
	src/swi/lock.cpp
	src/swi/gui.cpp
	src/swi/csm.cpp
	src/swi/obs.cpp
	src/swi/images.cpp
	src/swi/settings.cpp
	src/swi/explorer.cpp
	src/swi/helper.cpp
	src/swi/gbs.cpp
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

target_compile_options(elfloader3 PUBLIC -Wno-narrowing -g)
target_link_options(elfloader3 PUBLIC -static)

target_include_directories(elfloader3 PUBLIC third_party/stb)
target_include_directories(elfloader3 SYSTEM PUBLIC src)
target_include_directories(elfloader3 PUBLIC common)
target_link_directories(elfloader3 PUBLIC .)
