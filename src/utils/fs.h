#pragma once

#include <string>
#include <vector>

std::string getUnixCacheDir();
std::string getUnixHomeDir();
std::string readFile(const std::string &path);
std::vector<std::string> readDir(const std::string &path);
