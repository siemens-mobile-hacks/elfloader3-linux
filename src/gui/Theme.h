#pragma once

#include <cstdint>
#include <string>

class Theme {
	protected:
		static constexpr uint32_t MAX_PALETTE_COLORS = 100;
		char m_pallete[MAX_PALETTE_COLORS][4] = {};
		
		void load();
		void loadColorTheme();
	public:
		static void init();
		static Theme *instance();
		char *getColorPtr(int index);
};
