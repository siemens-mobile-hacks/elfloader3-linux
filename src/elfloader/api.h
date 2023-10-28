#pragma once

#include "elf.h"
#include "env.h"

typedef struct Elf32_Exec Elf32_Exec;

Elf32_Exec *loader_elf_open(const char *filename);
int loader_elf_close(Elf32_Exec *ex);
void *loader_elf_entry(Elf32_Exec *ex);

void loader_subproc(void *func, void *p1, void *p2);
int *loader_library();
