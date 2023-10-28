/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#ifndef __LOADER_H__
#define __LOADER_H__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#define __arch

extern int __ep3_debug;

#define EP3_ERROR(fmt, ...) do { fprintf(stderr, "[EP3] [error] " fmt, ## __VA_ARGS__); } while (0)
#define EP3_DEBUG(fmt, ...) do { if (__ep3_debug) { fprintf(stderr, "[EP3] [debug] " fmt, ## __VA_ARGS__); } } while (0)

#include "elf.h"

static const unsigned char elf_magic_header[] = {
	0x7f, 0x45, 0x4c, 0x46, /* 0x7f, 'E', 'L', 'F' */
	0x01, /* Only 32-bit objects. */
	0x01, /* Only LSB data. */
	0x01, /* Only ELF version 1. */
};

#define NO_FILEORDIR "no such file or directory"
#define BADFILE "bad file type"
#define OUTOFMEM "out of memory"

enum ERROR {

	E_NO_ERROR = 0x0,
	E_RELOCATION,
	E_READ,
	E_SHARED,
	E_HEADER,
	E_SECTION,
	E_RAM,
	E_EMPTY,
	E_FILE,
	E_MACHINE,
	E_ALIGN,
	E_UNDEF,
	E_SYMTAB,
	E_STRTAB,
	E_PHDR,
	E_HASTAB
};

typedef struct
{
	void *value;
} AlignedMemory;

typedef struct
{
	void *lib;
	void *next;
	void *prev;
} Global_Queue;

typedef struct
{
	void *lib;
	void *next;
} Libs_Queue;

typedef enum elf32_type {
	EXEC_NONE,
	EXEC_ELF,
	EXEC_LIB,
} Elf32_Type;

typedef struct
{
	AlignedMemory *body;
	unsigned int bin_size;
	Elf32_Ehdr ehdr;
	unsigned int v_addr;
	Elf32_Type type;
	Elf32_Word dyn[DT_FLAGS + 1];
	Elf32_Sym *symtab;
	Elf32_Rel *jmprel;
	Elf32_Word *hashtab;
	char *strtab;
	Libs_Queue *libs;
	int fp;
	char complete, __is_ex_import;
	void *meloaded;
	int *switab;
	const char *fname; // не постоянная переменная, после загрузки эльфа она обнулится
	char *temp_env; // временное переменное окружение для эльфа
	struct link_map linkmap;
} Elf32_Exec;

typedef struct
{
	char soname[64];
	Elf32_Exec *ex;
	int users_cnt;
	void *glob_queue;
} Elf32_Lib;

typedef int ELF_ENTRY(const char *, void *);
typedef int LIB_FUNC();

int loader_check_elf(Elf32_Ehdr *ehdr);
unsigned int loader_get_bin_size(Elf32_Exec *ex, Elf32_Phdr *phdrs);
int loader_load_sections(Elf32_Exec *ex);
int loader_do_reloc(Elf32_Exec *ex, Elf32_Dyn *dyn_sect, Elf32_Phdr *phdr);
unsigned long loader_elf_hash(const char *name);
Elf32_Word loader_find_export(Elf32_Exec *ex, const char *name);
Elf32_Word loader_find_function(Elf32_Lib *lib, const char *name);

void loader_set_debug(int flag);

/* shared support */
Elf32_Lib *loader_lib_open(const char *name, Elf32_Exec *ex);
int loader_lib_close(Elf32_Lib *lib, int immediate);
void loader_lib_unref_clients(Elf32_Lib *lib);

int loader_dlopen(const char *name);
int loader_dlclose(int handle);
Elf32_Word loader_dlsym(int handle, const char *name);

/* executable support */
Elf32_Exec *loader_elf_open(const char *filenam);
int loader_elf_close(Elf32_Exec *ex);
void *loader_elf_entry(Elf32_Exec *);

/* init/fini arrays support */
void loader_run_INIT_Array(Elf32_Exec *ex);
void loader_run_FINI_Array(Elf32_Exec *ex);

/* user-defined impl */
void loader_subproc_impl(void *func, void *p1);
int *loader_library_impl();

#endif
