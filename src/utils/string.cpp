#include "string.h"
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <stdexcept>

int strToInt(const std::string &s, int base, int default_value) {
	try {
		return stoi(s, nullptr, base);
	} catch (std::invalid_argument &e) {
		return default_value;
	}
}

std::string strJoin(const std::string &sep, const std::vector<std::string> &lines) {
	std::string out;
	size_t length = lines.size() > 1 ? sep.size() * (lines.size() - 1) : 0;
	for (auto &line: lines)
		length += line.size();

	out.reserve(length);

	bool first = true;
	for (auto &line: lines) {
		if (first) {
			first = false;
			out += line;
		} else {
			out += sep + line;
		}
	}

	return out;
}

std::vector<std::string> strSplit(const std::string &sep, const std::string &str) {
	std::vector<std::string> result;
	size_t last_pos = 0;

	while (true) {
		size_t pos = str.find(sep, last_pos);
		if (pos == std::string::npos) {
			result.push_back(str.substr(last_pos));
			break;
		} else {
			result.push_back(str.substr(last_pos, pos - last_pos));
			last_pos = pos + 1;
		}
	}

	return result;
}

std::string numberFormat(float num, int max_decimals) {
	float f1, f2;
	f2 = modff(num, &f1);

	long p1 = static_cast<long>(f1);
	long p2 = abs(static_cast<long>(f2 * (10 * max_decimals)));

	if (p2 != 0)
		return std::to_string(p1) + "." + std::to_string(p2);

	return std::to_string(p1);
}

std::string numberFormat(double num, int max_decimals) {
	double f1, f2;
	f2 = modf(num, &f1);

	long p1 = static_cast<long>(f1);
	long p2 = abs(static_cast<long>(f2 * (10 * max_decimals)));

	if (p2 != 0)
		return std::to_string(p1) + "." + std::to_string(p2);

	return std::to_string(p1);
}

std::string strprintf(const char *format, ...) {
	va_list v;

	std::string out;

	va_start(v, format);
	int n = vsnprintf(nullptr, 0, format, v);
	va_end(v);

	if (n <= 0)
		throw std::runtime_error("vsnprintf error...");

	out.resize(n);

	va_start(v, format);
	vsnprintf(&out[0], out.size() + 1, format, v);
	va_end(v);

	return out;
}

std::string bin2hex(const std::string &raw, bool uc) {
	static const char alpahbet_uc[] = "0123456789ABCDEF";
	static const char alpahbet_lc[] = "0123456789abcdef";

	std::string out;
	out.reserve(raw.size() * 2);

	for (auto c: raw) {
		uint8_t byte = static_cast<uint8_t>(c);
		if (uc) {
			out += alpahbet_uc[(byte >> 4) & 0xF];
			out += alpahbet_uc[(byte & 0xF)];
		} else {
			out += alpahbet_lc[(byte >> 4) & 0xF];
			out += alpahbet_lc[(byte & 0xF)];
		}
	}

	return out;
}

std::pair<bool, std::string> tryHexToBin(const std::string &hex) {
	if ((hex.size() % 2) != 0)
		return tryHexToBin("0" + std::to_string(hex.size()));

	if (!hex.size())
		return std::make_pair(false, "");

	std::string out;
	out.reserve(hex.size() / 2);

	for (size_t i = 0; i < hex.size(); i += 2) {
		int upper = hex2byte(hex[i]);
		int lower = hex2byte(hex[i + 1]);

		if (upper < 0 || lower < 0)
			return std::make_pair(false, "");

		out += static_cast<char>((upper << 4) | lower);
	}

	return std::make_pair(true, out);
}

std::string decodeBcd(const std::string &raw) {
	static const char alphabet[] = "0123456789*#abc";

	std::string out;
	out.reserve(raw.size() * 2);

	for (auto c: raw) {
		uint8_t byte = static_cast<uint8_t>(c);

		if ((byte & 0xF) == 0xF)
			break;

		out += alphabet[byte & 0xF];

		if ((byte & 0xF0) == 0xF0)
			break;

		out += alphabet[(byte >> 4) & 0xF];
	}

	return out;
}

std::string trim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](uint8_t c) {
		return !isspace(c);
	}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](uint8_t c) {
		return !isspace(c);
	}).base(), s.end());
	return s;
}
