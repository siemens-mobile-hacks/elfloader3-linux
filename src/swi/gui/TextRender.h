#pragma once

#include <queue>
#include <swilib/gui.h>

#include "src/gui/Painter.h"

class TextRender {
	protected:
		enum {
			ALIGN_LEFT,
			ALIGN_RIGHT,
			ALIGN_CENTER
		};
		
		struct Style {
			uint32_t pen;
			uint32_t brush;
			int default_font;
			int font;
			uint32_t flags;
		};
		
		struct Word {
			int start;
			int end;
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
		int m_prev_align = ALIGN_LEFT;
		int m_prev_line_avail = 0;
		int m_prev_line_height = 0;
		bool m_push_to_prev_line = false;
		
		int m_line_height = 0;
		int m_line_width = 0;
		int m_offset_y = 0;
		
		static int handleModifiers(Style *style, uint16_t *str, int length);
		static int getFontByEXXX(uint16_t ch);
		static bool isWordSeparator(uint16_t ch);
		static int getAlignFromStyle(Style *style);
		
		void parseWord(int start, int end);
		void onNewWord(int start, int end, int w, int h);
		int drawWord(Word *word, int x, int y);
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
