#include "Theme.h"

#include <string>
#include <cassert>
#include <regex>

#include "SieFs.h"
#include "utils.h"

static Theme *m_instance = nullptr;

void Theme::init() {
	assert(m_instance == nullptr);
	m_instance = new Theme();
	m_instance->load();
}

Theme *Theme::instance() {
	assert(m_instance != nullptr);
	return m_instance;
}

void Theme::load() {
	loadColorTheme();
}

void Theme::loadColorTheme() {
	int color_index = 0;
	std::smatch m;
	std::regex color_re("^\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	
	std::string col = readFile(SieFs::sie2path("2:\\Default\\ColorControls.col"));
	
	bool color_data_started = false;
	for (auto line: strSplit("\n", col)) {
		line = trim(line);
		if (!color_data_started) {
			color_data_started = strStartsWith(line, "Data");
		} else if (line.size() > 0) {
			assert(std::regex_match(line, m, color_re));
			m_pallete[color_index][0] = strToInt(m[1].str(), 10, 0);
			m_pallete[color_index][1] = strToInt(m[2].str(), 10, 0);
			m_pallete[color_index][2] = strToInt(m[3].str(), 10, 0);
			m_pallete[color_index][3] = strToInt(m[4].str(), 10, 0);
			color_index++;
		}
	}
}

char *Theme::getColorPtr(int index) {
	assert(index >= 0 && index < MAX_PALETTE_COLORS);
	return m_pallete[index];
}
