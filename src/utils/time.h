#pragma once

#include <cmath>
#include <cstdint>
#include <ctime>

int64_t getCurrentTimestamp();

inline uint64_t ticksToMs(uint64_t ticks) {
	return ticks * 4615LL / 1000LL;
}

inline int64_t timespecToMs(struct timespec *tm) {
	return ((int64_t) tm->tv_sec * 1000) + (int64_t) round((double) tm->tv_nsec / 1000000.0);
}
