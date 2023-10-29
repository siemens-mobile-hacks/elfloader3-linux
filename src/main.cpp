#include <cstdlib>
#include <cstdio>
#include <stddef.h>
#include <unistd.h>

#include "elfloader/loader.h"
#include "elfloader/env.h"
#include "elfloader/debug.h"
#include "stubs.h"

void swi_stub(int swi) {
	fprintf(stderr, "Unk swi 0x%04X (%d) func called\n", swi, swi);
	abort();
}

void loader_subproc_impl(void *func, void *p1) {
	EP3_DEBUG("subproc called");
}

int *loader_library_impl() {
	fprintf(stderr, "loader_library_impl\n");
	return (int *) switab_functions;
}

extern "C" void SWI_MutexCreate() {
	
}

extern "C" void SWI_MutexDestroy() {
	
}

extern "C" void SWI_close(int FileHandler, unsigned int *ErrorNumber) {
	fprintf(stderr, "_close(%d) %p\n", FileHandler, ErrorNumber);
}

extern "C" uint32_t SWI_strlen(const char *param) {
	return strlen(param);
}

extern "C" void *SWI_malloc(size_t size) {
	return malloc(size);
}

extern "C" void SWI_free(void *ptr) {
	free(ptr);
}

extern "C" const char *SWI_strrchr(const char *s, int c) {
	return strrchr(s, c);
}

extern "C" void *SWI_memcpy(void *dest, const void *source, size_t cnt) {
	return memcpy(dest, source, cnt);
}

extern "C" void SWI_elfclose(Elf32_Exec *ex) {
	loader_elf_close(ex);
}

static void init_switab() {
	switab_functions[0x00D] = (void *) SWI_close;
	switab_functions[0x014] = (void *) SWI_malloc;
	switab_functions[0x015] = (void *) SWI_free;
	switab_functions[0x01B] = (void *) SWI_strlen;
	switab_functions[0x117] = (void *) SWI_strrchr;
	switab_functions[0x11E] = (void *) SWI_memcpy;
	switab_functions[0x190] = (void *) SWI_MutexCreate;
	switab_functions[0x191] = (void *) SWI_MutexDestroy;
	switab_functions[0x2EE] = (void *) SWI_elfclose;
}

extern int __executable_start;

int main(int argc, char **argv) {
	if (argc < 2)
		return 0;
	
	const char *filename = argv[1];
	
	printf("Loading ELF: %s %p\n", filename, &__executable_start);
	
	loader_gdb_init();
	
	init_switab();
	
	loader_set_debug(1);
	loader_setenv("LD_LIBRARY_PATH", "../sdk/lib/NSG/;../sdk/lib/", 1);
	
	Elf32_Exec *ex = loader_elf_open(filename);
	if (!ex) {
		printf("loader_elf_open failed.\n");
		return 1;
	}
	
	auto entry = (void (*)(const char *, const char *, const void *)) loader_elf_entry(ex);
	
	fprintf(stderr, "loader_run_INIT_Array\n");
	loader_run_INIT_Array(ex);
	
	fprintf(stderr, "loader_run_FINI_Array\n");
	loader_run_FINI_Array(ex);
	
	printf("entry: 0x%p\n", entry);
	entry(filename, nullptr, nullptr);
	
	printf("wtf\n");
	loader_elf_close(ex);
	
	return 0;
}
