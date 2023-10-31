cmake_minimum_required(VERSION 3.15)
project(elfloader3 LANGUAGES C ASM CXX)

add_subdirectory(host-helper)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_COMPILER		arm-linux-gnueabihf-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_COMPILER		arm-linux-gnueabihf-g++${CMAKE_EXECUTABLE_SUFFIX})

# add_compile_options(-mcpu=arm926ej-s -g -mfloat-abi=soft -fno-common -ffunction-sections -fdata-sections)
add_link_options(-static)

add_executable(elfloader3
	src/main.cpp
	src/stubs.cpp
	src/utils.cpp
	src/SieFs.cpp
	src/charset.cpp
	
	# SWI
	src/swi.cpp
	src/swi/libc.cpp
	src/swi/fs.cpp
	src/swi/time.cpp
	src/swi/lock.cpp
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
target_link_libraries(elfloader3 -lc -lgcc)
