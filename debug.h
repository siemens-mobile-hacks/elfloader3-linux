#pragma once

#include "elf.h"

/* magic shared structures that GDB knows about */
struct link_map {
	uintptr_t l_addr;
	char *l_name;
	uintptr_t l_ld;
	struct link_map *l_next;
	struct link_map *l_prev;
};

/* needed for dl_iterate_phdr to be passed to the callbacks provided */
struct dl_phdr_info {
	Elf32_Addr dlpi_addr;
	const char *dlpi_name;
	const Elf32_Phdr *dlpi_phdr;
	Elf32_Half dlpi_phnum;
};

// Values for r_debug->state
enum {
	RT_CONSISTENT,
	RT_ADD,
	RT_DELETE
};

struct r_debug {
	int32_t r_version;
	struct link_map * r_map;
	void (*r_brk)(void);
	int32_t r_state;
	uintptr_t r_ldbase;
};

typedef struct soinfo soinfo;

#define FLAG_LINKED     0x00000001
#define FLAG_ERROR      0x00000002
#define FLAG_EXE        0x00000004 // The main executable
#define FLAG_PRELINKED  0x00000008 // This is a pre-linked lib
#define SOINFO_NAME_LEN 128

struct soinfo {
	const char name[SOINFO_NAME_LEN];
	Elf32_Phdr *phdr;
	int phnum;
	unsigned entry;
	unsigned base;
	unsigned size;
	// buddy-allocator index, negative for prelinked libraries
	int ba_index;
	unsigned *dynamic;
	unsigned wrprotect_start;
	unsigned wrprotect_end;
	soinfo *next;
	unsigned flags;
	const char *strtab;
	Elf32_Sym *symtab;
	unsigned nbucket;
	unsigned nchain;
	unsigned *bucket;
	unsigned *chain;
	unsigned *plt_got;
	Elf32_Rel *plt_rel;
	unsigned plt_rel_count;
	Elf32_Rel *rel;
	unsigned rel_count;
	unsigned *preinit_array;
	unsigned preinit_array_count;
	unsigned *init_array;
	unsigned init_array_count;
	unsigned *fini_array;
	unsigned fini_array_count;
	void (*init_func)(void);
	void (*fini_func)(void);
#ifdef ANDROID_ARM_LINKER
	/* ARM EABI section used for stack unwinding. */
	unsigned *ARM_exidx;
	unsigned ARM_exidx_count;
#endif
	unsigned refcount;
	struct link_map linkmap;
};

extern soinfo libdl_info;
