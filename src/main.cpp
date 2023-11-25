#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <unistd.h>
#include <string>
#include <filesystem>
#include <thread>

#include <sys/types.h>
#include <sys/shm.h>

#include "elfloader/loader.h"
#include "elfloader/env.h"
#include "swi.h"
#include "log.h"
#include "utils.h"

#include "gui/Theme.h"
#include "Loop.h"
#include "IPC.h"
#include "SieFs.h"

static void mrpropper() {
	IPC::instance()->stop();
}

static std::string normalizeLibraryPath(const std::string &library_path_env) {
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

static uint8_t *createSharedMemory(int *mem_id) {
	*mem_id = shmget(IPC_PRIVATE, 4, IPC_CREAT | 0600);
	if (*mem_id < 0) {
		perror("shmget()");
		return nullptr;
	}
	
	auto mem = reinterpret_cast<uint8_t *>(shmat(*mem_id, NULL, 0));
	if (mem == (uint8_t *) -1) {
		perror("shmat()");
		return nullptr;
	}
	
	return mem;
}

int main(int argc, char **argv) {
	std::atexit(mrpropper);
	
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
	
	std::string exe_dir = std::filesystem::canonical(std::filesystem::path(filename).parent_path());
	std::string self_dir = std::filesystem::canonical(std::filesystem::path(argv[0]).parent_path());
	
	std::string library_path_env;
	if (getenv("SDK_PATH")) {
		library_path_env = strprintf("%s/lib/NSG;%s/lib;%s/lib/legacy", getenv("SDK_PATH"), getenv("SDK_PATH"), getenv("SDK_PATH"));
	} else if (getenv("EL3_LIBRARY_PATH")) {
		library_path_env = getenv("EL3_LIBRARY_PATH");
	} else {
		library_path_env = self_dir + "/../sdk/lib/NSG;" + self_dir + "/../sdk/lib;" + self_dir + "/../sdk/lib/legacy;" + self_dir + "/rootfs/Data/ZBin/lib";
	}
	
	SieFs::mount("0", self_dir + "/rootfs/Data");
	SieFs::mount("1", self_dir + "/rootfs/Cache");
	SieFs::mount("2", self_dir + "/rootfs/Config");
	SieFs::mount("4", "");
	
	library_path_env = normalizeLibraryPath(library_path_env);
	loader_setenv("LD_LIBRARY_PATH", library_path_env.c_str(), true);
	LOGD("EL3_LIBRARY_PATH: %s\n", loader_getenv("LD_LIBRARY_PATH"));
	
	Loop *loop = new Loop();
	loop->init();
	Loop::setInstance(loop);
	
	IPC *ipc = IPC::instance();
	ipc->setWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	ipc->setHelperPath(self_dir + "/host-helper/elfloader3-helper");
	ipc->start();
	
	GBS_Init();
	MMI_Init();
	Helper_Init();
	Theme::init();
	CSM_Init();
	GUI_Init();
	
	loader_init_switab();
	loader_set_debug(false);
	loader_gdb_init();
	
	GBS_RunInContext(MMI_CEPID, [=]() {
		LOGD("Loading ELF: %s\n", filename.c_str());
		
		printf("---------------------------------------------------------\n");
		Elf32_Exec *ex = loader_elf_open(filename.c_str());
		if (!ex) {
			LOGE("loader_elf_open failed.\n");
			return;
		}
		
		LOGD("run INIT array\n");
		loader_run_INIT_Array(ex);
		
		std::string fname = SieFs::path2sie(filename);
		
		auto entry = (int (*)(const char *, const char *, const void *)) loader_elf_entry(ex);
		printf("run entry at %p (fname=%s)\n", entry, fname.c_str());
		int ret = entry(fname.c_str(), "", nullptr);
		LOGD("entry ret = %d\n", ret);
	});
	
	LOGD("Running loop...\n");
	loop->run();
	
	ipc->stop();
	
	return 0;
}
