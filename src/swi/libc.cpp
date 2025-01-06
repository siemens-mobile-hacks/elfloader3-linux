#include <spdlog/spdlog.h>
#include <stdarg.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csetjmp>
#include <swilib/libc.h>
#include <swilib/system.h>
#include "src/elfloader/loader.h"

void _longjmp(jmp_buf env, int val) {
	spdlog::debug("{}: not implemented!", __func__);
	abort();
}

int _dlopen(const char *name) {
	return loader_dlopen(name);
}

void *_dlsym(int handle, const char *name) {
	return (void *) loader_dlsym(handle, name);
}

int _dlclose(int handle) {
	return loader_dlclose(handle);
}

const char *_dlerror() {
	return loader_dlerror();
}

void zeromem(void *ptr, int num) {
	memset(ptr, 0, num);
}
