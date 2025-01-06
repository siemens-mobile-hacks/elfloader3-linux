#include <cstdarg>
#include <spdlog/spdlog.h>
#include <swilib/system.h>

#include "src/elfloader/loader.h"
#include "src/swilib/switab.h"

extern "C" void loader_swi_stub(int swi) {
	spdlog::error("Unk swi 0x{:04X} ({}) func called", swi, swi);
	abort();
}

extern "C" void loader_subproc_impl(void *func, void *p1) {
	SUBPROC(func, (int) p1);
}

extern "C" int *loader_library_impl() {
	return (int *) SWILIB;
}

extern "C" void loader_log_error(const char *format, ...) {
	char tmp[4096];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(tmp, sizeof(tmp), format, ap);
	va_end(ap);

	assert(len >= 0);

	if (tmp[len - 1] == '\n')
		tmp[len - 1] = 0;

	spdlog::error("[loader] {}", tmp);
}

extern "C" void loader_log_debug(const char *format, ...) {
	char tmp[4096];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(tmp, sizeof(tmp), format, ap);
	va_end(ap);

	assert(len >= 0);

	if (tmp[len - 1] == '\n')
		tmp[len - 1] = 0;

	// spdlog::debug("[loader] {}", tmp);
}
