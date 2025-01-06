#include "fs.h"
#include <cstring>
#include <dirent.h>
#include <format>
#include <pwd.h>
#include <stdexcept>
#include <unistd.h>

std::string getUnixHomeDir() {
	const char *home = std::getenv("HOME");
	if (home != nullptr)
		return std::string(home);
	struct passwd *pw = getpwuid(getuid());
	if (pw != nullptr)
		return std::string(pw->pw_dir);
	return "/tmp";
}

std::string getUnixCacheDir() {
	const char *xdgCacheHome = std::getenv("XDG_CACHE_HOME");
	if (xdgCacheHome != nullptr)
		return std::string(xdgCacheHome);
	return getUnixHomeDir() + "/.cache";
}

std::string readFile(const std::string &path) {
	FILE *fp = fopen(path.c_str(), "r");
	if (!fp) {
		throw std::runtime_error(std::format("fopen({}) error: {}", path, strerror(errno)));
	}

	char buff[4096];
	std::string result;
	while (!feof(fp)) {
		int readed = fread(buff, 1, sizeof(buff), fp);
		if (readed > 0)
			result.append(buff, readed);
	}
	fclose(fp);

	return result;
}

std::vector<std::string> readDir(const std::string &path) {
	DIR *dir_p = opendir(path.c_str());
	if (!dir_p)
		throw std::runtime_error(std::format("opendir({}) error: {}", path, strerror(errno)));

	std::vector<std::string> result;
	while (auto *ent = readdir(dir_p)) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;
		result.push_back(path + "/" + ent->d_name);
	}
	closedir(dir_p);

	return result;
}
