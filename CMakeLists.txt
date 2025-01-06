cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)

project(elfloader3 LANGUAGES C ASM CXX)

include(ExternalProject)
ExternalProject_Add(host_helper
	SOURCE_DIR			"${CMAKE_CURRENT_SOURCE_DIR}/host-helper"
	BINARY_DIR			"${CMAKE_CURRENT_SOURCE_DIR}/host-helper/build"
	INSTALL_COMMAND		""
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

set(SPDLOG_SYSTEM_INCLUDES ON)
set(SPDLOG_USE_STD_FORMAT ON)
set(ENABLE_UDS ON)
set(BUILD_SHARED OFF)
set(BUILD_STATIC ON)

add_subdirectory(third_party/openssl-0.9.5a)
add_subdirectory(third_party/libpng-1.2.5)
add_subdirectory(third_party/zlib-1.2.1)
add_subdirectory(third_party/argparse)
add_subdirectory(third_party/spdlog)

add_executable(elfloader3
	src/main.cpp
	src/glue.cpp
	src/IPC.cpp
	src/FFS.cpp
	src/Resources.cpp
	src/gui/Painter.cpp
	src/io/Loop.cpp

	# utils
	src/utils/fs.cpp
	src/utils/stb.cpp
	src/utils/time.cpp
	src/utils/string.cpp
	src/utils/charset.cpp
	src/utils/BinaryStream.cpp

	# swilib
	src/swilib/switab.c
	src/swilib/jumpers.S
	src/swilib/swihook.cpp
	src/swilib/switab-stubs.cpp

	# SWI
	src/swi/init.cpp
	src/swi/system.cpp
	src/swi/utils.cpp
	src/swi/info.cpp
	src/swi/openssl.cpp
	src/swi/const.cpp
	src/swi/libc.cpp
	src/swi/wstring.cpp
	src/swi/ws_printf.cpp
	src/swi/nucleus.cpp
	src/swi/date.cpp
	src/swi/image.cpp
	src/swi/file.cpp
	src/swi/settings.cpp
	src/swi/explorer.cpp
	src/swi/obs.cpp
	src/swi/csm.cpp
	src/swi/ll.cpp
	src/swi/gbs.cpp
	src/swi/gbs/MOPIProcessGroup.cpp

	# gui
	src/swi/gui.cpp
	src/swi/gui/font.cpp
	src/swi/gui/utils.cpp
	src/swi/gui/drwobj.cpp
	src/swi/gui/lcdlayer.cpp
	src/swi/gui/painter.cpp
	src/swi/gui/TextRender.cpp

	# gbs processes
	src/swi/proc/mmi.cpp
	src/swi/proc/helper.cpp

	# vova7890 & Ganster elfloader
	src/elfloader/debug.c
	src/elfloader/env.c
	src/elfloader/loadelf.c
	src/elfloader/loader.c
	src/elfloader/loadlib.c
	src/elfloader/log.c
)

target_compile_options(elfloader3 PUBLIC -Wall -Werror -Wno-unused-function -Wsign-compare -Wno-psabi -g)
target_compile_definitions(elfloader3 PUBLIC -DNEWSGOLD=1 -DELKA=1)
target_link_options(elfloader3 PUBLIC -Wl,--dynamic-linker=/usr/arm-linux-gnueabihf/lib/ld-linux-armhf.so.3 -Wl,-rpath=/usr/arm-linux-gnueabihf/lib/)
target_link_libraries(elfloader3 PUBLIC ssl crypto png z argparse spdlog)

target_include_directories(elfloader3 PUBLIC SYSTEM overrides)
target_include_directories(elfloader3 PUBLIC SYSTEM ../sdk/swilib/include/)
target_include_directories(elfloader3 PUBLIC SYSTEM third_party/stb)
target_include_directories(elfloader3 PRIVATE .)
target_include_directories(elfloader3 PUBLIC common)
target_link_directories(elfloader3 PUBLIC ${CMAKE_BINARY_DIR}/lib)
target_link_directories(elfloader3 PUBLIC .)
