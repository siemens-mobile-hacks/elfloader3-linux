#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <unistd.h>
#include <string>
#include <filesystem>

#include "elfloader/loader.h"
#include "elfloader/env.h"
#include "swi.h"
#include "log.h"
#include "utils.h"

#include "SieFs.h"

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
	
	std::string self_dir = std::filesystem::canonical(std::filesystem::path(argv[0]).parent_path());
	
	std::string library_path_env;
	if (getenv("SDK_PATH")) {
		library_path_env = strprintf("%s/lib/NSG;%s/lib;%s/lib/legacy", getenv("SDK_PATH"), getenv("SDK_PATH"), getenv("SDK_PATH"));
	} else if (getenv("EL3_LIBRARY_PATH")) {
		library_path_env = getenv("EL3_LIBRARY_PATH");
	} else {
		library_path_env = self_dir + "/../sdk/lib/NSG;" + self_dir + "/../sdk/lib;" + self_dir + "/../sdk/lib/legacy";
	}
	
	SieFs::mount("0", self_dir + "/rootfs/Data");
	SieFs::mount("1", self_dir + "/rootfs/Cache");
	SieFs::mount("2", self_dir + "/rootfs/Config");
	SieFs::mount("4", "");
	
	library_path_env = normalizeLibraryPath(library_path_env);
	loader_setenv("LD_LIBRARY_PATH", library_path_env.c_str(), true);
	LOGD("EL3_LIBRARY_PATH: %s\n", loader_getenv("LD_LIBRARY_PATH"));
	
	loader_init_switab();
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
	
	std::string fname = SieFs::path2sie(filename);
	
	auto entry = (int (*)(const char *, const char *, const void *)) loader_elf_entry(ex);
	printf("run entry at %p (fname=%s)\n", entry, fname.c_str());
	int ret = entry(fname.c_str(), "", nullptr);
	LOGD("entry ret = %d\n", ret);
	
	// loader_elf_close(ex);
	
	return 0;
}
