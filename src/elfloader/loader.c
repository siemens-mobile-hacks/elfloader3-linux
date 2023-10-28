/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include "loader.h"
#include "debug.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

extern void __attribute__((noinline)) rtld_db_dlactivity(void);
static struct r_debug _r_debug = {1, NULL, &rtld_db_dlactivity, RT_CONSISTENT, 0};
static struct link_map *r_debug_tail = NULL;

int __ep3_debug = 0;
int loader_warnings = 1;
int realtime_libclean = 1;

/*
 * Debug
 * */
static void insert_soinfo_into_debug_map(Elf32_Exec *ex) {
	struct link_map * map;
	/* Copy the necessary fields into the debug structure. */
	map = &(ex->linkmap);
	map->l_addr = ex->v_addr;
	map->l_name = (char *) ex->fname;
	/* Stick the new library at the end of the list.
	 * gdb tends to care more about libc than it does
	 * about leaf libraries, and ordering it this way
	 * reduces the back-and-forth over the wire.
	 */
	if (r_debug_tail) {
		r_debug_tail->l_next = map;
		map->l_prev = r_debug_tail;
		map->l_next = 0;
	} else {
		_r_debug.r_map = map;
		map->l_prev = 0;
		map->l_next = 0;
	}
	r_debug_tail = map;
}

static void remove_soinfo_from_debug_map(Elf32_Exec *ex) {
	struct link_map *map = &(ex->linkmap);
	if (r_debug_tail == map)
		r_debug_tail = map->l_prev;
	if (map->l_prev) map->l_prev->l_next = map->l_next;
	if (map->l_next) map->l_next->l_prev = map->l_prev;
}

static void notify_gdb_of_load(Elf32_Exec *ex) {
	_r_debug.r_state = RT_ADD;
	rtld_db_dlactivity();
	insert_soinfo_into_debug_map(ex);
	_r_debug.r_state = RT_CONSISTENT;
	rtld_db_dlactivity();
}

static void notify_gdb_of_unload(Elf32_Exec *ex) {
	_r_debug.r_state = RT_DELETE;
	rtld_db_dlactivity();
	remove_soinfo_from_debug_map(ex);
	_r_debug.r_state = RT_CONSISTENT;
	rtld_db_dlactivity();
}

static void notify_gdb_of_libraries() {
	_r_debug.r_state = RT_ADD;
	rtld_db_dlactivity();
	_r_debug.r_state = RT_CONSISTENT;
	rtld_db_dlactivity();
}

/*
 * Loader
 * */

void loader_set_debug(int flag) {
	__ep3_debug = flag;
}

// Проверка валидности эльфа
int loader_check_elf(Elf32_Ehdr *ehdr) {
	if (memcmp(ehdr, elf_magic_header, sizeof(elf_magic_header)))
		return E_HEADER;
	if (ehdr->e_machine != EM_ARM)
		return E_MACHINE;

	return E_NO_ERROR;
}

// Получение нужного размера в раме
unsigned int loader_get_bin_size(Elf32_Exec *ex, Elf32_Phdr *phdrs) {
	unsigned int i = 0;
	unsigned long maxadr = 0;
	unsigned int end_adr;

	while (i < ex->ehdr.e_phnum) {
		Elf32_Phdr phdr = phdrs[i];

		if (phdr.p_type == PT_LOAD) {
			if (ex->v_addr > phdr.p_vaddr)
				ex->v_addr = phdr.p_vaddr;
			end_adr = phdr.p_vaddr + phdr.p_memsz;
			if (maxadr < end_adr)
				maxadr = end_adr;
		}
		++i;
	}
	return maxadr - ex->v_addr;
}

static AlignedMemory *_allocate_aligned(size_t size, size_t alignment) {
	const size_t mask = alignment - 1;
	AlignedMemory *mem = malloc(sizeof *mem + size + alignment);
	if (!mem)
		return NULL;
	mem->value = (void *) ((((uintptr_t) (mem + 1)) + mask) & ~mask);
	return mem;
}

static AlignedMemory *_allocate_body(size_t size) {
	AlignedMemory *mem = _allocate_aligned(size, getpagesize());
	if (!mem)
		return NULL;
	if (mprotect(mem->value, size, PROT_READ | PROT_EXEC | PROT_WRITE) != 0) {
		free(mem);
		return NULL;
	}
	return mem;
}

static char *_load_data(Elf32_Exec *ex, int offset, int size) {
	if (size && lseek(ex->fp, offset - ex->v_addr, SEEK_SET)) {
		char *data = malloc(size + 1);
		if (read(ex->fp, data, size) == size) {
			data[size] = 0;
			return data;
		} else {
			free(data);
		}
	}
	return 0;
}

/* Вспомогательная функция */
static inline unsigned int _look_sym(Elf32_Exec *ex, const char *name) {
	Libs_Queue *lib = ex->libs;
	unsigned int func = 0;
	while (lib && !func) {
		func = (unsigned int)loader_find_function(lib->lib, name);
		lib = lib->next;
	}
	return func;
}

/* функция пролетается рекурсивно по либам которые в зависимостях */
unsigned int try_search_in_base(Elf32_Exec *ex, const char *name, int bind_type) {
	EP3_DEBUG("'%s' Searching in libs... ", name);
	unsigned int address = 0;

	if (ex->type == EXEC_LIB && !ex->dyn[DT_SYMBOLIC])
		address = loader_find_export(ex, name);

	if (!address)
		address = (unsigned int)_look_sym(ex, name);

	if (!address) {
		if (!address && ex->meloaded) {
			Elf32_Exec *mex = (Elf32_Exec *)ex->meloaded;
			while (mex && !address && mex->type == EXEC_LIB) {
				address = loader_find_export(mex, name);
				mex = (Elf32_Exec *)mex->meloaded;
			}
		}
	}

	if (!address)
		EP3_DEBUG(" not found!\n");
	else
		EP3_DEBUG(" %X found\n", address);

	return address;
}

// Релокация
int loader_do_reloc(Elf32_Exec *ex, Elf32_Dyn *dyn_sect, Elf32_Phdr *phdr) {
	unsigned int i = 0;
	Elf32_Word libs_needed[64] = {};
	unsigned int libs_cnt = 0;
	char dbg[128];

	// Вытаскиваем теги
	while (dyn_sect[i].d_tag != DT_NULL) {
		if (dyn_sect[i].d_tag <= DT_FLAGS) {
			switch (dyn_sect[i].d_tag) {
				case DT_SYMBOLIC:
					// Флаг SYMBOLIC-библиотек. В d_val 0, даже при наличии :(
					ex->dyn[dyn_sect[i].d_tag] = 1;
				break;
				case DT_NEEDED:
					// Получаем смещения в .symtab на имена либ
					libs_needed[libs_cnt++] = dyn_sect[i].d_un.d_val;
				break;
				case DT_DEBUG:
					// Set the DT_DEBUG entry to the addres of _r_debug for GDB
					dyn_sect[i].d_un.d_ptr = (Elf32_Addr) &_r_debug;
				break;
				default:
					EP3_ERROR("Unknown DT_XXXX tag: %d\n", dyn_sect[i].d_tag);
					ex->dyn[dyn_sect[i].d_tag] = dyn_sect[i].d_un.d_val;
				break;
			}
		}
		++i;
	}

	// Таблички. Нужны только либам, и их юзающим)
	ex->symtab = ex->dyn[DT_SYMTAB] ? (Elf32_Sym *)(ex->body->value + ex->dyn[DT_SYMTAB] - ex->v_addr) : 0;
	ex->jmprel = (Elf32_Rel *)(ex->body->value + ex->dyn[DT_JMPREL] - ex->v_addr);
	ex->strtab = ex->dyn[DT_STRTAB] ? ex->body->value + ex->dyn[DT_STRTAB] - ex->v_addr : 0;

	EP3_DEBUG("STRTAB: %X\n", ex->dyn[DT_STRTAB]);
	EP3_DEBUG("SYMTAB: %X %p\n", ex->dyn[DT_SYMTAB], ex->symtab);

	if (ex->type == EXEC_LIB) {
		Elf32_Word *hash_hdr = (Elf32_Word *)_load_data(ex, ex->dyn[DT_HASH], 8);
		if (hash_hdr) {
			int hash_size = hash_hdr[0] * sizeof(Elf32_Word) + hash_hdr[1] * sizeof(Elf32_Word) + 8;
			ex->hashtab = (Elf32_Word *)_load_data(ex, ex->dyn[DT_HASH], hash_size);
			free(hash_hdr);
			if (!ex->hashtab)
				goto __hash_err;
		} else {
		__hash_err:
			EP3_ERROR("Hash tab is mising");
			return E_HASTAB;
		}
	}

	// Загрузка библиотек
	for (i = 0; i < libs_cnt; ++i) {
		char *lib_name = ex->strtab + libs_needed[i];
		Elf32_Lib *lib;
		if ((lib = loader_lib_open(lib_name, ex))) {
			Libs_Queue *libq = malloc(sizeof(Libs_Queue));
			libq->lib = lib;

			if (ex->libs)
				libq->next = ex->libs;
			else
				libq->next = 0;

			ex->libs = libq;
		} else {
			EP3_ERROR("Can't load lib: %s!", lib_name);
			return E_SHARED;
		}
	}

	// Релокация
	if (ex->dyn[DT_RELSZ]) {
		i = 0;
		unsigned int *addr;
		char *name;
		Elf32_Word r_type;
		unsigned int func = 0;
		int symtab_index = 0;
		int bind_type = 0;
		int reloc_type = 0;

		// Таблица релокаций
		Elf32_Rel *reltab = (Elf32_Rel *)_load_data(ex, phdr->p_offset + ex->dyn[DT_REL] - phdr->p_vaddr, ex->dyn[DT_RELSZ]);

		if (!reltab) {
			loader_elf_close(ex);
			return E_RELOCATION;
		}

		while (i * sizeof(Elf32_Rel) < ex->dyn[DT_RELSZ]) {
			r_type = ELF32_R_TYPE(reltab[i].r_info);
			symtab_index = ELF32_R_SYM(reltab[i].r_info);

			Elf32_Sym *sym = ex->symtab ? &ex->symtab[symtab_index] : 0;
			bind_type = sym ? ELF_ST_BIND(sym->st_info) : 0;
			reloc_type = sym ? ELF_ST_TYPE(sym->st_info) : 0;
			addr = (unsigned int *)(ex->body->value + reltab[i].r_offset - ex->v_addr);

			switch (r_type) {
			case R_ARM_NONE:
				break;

			case R_ARM_RABS32:
				EP3_DEBUG("R_ARM_RABS32\n");
				*addr += (unsigned int)(ex->body->value - ex->v_addr);
				name = ex->strtab + sym->st_name;
				EP3_DEBUG("*addr = %X\n", *addr);
				break;
			case R_ARM_ABS32:
				EP3_DEBUG("R_ARM_ABS32\n");

				if (!ex->symtab) {
					if (loader_warnings)
						EP3_ERROR("warning: symtab not found, but relocation R_ARM_ABS32 is exist");
					*addr = (unsigned int)ex->body->value;
					break;
				}

				if (!ex->strtab) {
					if (loader_warnings)
						EP3_ERROR("warning: symtab not found, but relocation R_ARM_ABS32 is exist");
					*addr = (unsigned int)ex->body->value;
					break;
				}

				/* на всякий случай, вдруг сум пустой будет */
				if (sym) {
					/* имя требуемой функции */
					name = ex->strtab + sym->st_name;

					// Если нужен указатель на эльф
					if (name[4] == 0 && name[0] == '_' && name[1] == '_' && name[2] == 'e' && name[3] == 'x') {
						ex->__is_ex_import = 1;
						*addr = (unsigned int)ex;
						break;
					}

					switch (reloc_type) {
					case STT_NOTYPE:
						EP3_DEBUG("STT_NOTYPE\n");
						if (bind_type != STB_LOCAL)
							func = (unsigned int) (ex->body->value + sym->st_value);
						else
							func = (unsigned int) (sym->st_value);

						goto skeep_err;

					default:
						if (sym->st_value)
							func = (unsigned int)ex->body->value + sym->st_value;
						else
							func = try_search_in_base(ex, name, bind_type);
						break;
					}

				} else {
					func = 0;
				}

				/* ничего не нашли, жаль */
				if (!func && bind_type != STB_WEAK) {
					EP3_ERROR("[2] Undefined reference to `%s'\n", name ? name : "");
					return E_UNDEF;
				}

			skeep_err:
				if (func < 0x6D00) {
					EP3_DEBUG("WARNING: SEEE HEER!!!! %s\n", name);
				}
				/* в ABS32 релоке в *addr всегда должен быть 0 */
				*addr += func;
				EP3_DEBUG("*addr = %X\n", *addr);
				break;

			case R_ARM_RELATIVE:
				EP3_DEBUG("R_ARM_RELATIVE\n");
				*addr += (unsigned int)(ex->body->value - ex->v_addr);
				name = ex->strtab + sym->st_name;
				EP3_DEBUG("*addr = %X\n", *addr);
				break;

			case R_ARM_GLOB_DAT:
			case R_ARM_JUMP_SLOT:
				EP3_DEBUG("R_ARM_GLOB_DAT\n");

				if (!ex->symtab) {
					EP3_ERROR("Relocation R_ARM_GLOB_DAT cannot run without symtab\n");
					free(reltab);
					return E_SYMTAB;
				}

				if (!ex->strtab) {
					EP3_ERROR("Relocation R_ARM_GLOB_DAT cannot run without strtab\n");
					free(reltab);
					return E_STRTAB;
				}

				if (sym) {
					name = ex->strtab + sym->st_name;
				} else
					name = 0;

				EP3_DEBUG(" strtab: '%s' \n", name);

				if (symtab_index && name) {
					switch (reloc_type) {
					case STT_NOTYPE:
						EP3_DEBUG("STT_NOTYPE\n");
						if (bind_type != STB_LOCAL)
							func = (unsigned int) (ex->body->value + sym->st_value);
						else
							func = sym->st_value;
						goto skeep_err1;

					default:
						if (sym->st_value)
							func = (unsigned int)ex->body->value + sym->st_value;
						else {
							EP3_DEBUG("Searching in libs...\n");
							func = try_search_in_base(ex, name, bind_type);
						}
						break;
					}

					if (!func && bind_type != STB_WEAK) {
						EP3_ERROR("[2] Undefined reference to `%s'\n", name ? name : "");
						return E_UNDEF;
					}

				skeep_err1:

					/* В доках написано что бинды типа STB_WEAK могут быть нулевыми */
					*addr = func;

					if (*addr) {
						EP3_DEBUG("found at 0x%X\n", *addr);
					} else {
						EP3_DEBUG("warning: address is zero\n");
					}
				} else
					*addr = sym->st_value;

				EP3_DEBUG("Adr 0x%X\n", *addr);

				break;

			case R_ARM_COPY:
				EP3_DEBUG("R_ARM_COPY\n");
				memcpy((void *)addr,
					(void *)(ex->body->value + sym->st_value), sym->st_size);
				break;

				/* хз чо за релок, ни в одном лоадере его не встречал,
				 * хотя по описанию похож на R_ARM_REL32.
				 * Но, если выполнить релокацию по описанию - эльфятник падает.
				 * С пропуском - работает, так что хайтек.
				 */
			case R_ARM_THM_RPC22:
				EP3_DEBUG("R_ARM_THM_RPC22 stub\n");
				break;

				// S + A - P
			case R_ARM_REL32:
				EP3_DEBUG("R_ARM_REL32\n");
				*addr += sym->st_value - (unsigned int)addr;
				EP3_DEBUG("*addr = %X\n", *addr);
				break;

			default:
				EP3_ERROR("Fatal error! Unknown type relocation '%d'!", r_type);
				return E_RELOCATION;
			}
			++i;
		}

		free(reltab);
	}

	// Биндим функции
	if (ex->dyn[DT_PLTRELSZ]) {
		i = 0;
		while (i * sizeof(Elf32_Rel) < ex->dyn[DT_PLTRELSZ]) {
			int sym_idx = ELF32_R_SYM(ex->jmprel[i].r_info);
			char *name = ex->strtab + ex->symtab[sym_idx].st_name;
			Elf32_Sym *sym = &ex->symtab[sym_idx];
			Elf32_Word func = 0;
			int bind_type = ELF_ST_BIND(sym->st_info);

			func = try_search_in_base(ex, name, bind_type);
			if (!func && bind_type != STB_WEAK) {
				EP3_ERROR("[3] Undefined reference to `%s'\n", name);
				return E_UNDEF;
			}

			*((Elf32_Word *)(ex->body->value + ex->jmprel[i].r_offset)) = func;
			++i;
		}
	}

	return E_NO_ERROR;
}

// Чтение сегментов из файла
int loader_load_sections(Elf32_Exec *ex) {
	Elf32_Phdr *phdrs = malloc(sizeof(Elf32_Phdr) * ex->ehdr.e_phnum);
	if (!phdrs)
		return E_SECTION;

	unsigned int hdr_offset = ex->ehdr.e_phoff;
	int i = 0;

	unsigned long maxadr = 0;
	unsigned int end_adr;

	// Читаем заголовки
	while (i < ex->ehdr.e_phnum) {
		if (lseek(ex->fp, hdr_offset, SEEK_SET) == -1)
			break;
		if (read(ex->fp, &phdrs[i], sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)) {
			/* кривой заголовок, шлём нафиг этот эльф */
			free(ex->body);
			ex->body = 0;
			free(phdrs);
			return E_PHDR;
		}

		/* тут же и размер бинарника посчитаем */
		if (phdrs[i].p_type == PT_LOAD) {
			if (ex->v_addr > phdrs[i].p_vaddr)
				ex->v_addr = phdrs[i].p_vaddr;
			end_adr = phdrs[i].p_vaddr + phdrs[i].p_memsz;
			if (maxadr < end_adr)
				maxadr = end_adr;
		}

		hdr_offset += ex->ehdr.e_phentsize;
		++i;
	}

	ex->bin_size = maxadr - ex->v_addr;

	if (i == ex->ehdr.e_phnum) { // Если прочитались все заголовки
		// ex->bin_size = loader_get_bin_size(ex, phdrs);

		if (ex->body = _allocate_body(ex->bin_size + 1)) { // Если хватило рамы
			memset(ex->body->value, 0, ex->bin_size + 1);
			memset(ex->dyn, 0, sizeof(ex->dyn));

			for (i = 0; i < ex->ehdr.e_phnum; ++i) {
				Elf32_Phdr phdr = phdrs[i];
				Elf32_Dyn *dyn_sect;

				switch (phdr.p_type) {
				case PT_LOAD:
					if (phdr.p_filesz == 0)
						break; // Пропускаем пустые сегменты
					EP3_DEBUG("PT_LOAD: %X - %X\n", phdr.p_offset, phdr.p_filesz);
					if (lseek(ex->fp, phdr.p_offset, SEEK_SET) != -1) {
						if (read(ex->fp, ex->body->value + phdr.p_vaddr - ex->v_addr, phdr.p_filesz) == phdr.p_filesz)
							break;
					}

					// Не прочитали сколько нужно
					free(ex->body);
					ex->body = 0;
					free(phdrs);
					return E_SECTION;

				case PT_DYNAMIC:
					if (phdr.p_filesz == 0)
						break; // Пропускаем пустые сегменты

					EP3_DEBUG("Load data dynamic segment: %d - %d\n", phdr.p_offset, phdr.p_filesz);
					if (dyn_sect = (Elf32_Dyn *)_load_data(ex, phdr.p_offset, phdr.p_filesz)) {
						if (!loader_do_reloc(ex, dyn_sect, &phdr)) {
							free(dyn_sect);
							break;
						}
					}

					// Если что-то пошло не так...
					free(dyn_sect);
					free(ex->body);
					ex->body = 0;
					free(phdrs);
					return E_SECTION;
				}
			}

			free(phdrs);
			return E_NO_ERROR;
		}
	}

	free(ex->body);
	ex->body = 0;
	free(phdrs);
	return E_RAM;
}

/* constructors */
void loader_run_INIT_Array(Elf32_Exec *ex) {
	if (!ex->dyn[DT_FINI_ARRAY])
		return;
	size_t sz = ex->dyn[DT_INIT_ARRAYSZ] / sizeof(void *);
	void **arr = (void **)(ex->body->value + ex->dyn[DT_INIT_ARRAY] - ex->v_addr);

	EP3_DEBUG("init_array sz: %d\n", sz);

	for (int i = 0; i < sz; ++i) {
		EP3_DEBUG("init %d: 0x%p\n", i, arr[i]);
		((void (*)())arr[i])();
	}
}

/* destructors */
void loader_run_FINI_Array(Elf32_Exec *ex) {
	if (!ex->dyn[DT_FINI_ARRAY])
		return;
	size_t sz = ex->dyn[DT_FINI_ARRAYSZ] / sizeof(void *);
	void **arr = (void **)(ex->body->value + ex->dyn[DT_FINI_ARRAY] - ex->v_addr);

	EP3_DEBUG("fini_array sz: %d\n", sz);

	for (int i = 0; i < sz; ++i) {
		EP3_DEBUG("fini %d: 0x%p\n", i, arr[i]);
		((void (*)())arr[i])();
	}
}
