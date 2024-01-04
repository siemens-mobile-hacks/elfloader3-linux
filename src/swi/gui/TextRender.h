#pragma once

#include "swi.h"
#include "Resources.h"
#include <queue>

class TextRender {
	protected:
		struct Style {
			uint32_t pen;
			uint32_t brush;
			int default_font;
			int font;
			uint32_t flags;
		};
		
		struct Word {
			uint16_t start;
			uint16_t end;
			int w;
			int h;
			Style style;
		};
		
		Painter *m_painter = nullptr;
		Style m_style = {};
		RECT *m_rect = nullptr;
		uint16_t *m_str = nullptr;
		int m_length = 0;
		std::queue<Word> m_words;
		
		int m_line_height = 0;
		int m_line_width = 0;
		int m_offset_y = 0;
		
		static int handleModifiers(Style *style, uint16_t *str, int length);
		static int getFontByEXXX(uint16_t ch);
		static bool isWordSeparator(uint16_t ch);
		
		void parseWord(int start, int end);
		void onNewWord(int start, int end, int w, int h);
		void renderLine();
	public:
		TextRender(Painter *painter, RECT *rect, uint16_t *str, int length);
		
		inline void setFont(int font) {
			m_style.font = font;
			m_style.default_font = font;
		}
		
		inline void setPen(uint32_t color) {
			m_style.pen = color;
		}
		
		inline void setBrush(uint32_t color) {
			m_style.brush = color;
		}
		
		inline void setFlags(uint32_t flags) {
			m_style.flags = flags;
		}
		
		void render();
		void renderInline(int x_offset);
		
		static std::pair<int, int> measureString(int font_id, uint16_t *str, int length);
};
