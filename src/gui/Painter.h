#pragma once

#include <cstdio>
#include <cstdint>
#include <utility>

#include "MaskPainter.h"

class Painter {
	protected:
		uint32_t *m_buffer = nullptr;
		int m_width = 0;
		int m_height = 0;
		
		enum {
			MASK_COLOR_BG	= 1,
			MASK_COLOR_FG	= 2,
		};
		
		MaskPainter m_mask;
	public:
		Painter(int width, int height);
		
		uint32_t blendColors(uint32_t old_color, uint32_t new_color);
		
		void drawPixel(int x, int y, uint32_t color);
		void drawHLine(int x, int y, int width, uint32_t color);
		void drawVLine(int x, int y, int height, uint32_t color);
		
		void drawRoundedRect(int x1, int y1, int x2, int y2, int x_radius, int y_radius, uint32_t fill_color, uint32_t stroke_color);
		void drawRect(int x, int y, int x2, int y2, uint32_t fill_color, uint32_t stroke_color);
		void drawLine(int x, int y, int x2, int y2, uint32_t color);
		void drawArc(int x, int y, int x2, int y2, int start, int end, uint32_t fill_color, uint32_t stroke_color);
		
		void clear(uint32_t color);
		
		void drawMask(const uint8_t *mask, int x, int y, int w, int h, const uint32_t *colors);
		
		void save();
		
		~Painter();
};
