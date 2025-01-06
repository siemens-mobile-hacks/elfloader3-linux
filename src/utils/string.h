#pragma once

#include <cstring>
#include <string>
#include <vector>

std::string numberFormat(float num, int max_decimals = 0);
std::string numberFormat(double num, int max_decimals = 0);
std::string strJoin(const std::string &sep, const std::vector<std::string> &lines);
std::vector<std::string> strSplit(const std::string &sep, const std::string &str);
std::string strprintf(const char *format, ...)  __attribute__((format(printf, 1, 2)));
std::string trim(std::string s);

std::string decodeBcd(const std::string &raw);

std::pair<bool, std::string> tryHexToBin(const std::string &hex);
std::string bin2hex(const std::string &raw, bool uc = false);
inline std::string hex2bin(const std::string &hex) {
	auto [success, decoded] = tryHexToBin(hex);
	return success ? decoded : hex;
}
int strToInt(const std::string &s, int base = 10, int default_value = 0);

static inline int hex2byte(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return (c - 'A') + 10;
	if (c >= 'a' && c <= 'f')
		return (c - 'a') + 10;
	return -1;
}

static inline bool strHasEol(const std::string &s) {
	return s.size() >= 2 && s[s.size() - 2] == '\r' && s[s.size() - 1] == '\n';
}

static inline bool strStartsWith(const std::string &a, const std::string &b) {
	return a.size() >= b.size() && memcmp(a.c_str(), b.c_str(), b.size()) == 0;
}
