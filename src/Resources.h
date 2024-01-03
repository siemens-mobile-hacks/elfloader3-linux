#pragma once

#include "swi.h"
#include "BinaryStream.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <map>

class Resources {
	protected:
		static constexpr uint32_t MAX_FONTS = 32;
		static constexpr uint32_t MAX_EXT_PALETTE_COLORS = 60;
		static constexpr uint32_t MAX_INT_PALETTE_COLORS = 24;
		
		struct Font {
			int w;
			int h;
			std::map<uint16_t, IMGHDR> chars;
			
			IMGHDR empty_char;
			
			~Font() {
				for (auto it: chars)
					delete[] it.second.bitmap;
			}
		};
		
		char m_int_pallete[MAX_INT_PALETTE_COLORS][4] = {
			{ 0xff, 0xff, 0xff, 0x64 },
			{ 0x00, 0x00, 0x00, 0x64 },
			{ 0xff, 0x00, 0x00, 0x64 },
			{ 0x00, 0x00, 0xff, 0x64 },
			{ 0x00, 0xff, 0x00, 0x64 },
			{ 0xff, 0x00, 0xff, 0x64 },
			{ 0x00, 0xff, 0xff, 0x64 },
			{ 0xff, 0xff, 0x00, 0x64 },
			{ 0xff, 0x80, 0x80, 0x64 },
			{ 0x80, 0x80, 0xff, 0x64 },
			{ 0x80, 0xff, 0x80, 0x64 },
			{ 0xff, 0x80, 0xff, 0x64 },
			{ 0x80, 0xff, 0xff, 0x64 },
			{ 0xff, 0xff, 0x80, 0x64 },
			{ 0x80, 0x00, 0x00, 0x64 },
			{ 0x00, 0x00, 0x80, 0x64 },
			{ 0x00, 0x80, 0x00, 0x64 },
			{ 0x80, 0x00, 0x80, 0x64 },
			{ 0x00, 0x80, 0x80, 0x64 },
			{ 0x80, 0x80, 0x00, 0x64 },
			{ 0x4a, 0x4a, 0x4a, 0x64 },
			{ 0x80, 0x80, 0x8c, 0x64 },
			{ 0xc0, 0xc0, 0xc0, 0x64 },
			{ 0x00, 0x00, 0x00, 0x00 },
		};
		char m_ext_pallete[MAX_EXT_PALETTE_COLORS][4] = {};
		
		std::map<int, IMGHDR *> m_pit_table;
		Font *m_fonts[MAX_FONTS] = {};
		
		std::string m_root;
		
		Resources(const std::string &root);
		
		void load();
		void loadColorTheme();
		void loadPictures();
		void loadFonts();
		bool loadFont(Font *font, const std::string &path);
	
	public:
		IMGHDR *getPicture(int num);
		int getPicIdByUnicode(uint32_t num);
		IMGHDR *getPicByUnicode(uint32_t num);
		
		inline Font *getFont(int id) {
			assert(id >= 0 && id < MAX_FONTS);
			assert(m_fonts[id] != nullptr);
			return m_fonts[id];
		}
		
		IMGHDR *getFontChar(int font_id, int16_t ch, bool allow_bitmaps = true);
		
		static void init(const std::string &root);
		static Resources *instance();
		char *getColorPtr(int index);
		
		static inline std::string getPlatformName() {
			#if defined(ELKA)
				return "ELKA";
			#elif defined(NEWSGOLD)
				return "NSG";
			#else
				return "SG";
			#endif
		}
};
