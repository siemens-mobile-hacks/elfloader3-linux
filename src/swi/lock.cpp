#include "../swi.h"

#include <cstdio>
#include <cstdlib>

void SWI_LockSched(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void SWI_UnlockSched(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void SWI_MutexCreate(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void SWI_MutexDestroy(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void SWI_MutexLock(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void SWI_MutexLockEx(MUTEX *mtx, int flag) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void SWI_MutexUnlock(MUTEX *mtx) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}
