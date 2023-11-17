#pragma once

#include <cstdio>
#include <cstdint>
#include <utility>
#include <vector>

class Painter {
	protected:
		int m_width = 0;
		int m_height = 0;
		bool m_perfect_drawing = false;
		
		std::vector<uint32_t> m_buffer;
		std::vector<bool> m_mask;
		
		enum {
			CIRCLE_DRAW_UPPER_RIGHT		= 1 << 0,
			CIRCLE_DRAW_UPPER_LEFT		= 1 << 1,
			CIRCLE_DRAW_LOWER_LEFT		= 1 << 2,
			CIRCLE_DRAW_LOWER_RIGHT		= 1 << 3,
		};
		
		void startPerfectDrawing(uint32_t color);
		void stopPerfectDrawing();
		
		static bool isInEllipseRange(int x, int y, int start, int end);
		static std::tuple<int, int> getEllipseSectionRegion(int x1, int x2, int y1, int y2);
		
		void strokeCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color);
		void strokeCircleHelper(int x0, int y0, int rad, uint8_t option, uint32_t color);
		
		void fillCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color);
		void fillCircleHelper(int x0, int y0, int rad, uint8_t option, uint32_t color);
		
		void strokeEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color, int start = -1, int end = -1);
		void strokeEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint32_t color, int start = -1, int end = -1);
		
		void fillEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color, int start = -1, int end = -1);
		void fillEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint32_t color, int start = -1, int end = -1);
	public:
		Painter(int width, int height);
		
		uint32_t blendColors(uint32_t old_color, uint32_t new_color);
		
		void drawPixel(int x, int y, uint32_t color);
		void drawHLine(int x, int y, int width, uint32_t color);
		void drawVLine(int x, int y, int height, uint32_t color);
		void drawLine(int x, int y, int w, int h, uint32_t color);
		
		void strokeRect(int x, int y, int w, int h, uint32_t color);
		void fillRect(int x, int y, int w, int h, uint32_t color);
		void drawRect(int x, int y, int w, int h, uint32_t fill_color, uint32_t stroke_color);
		
		void strokeRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint32_t color);
		void fillRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint32_t color);
		void drawRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint32_t fill_color, uint32_t stroke_color);
		
		void strokeArc(int x, int y, int w, int h, int start_angle, int sweep_angle, uint32_t color);
		void fillArc(int x, int y, int w, int h, int start_angle, int sweep_angle, uint32_t color);
		void drawArc(int x, int y, int w, int h, int start, int end, uint32_t fill_color, uint32_t stroke_color);
		
		void clear(uint32_t color);
		
		void drawMask(const uint8_t *mask, int x, int y, int w, int h, const uint32_t *colors);
		
		void save();
		
		~Painter();
};
