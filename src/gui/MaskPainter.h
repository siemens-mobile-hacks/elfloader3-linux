#pragma once

#include <cstdio>
#include <cstdint>
#include <utility>
#include <vector>

enum {
	CIRCLE_DRAW_UPPER_RIGHT = 1,
	CIRCLE_DRAW_UPPER_LEFT = 2,
	CIRCLE_DRAW_LOWER_RIGHT = 4,
	CIRCLE_DRAW_LOWER_LEFT = 8,
};

class MaskPainter {
	protected:
		std::vector<uint8_t> m_buffer;
		int m_width;
		int m_height;
		
		static bool isInEllipseRange(int x, int y, int start, int end);
		static std::tuple<int, int> getEllipseSectionRegion(int x1, int x2, int y1, int y2);
		
		void drawCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color);
		void drawCircleHelper(int x0, int y0, int rad, uint8_t option, uint8_t color);
		
		void fillCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color);
		void fillCircleHelper(int x0, int y0, int rad, uint8_t option, uint8_t color);
		
		void drawEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color, int start = -1, int end = -1);
		void drawEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint8_t color, int start = -1, int end = -1);
		
		void fillEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color, int start = -1, int end = -1);
		void fillEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint8_t color, int start = -1, int end = -1);
	public:
		void setCanvasSize(int width, int height);
		
		void drawPixel(int x, int y, uint8_t color);
		void drawHLine(int x, int y, int width, uint8_t color);
		void drawVLine(int x, int y, int height, uint8_t color);
		
		void drawRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint8_t color);
		void fillRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint8_t color);
		
		void drawRect(int x, int y, int w, int h, uint8_t color);
		void fillRect(int x, int y, int w, int h, uint8_t color);
		
		void drawArc(int x, int y, int w, int h, int start, int end, uint8_t color);
		void fillArc(int x, int y, int w, int h, int start, int end, uint8_t color);
		
		void drawLine(int x1, int y1, int x2, int y2, uint8_t color);
		
		inline int width() {
			return m_width;
		}
		
		inline int height() {
			return m_height;
		}
		
		inline const uint8_t *data() {
			return &m_buffer[0];
		}
};
