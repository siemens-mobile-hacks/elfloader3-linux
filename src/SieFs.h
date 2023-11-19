#pragma once

#include <string>

class SieFs {
	public:
		static void mount(const std::string &drive, const std::string &path);
		static std::string path2sie(const std::string &path);
		static std::string sie2path(const std::string &path);
		static std::string caseInsensitivePath(const std::string &path);
};
