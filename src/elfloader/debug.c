
/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include "loader.h"

void dump1(Elf32_Ehdr *ehdr) {
	EP3_DEBUG("Elf32_Ehdr dump\n");
	EP3_DEBUG("e_ehsize: %d\n", ehdr->e_ehsize);
	EP3_DEBUG("e_entry: %d\n", ehdr->e_entry);
	EP3_DEBUG("e_flags: %d\n", ehdr->e_flags);
	EP3_DEBUG("e_ident: %s\n", ehdr->e_ident);
	EP3_DEBUG("e_machine: %d\n", ehdr->e_machine);
	EP3_DEBUG("e_phentsize: %d\n", ehdr->e_phentsize);
	EP3_DEBUG("e_phnum: %d\n", ehdr->e_phnum);
	EP3_DEBUG("e_phoff: %d\n", ehdr->e_phoff);
	EP3_DEBUG("e_shentsize: %d\n", ehdr->e_shentsize);
	EP3_DEBUG("e_shnum: %d\n", ehdr->e_shnum);
	EP3_DEBUG("e_shoff: %d\n", ehdr->e_shoff);
	EP3_DEBUG("e_shstrndx: %d\n", ehdr->e_shstrndx);
	EP3_DEBUG("e_type: %d\n", ehdr->e_type);
	EP3_DEBUG("e_version: %d\n", ehdr->e_version);
}

void dump2(Elf32_Phdr *p) {
	EP3_DEBUG("Elf32_Phdr dump\n");
	EP3_DEBUG("p_align: %d\n", p->p_align);
	EP3_DEBUG("p_filesz: %d\n", p->p_filesz);
	EP3_DEBUG("p_flags: %d\n", p->p_flags);
	EP3_DEBUG("p_memsz: %d\n", p->p_memsz);
	EP3_DEBUG("p_offset: %d\n", p->p_offset);
	EP3_DEBUG("p_paddr: %d\n", p->p_paddr);
	EP3_DEBUG("p_type: %d\n", p->p_type);
	EP3_DEBUG("p_vaddr: %d\n", p->p_vaddr);
}

void dump3(Elf32_Sym *p) {
	EP3_DEBUG("Elf32_Sym dump\n");
	EP3_DEBUG("st_name: %d\n", p->st_name);
	EP3_DEBUG("st_value: %d\n", p->st_value);
	EP3_DEBUG("st_size: %d\n", p->st_size);
	EP3_DEBUG("st_info: %d\n", p->st_info);
	EP3_DEBUG("st_other: %d\n", p->st_other);
	EP3_DEBUG("st_shndx: %d\n", p->st_shndx);
}

void dump(char *data, int sz) {
	for (int i = 0, a = 0; i < sz; i++, a++) {
		if (a > 5) {
			a = 0;
			EP3_DEBUG("\n");
		}
		EP3_DEBUG("%02x ", data[i]);
	}
}
