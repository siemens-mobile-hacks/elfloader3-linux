#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <swilib.h>
#include <cassert>

#include "src/elfloader/loader.h"
#include "src/swilib/switab.h"

std::recursive_mutex m_sched_mutex;
static int ram_rand_seed = 0;
static bool cpu_usage_enabled = false;
static uint64_t cpu_usage_start = 0;

void loopback0() {
	// noop
}

void loopback1() {
	// noop
}

void loopback2() {
	// noop
}

void loopback3() {
	// noop
}

void loopback4() {
	// noop
}

uint8_t GetCPUClock() {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

uint8_t GetCPULoad() {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

void loopback171() {
	// noop
}

int *RamRandSeed() {
	return &ram_rand_seed;
}

void *AddrLibrary() {
	return SWILIB;
}

void LockSched(void) {
	m_sched_mutex.lock();
}

void UnlockSched(void) {
	m_sched_mutex.unlock();
}

void SUBPROC(void *callback, ...) {
	va_list ap;
	va_start(ap, callback);
	void *p2 = va_arg(ap, void *);
	void *p1 = va_arg(ap, void *);
	va_end(ap);
	GBS_SendMessage(HELPER_CEPID, 1, p2, callback, p1);
}

void MutexCreate(MUTEX *mtx) {
	assert(mtx != nullptr);
	mtx->p = reinterpret_cast<void *>(new std::mutex);
}

void MutexDestroy(MUTEX *mtx) {
	assert(mtx != nullptr && mtx->p != nullptr);
	delete reinterpret_cast<std::mutex *>(mtx->p);
	mtx->p = nullptr;
}

void MutexLock(MUTEX *mtx) {
	assert(mtx != nullptr && mtx->p != nullptr);
	reinterpret_cast<std::mutex *>(mtx->p)->lock();
}

void MutexLockEx(MUTEX *mtx, int flag) {
	assert(mtx != nullptr && mtx->p != nullptr);
	reinterpret_cast<std::mutex *>(mtx->p)->lock();
}

void MutexUnlock(MUTEX *mtx) {
	assert(mtx != nullptr && mtx->p != nullptr);
	reinterpret_cast<std::mutex *>(mtx->p)->unlock();
}

unsigned long long getCpuUsedTime_if_ena(void) {
	if (cpu_usage_enabled)
		return GetSessionAge() - cpu_usage_start;
	return 0;
}

void StartCpuUsageCount(void) {
	if (!cpu_usage_enabled) {
		cpu_usage_enabled = true;
		cpu_usage_start = GetSessionAge();
	}
}

int isnewSGold(void) {
#if defined(ELKA)
	return 2;
#elif defined(NEWSGOLD)
	return 1;
#else
	return 0;
#endif
}

void *LIB_TOP(void) {
	return SWILIB;
}

uint32_t GetSessionAge(void) {
	return 0;
}

void SetCpuClockLow(int unk) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetCpuClockHi(int flag) {
	spdlog::debug("{}: not implemented!", __func__);
}

const char *GSM_L1_Disable() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int elfclose(void *ex) {
	return loader_elf_close((Elf32_Exec *) ex);
}

char **getBaseEnviron() {
	// deprecated?
	return NULL;
}

const char *RamLastStoreString() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

