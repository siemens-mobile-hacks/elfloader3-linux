#pragma once

#include <cstdint>
#include <map>

struct FontChar {
	int w;
	int h;
	uint8_t *bitmap;
};

struct Font {
	int w;
	int h;
	std::map<uint16_t, FontChar> chars;
	
	~Font() {
		for (auto it: chars) {
			delete[] it.second.bitmap;
		}
	}
};
