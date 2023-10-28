
/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include "loader.h"
#include "env.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc < 2)
		return 0;

	const char *NAME = argv[1];

	char buf[256] = { 0 };

	loader_setenv("LD_LIBRARY_PATH", "../sdk/lib/NSG/;../sdk/lib/", 1);
	printf("env: %s\n", loader_getenv("LD_LIBRARY_PATH"));

	printf("loading... %s\n", NAME);
	Elf32_Exec *ex = elfopen(NAME);
	if (ex == 0) {
		printf("Error\n");
		return 1;
	}

	void (*addr)(const char *argc, char *argv) = (void (*)(const char *argc, char *argv))elf_entry(ex);

	run_INIT_Array(ex);
	run_FINI_Array(ex);

	if (!addr) {
		printf("entry not found\n");
	} else {
		printf("entry: 0x%p\n", addr);
		fflush(stdout);
		// addr( NAME, (char*)0);
		FILE *fp = fopen("elfdump.bin", "w+");
		if (fp) {
			fwrite(ex->body, 1, ex->bin_size, fp);
			fclose(fp);
		}
	}
	
	elfclose(ex);
	return 0;
}
