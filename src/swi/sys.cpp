#include "swi.h"

#include <cstdlib>

uint16_t ram_bat_capacity = 100;

int SYS_GetFreeRamAvail(void) {
	return 16 * 1024 * 1024; // stub
}

uint16_t *SYS_RamCap() {
	return &ram_bat_capacity;
}
