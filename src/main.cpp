#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <filesystem>
#include <sched.h>
#include <swilib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

#include "src/main.h"
#include "src/IPC.h"
#include "src/elfloader/loader.h"
#include "src/elfloader/env.h"
#include "src/Resources.h"
#include "src/FFS.h"
#include "src/io/Loop.h"
#include "src/swi/gbs.h"
#include "src/swi/gbs/MOPIProcessGroup.h"
#include "src/swi/info.h"
#include "src/swi/init.h"
#include "src/swilib/switab.h"
#include "src/utils/fs.h"
#include "src/utils/string.h"

static volatile sig_atomic_t sigalrm_called = 0;
static std::string buildLibraryPathEnv(const std::vector<std::string> &library_path);

/*
 *
 * $HOME/.cache/elfloader3/NSG
 *
 * -b --bind
 * -f --file
 * -a --arg
 * -r --root
 * -s --sdk
 * --reset
 *
 * elfloader -f test.elf -a <fname>
 * elfloader -r ~/rootfs
 *
 */

int main(int argc, char **argv) {
	spdlog::set_level(spdlog::level::debug);

	std::set_terminate([]() {
		try {
			if (std::current_exception()) {
				std::rethrow_exception(std::current_exception());
			} else {
				spdlog::error("[exception] process terminated!");
			}
		} catch (const std::exception &e) {
			spdlog::error("[exception] {}", e.what());
			std::rethrow_exception(std::current_exception());
		} catch (...) {
			spdlog::error("[exception] unknown exception occurred!");
			std::rethrow_exception(std::current_exception());
		}
	});

	std::string default_root_path = getUnixCacheDir() + "/elfloader3/" + getPlatformName();
	std::string default_sdk_path;
	std::string resources_dir = std::filesystem::canonical(std::filesystem::path(argv[0]).parent_path() / "..").string();

	if (std::filesystem::exists(resources_dir + "/../sdk/swilib/include"))
		default_sdk_path = std::filesystem::canonical(resources_dir + "/../sdk");

	std::string host_helper_path = resources_dir + "/elfloader3-helper";
	if (std::filesystem::exists(resources_dir + "/host-helper/build/elfloader3-helper"))
		host_helper_path = resources_dir + "/host-helper/build/elfloader3-helper";

	argparse::ArgumentParser program("elfloader", "1.0.0");
	program.add_argument("-f", "--file")
		.help("Path to file.elf")
		.required()
		.nargs(1);
	program.add_argument("-a", "--arg")
		.help("Open file with elf")
		.default_value("")
		.nargs(1);
	program.add_argument("-r", "--root")
		.help("Path to filesystem dir")
		.default_value(default_root_path)
		.nargs(1);
	program.add_argument("-s", "--sdk")
		.help("Path to ELF SDK")
		.default_value(default_sdk_path)
		.nargs(1);
	program.add_argument("-L", "--lib-dir")
		.default_value<std::vector<std::string>>({})
		.append()
		.help("Path to dir with .so libraries");
	program.add_argument("-V", "--verbose")
		.help("Verbose log output")
		.default_value(false)
		.implicit_value(true)
		.nargs(0);

	try {
		program.parse_args(argc, argv);
	} catch (const std::exception &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return EXIT_FAILURE;
	}

	if (program.get<bool>("--verbose")) {
		spdlog::set_level(spdlog::level::debug);
	} else {
		spdlog::set_level(spdlog::level::info);
	}

	// Init FFS
	std::string default_fs_content = resources_dir + "/rootfs/" + getPlatformName();
	std::string root_path = program.get<std::string>("--root");

	FFS::mountAndInit("0:", root_path + "/Data", default_fs_content + "/Data");
#ifdef NEWSGOLD
	FFS::mountAndInit("1:", root_path + "/Cache", default_fs_content + "/Cache");
	FFS::mountAndInit("2:", root_path + "/Config", default_fs_content + "/Config");
#else
	FFS::mountAndInit("a:", root_path + "/Cache", default_fs_content + "/Cache");
	FFS::mountAndInit("b:", root_path + "/Config", default_fs_content + "/Config");
#endif
	FFS::mountAndInit("4:", root_path + "/MMCard", default_fs_content + "/MMCard");
	FFS::mount("5:", getUnixHomeDir());
	FFS::mount("6:", "");

	std::vector<std::string> library_path;
	library_path.push_back(root_path + "/Data/ZBin/lib");
	library_path.push_back(root_path + "/MMCard/ZBin/lib");

	std::string elf_file = FFS::any2unix(program.get<std::string>("--file"));
	std::string open_file_with_elf = FFS::any2unix(program.get<std::string>("--arg"));

	if (!std::filesystem::exists(elf_file)) {
		spdlog::error("ELF not found: {}", elf_file);
		return EXIT_FAILURE;
	}
	elf_file = std::filesystem::canonical(elf_file);

	if (open_file_with_elf.size() > 0) {
		if (!std::filesystem::exists(open_file_with_elf)) {
			spdlog::error("File not found: {}", open_file_with_elf);
			return EXIT_FAILURE;
		}
		open_file_with_elf = std::filesystem::canonical(open_file_with_elf);
	}

	std::string sdk_path = program.get<std::string>("--sdk");
	if (sdk_path.size() == 0) {
		spdlog::warn("SDK path is not set!");
		spdlog::warn("Please, set --sdk=path/to/sdk for correct work.");
		spdlog::warn("You can get SDK with this command:");
		spdlog::warn("git clone https://github.com/siemens-mobile-hacks/sdk");
	} else if (!std::filesystem::exists(sdk_path + "/swilib/include")) {
		spdlog::error("SDK not found: {}", sdk_path);
		spdlog::warn("Please, set --sdk=path/to/sdk for correct work.");
		spdlog::warn("You can get SDK with this command:");
		spdlog::warn("git clone https://github.com/siemens-mobile-hacks/sdk");
		return EXIT_FAILURE;
	} else {
		spdlog::debug("SDK: {}", sdk_path);
	}

	if (sdk_path.size() > 0) {
#if defined(ELKA)
		library_path.push_back(sdk_path + "/lib/ELKA");
		library_path.push_back(sdk_path + "/lib/NSG");
		library_path.push_back(sdk_path + "/lib");
#elif defined(NEWSGOLD)
		library_path.push_back(sdk_path + "/lib/NSG");
		library_path.push_back(sdk_path + "/lib");
#elif defined(X75)
		library_path.push_back(sdk_path + "/lib/X75");
		library_path.push_back(sdk_path + "/lib/SG");
		library_path.push_back(sdk_path + "/lib");
#else
		library_path.push_back(sdk_path + "/lib/SG");
		library_path.push_back(sdk_path + "/lib");
#endif
	}

	for (auto dir: program.get<std::vector<std::string>>("--lib-dir")) {
		library_path.push_back(std::filesystem::canonical(dir).string());
	}

	auto library_path_env = buildLibraryPathEnv(library_path);
	spdlog::debug("LD_LIBRARY_PATH={}", library_path_env);
	loader_setenv("LD_LIBRARY_PATH", library_path_env.c_str(), true);

	spdlog::debug("Loading ELF: {}", elf_file);
	spdlog::debug("First argument: {}", open_file_with_elf.size() > 0 ? open_file_with_elf : "<none>");

	std::string arg0 = FFS::unix2dos(elf_file);
	std::string arg1 = open_file_with_elf.size() > 0 ? FFS::unix2dos(open_file_with_elf) : "";

	auto *ipc = IPC::instance();
	ipc->setWindowSize(ScreenW(), ScreenH());
	ipc->setHelperPath(host_helper_path);
	ipc->start();

	Resources::init(resources_dir);
	OS_Init();

	loader_install_swihook();
	loader_init_swilib();

	Loop::instance()->setTimeout([=]() {
		GBS_RunInContext(MMI_CEPID, [=]() {
			Elf32_Exec *ex = loader_elf_open(arg0.c_str());
			if (!ex) {
				spdlog::error("Invalid ELF: {}", elf_file);
				exit(1);
			}

			auto entry = (int (*)(const char *, const char *, const void *)) loader_elf_entry(ex);
			spdlog::debug("Run entry at {} (exe={}, fname={})", (void *) entry, arg0, arg1);
			int ret = entry(arg0.c_str(), arg1.c_str(), nullptr);
			spdlog::debug("int main() = {}", ret);
		});
	});

	Loop::instance()->run();
	spdlog::debug("Loop exit...");

	std::signal(SIGALRM, [](int signal) {
		const char err[] = "Program hangs at exit, sending SIGKILL...\n";
		write(2, err, strlen(err));
		kill(getpid(), SIGKILL);
	});
	alarm(10);

	MOPIProcessGroup::cleanup();

	return 0;
}

static std::string makeTempDir() {
	std::string temp_dir = std::filesystem::temp_directory_path().string() + "/elfloader3-XXXXXX";
	if (!mkdtemp(temp_dir.data()))
		throw std::runtime_error(std::format("mkdtemp(): {}", strerror(errno)));
	return temp_dir;
}

std::string tempDir() {
	static std::string temp_dir = makeTempDir();
	return temp_dir;
}

static std::string buildLibraryPathEnv(const std::vector<std::string> &library_path) {
	std::vector<std::string> new_library_path;
	for (auto &path: library_path) {
		auto unix_path = FFS::any2unix(path);
		new_library_path.push_back(FFS::any2dos(unix_path) + "\\");
	}
	return strJoin(";", new_library_path);
}
