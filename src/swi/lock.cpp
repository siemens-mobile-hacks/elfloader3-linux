#include "../swi.h"

#include <cstdio>
#include <cstdlib>

void LockSched(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void UnlockSched(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void MutexCreate(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void MutexDestroy(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void MutexLock(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void MutexLockEx(MUTEX *mtx, int flag) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void MutexUnlock(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}
