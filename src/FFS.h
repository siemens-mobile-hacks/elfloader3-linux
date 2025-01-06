#pragma once

#include <string>

class FFS {
	public:
		static void mount(const std::string &drive, const std::string &path);
		static void mountAndInit(const std::string &drive, const std::string &path, const std::string &defaultContentPath);
		static std::string unix2dos(const std::string &path);
		static std::string dos2unix(const std::string &path);
		static std::string any2dos(const std::string &path);
		static std::string any2unix(const std::string &path);
		static std::string caseInsensitivePath(const std::string &path);
};
