#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <unistd.h>
#include <string>
#include <filesystem>

#include "elfloader/loader.h"
#include "elfloader/env.h"
#include "stubs.h"
#include "swi.h"
#include "log.h"
#include "utils.h"

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

extern "C" void SWI_elfclose(Elf32_Exec *ex) {
	loader_elf_close(ex);
}

static void init_switab() {
	// LIBC
	switab_functions[0x0113]	= (void *) SWI_strpbrk,
	switab_functions[0x0113]	= (void *) SWI_strcspn,
	switab_functions[0x0114]	= (void *) SWI_strncat,
	switab_functions[0x0115]	= (void *) SWI_strncmp,
	switab_functions[0x0116]	= (void *) SWI_strncpy,
	switab_functions[0x0117]	= (void *) SWI_strrchr,
	switab_functions[0x0118]	= (void *) SWI_strstr,
	switab_functions[0x0119]	= (void *) SWI_strtol,
	switab_functions[0x011A]	= (void *) SWI_strtoul,
	switab_functions[0x011C]	= (void *) SWI_memcmp,
	switab_functions[0x011D]	= (void *) SWI_zeromem,
	switab_functions[0x011E]	= (void *) SWI_memcpy,
	switab_functions[0x0132]	= (void *) SWI_memmove,
	switab_functions[0x0162]	= (void *) SWI_setjmp,
	switab_functions[0x011B]	= (void *) SWI_snprintf,
	switab_functions[0x00BB]	= (void *) SWI_memset,
	switab_functions[0x0092]	= (void *) SWI_calloc,
	switab_functions[0x0085]	= (void *) SWI_strcmpi,
	switab_functions[0x0054]	= (void *) SWI_StrToInt,
	switab_functions[0x0014]	= (void *) SWI_malloc,
	switab_functions[0x0015]	= (void *) SWI_free,
	switab_functions[0x0015]	= (void *) SWI_free,
	switab_functions[0x8014]	= (void *) SWI_malloc_adr,
	switab_functions[0x8015]	= (void *) SWI_mfree_adr,
	switab_functions[0x0016]	= (void *) SWI_sprintf,
	switab_functions[0x0017]	= (void *) SWI_strcat,
	switab_functions[0x0018]	= (void *) SWI_strchr,
	switab_functions[0x0019]	= (void *) SWI_strcmp,
	switab_functions[0x001A]	= (void *) SWI_strcpy,
	switab_functions[0x001B]	= (void *) SWI_strlen,
	
	switab_functions[0x00D]		= (void *) SWI_close;
	switab_functions[0x190]		= (void *) SWI_MutexCreate;
	switab_functions[0x191]		= (void *) SWI_MutexDestroy;
	switab_functions[0x2EE]		= (void *) SWI_elfclose;
}

extern int __executable_start;

std::string normalizeLibraryPath(const std::string &library_path_env) {
	std::vector<std::string> new_library_path_env;
	for (auto &path: strSplit(";", library_path_env)) {
		if (isDir(path)) {
			std::string new_path = std::string(std::filesystem::canonical(path)) + "/";
			new_library_path_env.push_back(new_path);
		} else {
			fprintf(stderr, "Warning: libdir '%s' not found\n", path.c_str());
		}
	}
	return strJoin(";", new_library_path_env);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s path/to/file.elf\n", argv[0]);
		return 1;
	}
	
	std::string filename = argv[1];
	
	if (!isFileExists(filename)) {
		LOGE("ELF not found: %s\n", filename.c_str());
		return 1;
	}
	
	filename = std::filesystem::canonical(filename);
	
	std::string library_path_env;
	if (getenv("SDK_PATH")) {
		library_path_env = strprintf("%s/lib/NSG:%s/lib", getenv("SDK_PATH"), getenv("SDK_PATH"));
	} else if (getenv("EL3_LIBRARY_PATH")) {
		library_path_env = getenv("EL3_LIBRARY_PATH");
	} else {
		std::string bindir = std::filesystem::canonical(std::filesystem::path(argv[0]).parent_path());
		library_path_env = bindir + "/../sdk/lib/NSG;" + bindir + "/../sdk/lib";
	}
	
	library_path_env = normalizeLibraryPath(library_path_env);
	loader_setenv("LD_LIBRARY_PATH", library_path_env.c_str(), true);
	LOGD("EL3_LIBRARY_PATH: %s\n", loader_getenv("LD_LIBRARY_PATH"));
	
	init_switab();
	loader_set_debug(false);
	loader_gdb_init();
	
	LOGD("Loading ELF: %s\n", filename.c_str());
	
	Elf32_Exec *ex = loader_elf_open(filename.c_str());
	if (!ex) {
		LOGD("loader_elf_open failed.\n");
		return 1;
	}
	
	LOGD("run INIT array\n");
	loader_run_INIT_Array(ex);
	
	auto entry = (int (*)(const char *, const char *, const void *)) loader_elf_entry(ex);
	printf("run entry at %p\n", entry);
	int ret = entry(filename.c_str(), nullptr, nullptr);
	LOGD("entry ret = %d\n", ret);
	
	// loader_elf_close(ex);
	
	return 0;
}
