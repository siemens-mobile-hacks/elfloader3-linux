#pragma once

#include <cstdio>
#include <cstdint>
#include <utility>

class Painter {
	protected:
		uint32_t *m_buffer = nullptr;
		int m_width = 0;
		int m_height = 0;
	public:
		Painter(int width, int height);
		void drawRoundedRect(int x1, int y1, int x2, int y2, int x_radius, int y_radius, int stroke_size, const color_t &bg, const color_t &stroke_color);
		void drawRoundedRectDotted(int x1, int y1, int x2, int y2, int x_radius, int y_radius, int stroke_size, const color_t &bg, const color_t &stroke_color);
		
		void clear(uint32_t color);
		
		void save();
		
		~Painter();
};
