/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include "loader.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>

int loader_warnings = 1;
int realtime_libclean = 1;

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

static void *_allocate_body(size_t size) {
	void *body = aligned_alloc(getpagesize(), size);
	if (mprotect(body, size, PROT_READ | PROT_EXEC | PROT_WRITE) != 0) {
		free(body);
		return NULL;
	}
	return body;
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
static inline unsigned int _look_sym(Elf32_Exec *ex, const char *name, unsigned int hash) {
	Libs_Queue *lib = ex->libs;
	unsigned int func = 0;
	while (lib && !func) {
		func = (unsigned int)loader_find_function(lib->lib, name, hash);
		lib = lib->next;
	}
	return func;
}

/* функция пролетается рекурсивно по либам которые в зависимостях */
unsigned int try_search_in_base(Elf32_Exec *ex, const char *name, unsigned int hash, int bind_type) {
	EP3_DEBUG("'%s' Searching in libs... \n", name);
	unsigned int address = 0;

	if (ex->type == EXEC_LIB && !ex->dyn[DT_SYMBOLIC])
		address = loader_find_export(ex, name, hash);

	if (!address)
		address = (unsigned int)_look_sym(ex, name, hash);

	if (!address) {
		if (!address && ex->meloaded) {
			Elf32_Exec *mex = (Elf32_Exec *)ex->meloaded;
			while (mex && !address && mex->type == EXEC_LIB) {
				address = loader_find_export(mex, name, hash);
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

static bool loader_resolve_builtin_symbols(Elf32_Exec *ex, unsigned int *addr, const char *name, unsigned int hash) {
    switch (hash) {
        case 0x000655C8:
            /* запросили указатель на ELF */
            if (strcmp(name, "__ex") == 0) {
                ex->__is_ex_import = 1;
                *addr = (unsigned int) ex;
                return true;
            }
        break;
        case 0x0AB11643:
            /* запросили указатель на таблицу функций */
            if (strcmp(name, "__sys_switab_addres") == 0) {
                *addr = (unsigned int) ex->switab;
                return true;
            }
        break;
        case 0x06AE0C22:
            /* запросили указатель на таблицу функций (указатель) */
            if (strcmp(name, "__switab") == 0) {
                *addr = (unsigned int) &ex->switab;
                return true;
            }
        break;
    }
    return false;
}

// Релокация
int loader_do_reloc(Elf32_Exec *ex, Elf32_Phdr *phdr, int is_iar_elf) {
	unsigned int i = 0;
	Elf32_Word libs_needed[64] = {};
	unsigned int libs_cnt = 0;

	// Вытаскиваем теги
	while (ex->dynamic[i].d_tag != DT_NULL) {
		if (ex->dynamic[i].d_tag <= DT_FLAGS) {
			switch (ex->dynamic[i].d_tag) {
				case DT_SYMBOLIC:
					// Флаг SYMBOLIC-библиотек. В d_val 0, даже при наличии :(
					ex->dyn[ex->dynamic[i].d_tag] = 1;
				break;
				case DT_DEBUG:
					ex->dynamic[i].d_un.d_val = loader_gdb_r_debug();
				break;
				case DT_NEEDED:
					// Получаем смещения в .symtab на имена либ
					libs_needed[libs_cnt++] = ex->dynamic[i].d_un.d_val;
				break;
				default:
					ex->dyn[ex->dynamic[i].d_tag] = ex->dynamic[i].d_un.d_val;
				break;
			}
		}
		++i;
	}

	// Таблички. Нужны только либам, и их юзающим)
	ex->symtab = ex->dyn[DT_SYMTAB] ? (Elf32_Sym *)(ex->body + ex->dyn[DT_SYMTAB] - ex->v_addr) : 0;
	ex->jmprel = (Elf32_Rel *)(ex->body + ex->dyn[DT_JMPREL] - ex->v_addr);
	ex->strtab = ex->dyn[DT_STRTAB] ? ex->body + ex->dyn[DT_STRTAB] - ex->v_addr : 0;

	EP3_DEBUG("STRTAB: %X\n", ex->dyn[DT_STRTAB]);
	EP3_DEBUG("SYMTAB: %X %p\n", ex->dyn[DT_SYMTAB], ex->symtab);

	if (ex->type == EXEC_LIB) {
		ex->hashtab = (Elf32_Word *) (Elf32_Sym *)(ex->body + ex->dyn[DT_HASH] - ex->v_addr);
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
		char *name = NULL;
		Elf32_Word r_type;
		unsigned int func = 0;
		int symtab_index = 0;
		int bind_type = 0;
		int reloc_type = 0;

		// Таблица релокаций
		Elf32_Rel *reltab = ex->body + ex->dyn[DT_REL] - ex->v_addr;
		
		if (is_iar_elf) {
			reltab = (void *) ex->dynamic + ex->dyn[DT_REL];
		}
		
		while (i * sizeof(Elf32_Rel) < ex->dyn[DT_RELSZ]) {
			r_type = ELF32_R_TYPE(reltab[i].r_info);
			symtab_index = ELF32_R_SYM(reltab[i].r_info);

			Elf32_Sym *sym = ex->symtab ? &ex->symtab[symtab_index] : 0;
			bind_type = sym ? ELF_ST_BIND(sym->st_info) : 0;
			reloc_type = sym ? ELF_ST_TYPE(sym->st_info) : 0;
			addr = (unsigned int *)(ex->body + reltab[i].r_offset - ex->v_addr);

			switch (r_type) {
			case R_ARM_NONE:
				break;

			case R_ARM_RABS32:
				EP3_DEBUG("R_ARM_RABS32\n");
				*addr += (unsigned int)(ex->body - ex->v_addr);
				break;
			case R_ARM_ABS32:
				EP3_DEBUG("R_ARM_ABS32\n");

				if (!ex->symtab) {
					if (loader_warnings)
						EP3_ERROR("warning: symtab not found, but relocation R_ARM_ABS32 is exist");
					*addr = (unsigned int)ex->body;
					break;
				}

				if (!ex->strtab) {
					if (loader_warnings)
						EP3_ERROR("warning: symtab not found, but relocation R_ARM_ABS32 is exist");
					*addr = (unsigned int)ex->body;
					break;
				}

				/* на всякий случай, вдруг сум пустой будет */
				if (sym) {
					/* имя требуемой функции */
					name = ex->strtab + sym->st_name;

					// Сразу посчитаем хэш
					unsigned int hash = loader_elf_hash(name);
					
					if (loader_resolve_builtin_symbols(ex, &func, name, hash)) {
						EP3_DEBUG("Builtin symbol %s resolved.\n", name);
						*addr += func;
						break;
					}
					
					switch (reloc_type) {
						case STT_NOTYPE:
							EP3_DEBUG("STT_NOTYPE\n");
							if (bind_type != STB_LOCAL)
								func = (unsigned int) (ex->body + sym->st_value);
							else
								func = (unsigned int) (sym->st_value);

							goto skip_err;

						default:
							if (sym->st_value)
								func = (unsigned int)ex->body + sym->st_value;
							else
								func = try_search_in_base(ex, name, hash, bind_type);
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

			skip_err:
				/* в ABS32 релоке в *addr всегда должен быть 0 */
				*addr += func;
				EP3_DEBUG("*addr = %X\n", *addr);
				break;

			case R_ARM_RELATIVE:
				EP3_DEBUG("R_ARM_RELATIVE\n");
				*addr += (unsigned int)(ex->body - ex->v_addr);
				name = ex->strtab + sym->st_name;
				EP3_DEBUG("*addr = %X\n", *addr);
				break;

			case R_ARM_GLOB_DAT:
			case R_ARM_JUMP_SLOT:
				EP3_DEBUG("R_ARM_GLOB_DAT\n");

				if (!ex->symtab) {
					EP3_ERROR("Relocation R_ARM_GLOB_DAT cannot run without symtab");
					return E_SYMTAB;
				}

				if (!ex->strtab) {
					EP3_ERROR("Relocation R_ARM_GLOB_DAT cannot run without strtab");
					return E_STRTAB;
				}

				if (sym) {
					name = ex->strtab + sym->st_name;
				} else
					name = 0;

				if (symtab_index && name) {
					// Сразу посчитаем хэш
					unsigned int hash = loader_elf_hash(name);
					
					if (loader_resolve_builtin_symbols(ex, &func, name, hash)) {
						EP3_DEBUG("Builtin symbol %s resolved.\n", name);
						*addr += func;
						break;
					}
					
					switch (reloc_type) {
					case STT_NOTYPE:
						EP3_DEBUG("STT_NOTYPE\n");
						if (bind_type != STB_LOCAL)
							func = (unsigned int) (ex->body + sym->st_value);
						else
							func = sym->st_value;
						goto skip_err1;

					default:
						if (sym->st_value)
							func = (unsigned int)ex->body + sym->st_value;
						else {
							EP3_DEBUG("Searching in libs...\n");
							unsigned int hash = loader_elf_hash(name);
							func = try_search_in_base(ex, name, hash, bind_type);
						}
						break;
					}

					if (!func && bind_type != STB_WEAK) {
						EP3_ERROR("[2] Undefined reference to `%s'\n", name ? name : "");
						return E_UNDEF;
					}

				skip_err1:
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
					(void *)(ex->body + sym->st_value), sym->st_size);
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
			unsigned int hash = loader_elf_hash(name);

			func = try_search_in_base(ex, name, hash, bind_type);
			if (!func && bind_type != STB_WEAK) {
				EP3_ERROR("[3] Undefined reference to `%s'\n", name);
				return E_UNDEF;
			}

			*((Elf32_Word *)(ex->body + ex->jmprel[i].r_offset)) = func;
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

		ex->body = _allocate_body(ex->bin_size + 1);
		if (ex->body) { // Если хватило рамы
			memset(ex->body, 0, ex->bin_size + 1);
			memset(ex->dyn, 0, sizeof(ex->dyn));

			for (i = 0; i < ex->ehdr.e_phnum; ++i) {
				Elf32_Phdr phdr = phdrs[i];

				switch (phdr.p_type) {
				case PT_LOAD:
					if (phdr.p_filesz == 0)
						break; // Пропускаем пустые сегменты
					EP3_DEBUG("PT_LOAD: %08X - %08X | %08X - %08X\n", phdr.p_offset, phdr.p_offset + phdr.p_filesz, phdr.p_vaddr - ex->v_addr, phdr.p_vaddr - ex->v_addr + phdr.p_filesz);
					if (lseek(ex->fp, phdr.p_offset, SEEK_SET) != -1) {
						int ret = read(ex->fp, ex->body + phdr.p_vaddr - ex->v_addr, phdr.p_filesz);
						if (ret >= 0 && (size_t) ret == phdr.p_filesz)
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

					int is_iar_elf = 0;
					if (!phdr.p_memsz) {
						EP3_DEBUG("Load data dynamic segment: %d - %d\n", phdr.p_offset, phdr.p_filesz);
						ex->dynamic = (Elf32_Dyn *) _load_data(ex, phdr.p_offset, phdr.p_filesz);
						is_iar_elf = 1;
					} else {
						ex->dynamic = (Elf32_Dyn *) (ex->body + phdr.p_vaddr - ex->v_addr);
						is_iar_elf = 0;
					}
					
					int ret = loader_do_reloc(ex, &phdr, is_iar_elf);
					
					if (is_iar_elf) {
						free(ex->dynamic);
						ex->dynamic = NULL;
					}
					
					if (ret == 0)
						break;
					
					// Если что-то пошло не так...
					free(ex->body);
					ex->body = 0;
					free(phdrs);
					return E_SECTION;
				}
			}

			loader_gdb_add_lib(ex);

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
	if (!ex->dyn[DT_INIT_ARRAY])
		return;
	size_t sz = ex->dyn[DT_INIT_ARRAYSZ] / sizeof(void *);
	void **arr = (void **)(ex->body + ex->dyn[DT_INIT_ARRAY] - ex->v_addr);

	EP3_DEBUG("init_array sz: %d\n", sz);

	for (size_t i = 0; i < sz; ++i) {
		EP3_DEBUG("init %d: 0x%p\n", i, arr[i]);
		((void (*)())arr[i])();
	}
}

/* destructors */
void loader_run_FINI_Array(Elf32_Exec *ex) {
	if (!ex->dyn[DT_FINI_ARRAY])
		return;
	size_t sz = ex->dyn[DT_FINI_ARRAYSZ] / sizeof(void *);
	void **arr = (void **)(ex->body + ex->dyn[DT_FINI_ARRAY] - ex->v_addr);

	EP3_DEBUG("fini_array sz: %d\n", sz);

	for (size_t i = 0; i < sz; ++i) {
		EP3_DEBUG("fini %d: 0x%p\n", i, arr[i]);
		((void (*)())arr[i])();
	}
}
