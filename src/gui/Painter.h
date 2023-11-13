#pragma once

#include <cstdio>
#include <cstdint>
#include <utility>

enum {
	CIRCLE_DRAW_UPPER_RIGHT = 1,
	CIRCLE_DRAW_UPPER_LEFT = 2,
	CIRCLE_DRAW_LOWER_RIGHT = 4,
	CIRCLE_DRAW_LOWER_LEFT = 8,
};

class Painter {
	protected:
		uint32_t *m_buffer = nullptr;
		uint32_t *m_tmp_buffer = nullptr;
		int m_width = 0;
		int m_height = 0;
		
		bool m_drawing = false;
		
		void startDrawing();
	public:
		Painter(int width, int height);
		
		void drawPixel(int x, int y, uint32_t color);
		void drawHLine(int x, int y, int width, uint32_t color);
		void drawVLine(int x, int y, int height, uint32_t color);
		
		void drawRoundedRect(int x1, int y1, int x2, int y2, int x_radius, int y_radius, uint32_t fill_color, uint32_t stroke_color);
		
		void drawCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color);
		void drawCircleHelper(int x0, int y0, int rad, uint8_t option, uint32_t color);
		
		void drawEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color);
		void drawEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint32_t color);
		
		void drawRect(int x, int y, int x2, int y2, uint32_t fill_color, uint32_t stroke_color);
		
		void clear(uint32_t color);
		
		void save();
		
		~Painter();
};
