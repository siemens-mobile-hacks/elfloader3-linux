#include "../swi.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <mutex>

static std::mutex sched_dummy_lock;

void LockSched(void) {
	sched_dummy_lock.lock();
}

void UnlockSched(void) {
	sched_dummy_lock.unlock();
}

void MutexCreate(MUTEX *mtx) {
	assert(mtx);
	mtx->p = reinterpret_cast<void *>(new std::mutex);
}

void MutexDestroy(MUTEX *mtx) {
	assert(mtx != nullptr && mtx->p != nullptr);
	auto m = reinterpret_cast<std::mutex *>(mtx->p);
	delete m;
	mtx->p = nullptr;
}

void MutexLock(MUTEX *mtx) {
	assert(mtx != nullptr && mtx->p != nullptr);
	auto m = reinterpret_cast<std::mutex *>(mtx->p);
	m->lock();
}

void MutexLockEx(MUTEX *mtx, int flag) {
	assert(mtx != nullptr && mtx->p != nullptr);
	auto m = reinterpret_cast<std::mutex *>(mtx->p);
	m->lock();
}

void MutexUnlock(MUTEX *mtx) {
	assert(mtx != nullptr && mtx->p != nullptr);
	auto m = reinterpret_cast<std::mutex *>(mtx->p);
	m->unlock();
}
