cmake_minimum_required(VERSION 3.15)

include(${CMAKE_CURRENT_BINARY_DIR}/toolchain.cmake)

project(libuv LANGUAGES C ASM CXX)

add_library(uv STATIC
	third_party/libuv/src/fs-poll.c
	third_party/libuv/src/idna.c
	third_party/libuv/src/inet.c
	third_party/libuv/src/random.c
	third_party/libuv/src/strscpy.c
	third_party/libuv/src/thread-common.c
	third_party/libuv/src/threadpool.c
	third_party/libuv/src/timer.c
	third_party/libuv/src/uv-data-getter-setters.c
	third_party/libuv/src/uv-common.c
	third_party/libuv/src/version.c
	third_party/libuv/src/strtok.c

	third_party/libuv/src/unix/async.c
	third_party/libuv/src/unix/core.c
	third_party/libuv/src/unix/dl.c
	third_party/libuv/src/unix/fs.c
	third_party/libuv/src/unix/getaddrinfo.c
	third_party/libuv/src/unix/getnameinfo.c
	third_party/libuv/src/unix/loop-watcher.c
	third_party/libuv/src/unix/loop.c
	third_party/libuv/src/unix/pipe.c
	third_party/libuv/src/unix/poll.c
	third_party/libuv/src/unix/process.c
	third_party/libuv/src/unix/random-devurandom.c
	third_party/libuv/src/unix/signal.c
	third_party/libuv/src/unix/stream.c
	third_party/libuv/src/unix/tcp.c
	third_party/libuv/src/unix/thread.c
	third_party/libuv/src/unix/tty.c
	third_party/libuv/src/unix/udp.c

	third_party/libuv/src/unix/linux.c
	third_party/libuv/src/unix/procfs-exepath.c
	third_party/libuv/src/unix/proctitle.c
	third_party/libuv/src/unix/random-getrandom.c
	third_party/libuv/src/unix/random-sysctl-linux.c
)

target_compile_options(uv PUBLIC -Wall -D_GNU_SOURCE)
target_include_directories(uv PUBLIC third_party/libuv/include)
target_include_directories(uv SYSTEM PUBLIC third_party/libuv/src)
