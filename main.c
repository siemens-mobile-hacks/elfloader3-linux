
/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "loader.h"
#include "env.h"
#include "stubs.h"

void swi_stub(int swi) {
	fprintf(stderr, "Unk swi %02X func called\n", swi);
	abort();
}

void loader_subproc_impl(void *func, void *p1) {
	EP3_DEBUG("subproc called");
}

int *loader_library_impl() {
	fprintf(stderr, "loader_library_impl\n");
	return (int *) switab_functions;
}

int main(int argc, char **argv) {
	if (argc < 2)
		return 0;
	
	const char *NAME = argv[1];

	char buf[256] = { 0 };

	loader_setenv("LD_LIBRARY_PATH", "../sdk/lib/NSG/;../sdk/lib/", 1);
	printf("env: %s\n", loader_getenv("LD_LIBRARY_PATH"));
	
	loader_set_debug(1);
	
	printf("loading... %s\n", NAME);
	Elf32_Exec *ex = loader_elf_open(NAME);
	if (ex == 0) {
		printf("Error\n");
		return 1;
	}

	void (*addr)(const char *argc, char *argv) = (void (*)(const char *argc, char *argv))loader_elf_entry(ex);
	
	fprintf(stderr, "loader_run_INIT_Array\n");
	loader_run_INIT_Array(ex);
	
	fprintf(stderr, "loader_run_FINI_Array\n");
	loader_run_FINI_Array(ex);

	if (!addr) {
		printf("entry not found\n");
	} else {
		printf("entry: 0x%p\n", addr);
		fflush(stdout);
		// addr( NAME, (char*)0);
		FILE *fp = fopen("elfdump.bin", "w+");
		if (fp) {
			fwrite(ex->body->value, 1, ex->bin_size, fp);
			fclose(fp);
		}
	}
	
	loader_elf_close(ex);
	return 0;
}
