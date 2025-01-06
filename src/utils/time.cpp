#include "time.h"
#include <stdexcept>

int64_t getCurrentTimestamp() {
	struct timespec tm = {};
	int ret = clock_gettime(CLOCK_REALTIME, &tm);
	if (ret != 0)
		throw std::runtime_error("clock_gettime fatal error");
	return timespecToMs(&tm);
}
