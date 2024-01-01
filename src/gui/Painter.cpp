#include "Painter.h"

#include <cassert>
#include <cmath>
#include <new>
#include <fstream>
#include <ostream>

Painter::Painter(uint8_t *buffer, int width, int height) {
	m_buffer = buffer;
	m_width = width;
	m_height = height;
	m_mask.resize(m_width * m_height);
	setWindow(0, 0, m_width - 1, m_height - 1);
	clear(0xFFFFFFFF);
}

void Painter::startPerfectDrawing(uint32_t color) {
	if ((color & 0xFF000000) == 0xFF000000)
		return;
	m_perfect_drawing = true;
	std::fill(m_mask.begin(), m_mask.end(), false);
}

void Painter::stopPerfectDrawing() {
	m_perfect_drawing = false;
}

void Painter::setWindow(int x, int y, int x2, int y2) {
	m_window_x = x;
	m_window_y = y;
	m_window_x2 = std::min(x2, m_width - 1);
	m_window_y2 = std::min(y2, m_height - 1);
}

void Painter::clear(uint32_t color) {
	for (int i = 0; i < m_height; i++)
		drawHLine(0, i, m_width, color);
}

uint32_t Painter::blendColors(uint32_t old_color, uint32_t new_color) {
	uint32_t alpha = (new_color >> 24) & 0xFF;
	uint32_t inv_alpha = 0xFF - alpha;
	
	if (alpha == 0)
		return old_color;
	
	if (alpha == 0xFF)
		return new_color;
	
	uint32_t new_r = (new_color >> 16) & 0xFF;
	uint32_t new_g = (new_color >> 8) & 0xFF;
	uint32_t new_b = new_color & 0xFF;
	
	uint32_t old_r = (old_color >> 16) & 0xFF;
	uint32_t old_g = (old_color >> 8) & 0xFF;
	uint32_t old_b = old_color & 0xFF;
	
	uint32_t result_r = ((old_r * inv_alpha) >> 8) + ((new_r * alpha) >> 8);
	uint32_t result_g = ((old_g * inv_alpha) >> 8) + ((new_g * alpha) >> 8);
	uint32_t result_b = ((old_b * inv_alpha) >> 8) + ((new_b * alpha) >> 8);
	
	return 0xFF000000 | (result_r << 16) | (result_g << 8) | result_b;
}

uint32_t Painter::invertColor(uint32_t old_color) {
	uint32_t r = (old_color >> 16) & 0xFF;
	uint32_t g = (old_color >> 8) & 0xFF;
	uint32_t b = old_color & 0xFF;
	return 0xFF000000 | ((0xFF - r) << 16) | ((0xFF - g) << 8) | (0xFF - b);
}

void Painter::drawMask(const uint8_t *mask, int x, int y, int w, int h, const uint32_t *colors) {
	for (int mask_y = 0; mask_y < h; mask_y++) {
		int line_start = mask_y * w;
		for (int mask_x = 0; mask_x < w; mask_x++) {
			if (m_perfect_drawing) {
				if (m_mask[line_start + mask_x])
					continue;
				m_mask[line_start + mask_x] = 1;
			}
			
			uint8_t pixel = mask[line_start + mask_x];
			if (pixel)
				drawPixel(x + mask_x, y + mask_y, colors[pixel]);
		}
	}
}

/*
 * Text
 * */

void Painter::drawText(int x, int y, int w, int h, Font *font, uint16_t *text, int length, uint32_t fill_color, uint32_t stroke_color) {
	int offset_x = 0;
	for (int i = 0; i < length; i++) {
		uint16_t ch = text[i];
		
		auto it = font->chars.find(ch);
		if (it == font->chars.end()) {
			printf("Unknown char: %04X\n", ch);
			it = font->chars.find(0xFFFF); // fallback
			assert(it != font->chars.end());
		}
		
		drawBitmap(x + offset_x, y, it->second.w, it->second.h, it->second.bitmap, Bitmap::TYPE_WB, 0, 0, fill_color, stroke_color);
		
		offset_x += it->second.w;
		
		printf("U+%04X: %dx%d\n", ch, it->second.w, it->second.h);
	}
}

/*
 * Bitmap
 * */
void Painter::drawBitmap(int x, int y, int w, int h, uint8_t *bitmap, Bitmap::Type type, int offset_x, int offset_y, uint32_t fill_color, uint32_t stroke_color) {
	int min_img_x = m_window_x + x;
	int max_img_x = m_window_x + x + w - 1 - offset_x;
	
	int min_img_y = m_window_y + y;
	int max_img_y = m_window_y + y + h - 1 - offset_y;
	
	if (min_img_x > m_window_x2 || max_img_x < m_window_x)
		return;
	
	if (min_img_y > m_window_y2 || max_img_y < m_window_y)
		return;
	
	int start_x = offset_x;
	int start_y = offset_y;
	int end_x = w - 1;
	int end_y = h - 1;
	
	if (max_img_y - m_window_y2 > 0)
		end_y -= max_img_y - m_window_y2;
	
	if (max_img_x - m_window_x2 > 0)
		end_x -= max_img_x - m_window_x2;
	
	for (int img_y = start_y; img_y <= end_y; img_y++) {
		for (int img_x = start_x; img_x <= end_x; img_x++) {
			uint32_t color = Bitmap::getBitmapPixel(type, img_x, img_y, w, h, bitmap);
			if (type == Bitmap::TYPE_WB) {
				if (color & 0xFFFFFF) {
					drawPixel(x + img_x - offset_x, y + img_y - offset_y, stroke_color);
				} else {
					drawPixel(x + img_x - offset_x, y + img_y - offset_y, fill_color);
				}
			} else {
				drawPixel(x + img_x - offset_x, y + img_y - offset_y, color);
			}
		}
	}
}

/*
 * Lines
 * */
void Painter::drawPixel(int x, int y, uint32_t color) {
	x += m_window_x;
	y += m_window_y;
	
	if (x < m_window_x || y < m_window_y || x > m_window_x2 || y > m_window_y2) {
		// printf("ignored pixel %d x %d\n", x, y);
		return;
	}
	
	uint32_t index = y * m_width + x;
	
	if (m_perfect_drawing) {
		if (m_mask[index])
			return;
		m_mask[index] = 1;
	}
	
	if (m_blend_mode == BLEND_MODE_NORMAL) {
		uint16_t *rgb565_pixels = reinterpret_cast<uint16_t *>(m_buffer);
		rgb565_pixels[index] = Bitmap::RGB8888toRGB565(blendColors(Bitmap::RGB565toRGB8888(rgb565_pixels[index]), color));
	} else if (m_blend_mode == BLEND_MODE_INVERT) {
		uint16_t *rgb565_pixels = reinterpret_cast<uint16_t *>(m_buffer);
		rgb565_pixels[index] = Bitmap::RGB8888toRGB565(invertColor(Bitmap::RGB565toRGB8888(rgb565_pixels[index])));
	}
}

void Painter::drawHLine(int x, int y, int width, uint32_t color) {
	for (int i = 0; i < width; i++)
		drawPixel(x + i, y, color);
}

void Painter::drawVLine(int x, int y, int height, uint32_t color) {
	for (int i = 0; i < height; i++)
		drawPixel(x, y + i, color);
}

void Painter::getLinePoints(std::vector<std::pair<int, int>> &result, int x1, int y1, int x2, int y2) {
	int tmp;
	int x,y;
	int dx, dy;
	int err;
	int ystep;
	
	bool swapxy = false;
	
	if (x1 > x2) {
		dx = x1 - x2;
	} else {
		dx = x2 - x1;
	}
	
	if (y1 > y2) {
		dy = y1 - y2;
	} else {
		dy = y2 - y1;
	}
	
	if (dy > dx) {
		swapxy = true;
		std::swap(x1, y1);
		std::swap(x2, y2);
		std::swap(dx, dy);
	}
	
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	
	err = dx >> 1;
	
	if (y2 > y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	
	y = y1;
	
	for (x = x1; x <= x2; x++) {
		if (swapxy) {
			result.push_back({y, x});
		} else {
			result.push_back({x, y});
		}
		
		err -= dy;
		
		if (err < 0) {
			y += ystep;
			err += dx;
		}
	}
}

// From u8g2
void Painter::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
	int tmp;
	int x,y;
	int dx, dy;
	int err;
	int ystep;
	
	bool swapxy = false;
	
	if (y1 == y2) {
		if (x2 > x1) {
			drawHLine(x1, y1, x2 - x1, color);
		} else {
			drawHLine(x2, y2, x1 - x2, color);
		}
		return;
	} else if (x1 == x2) {
		if (y2 > y1) {
			drawVLine(x1, y1, y2 - y1, color);
		} else {
			drawVLine(x2, y2, y1 - y2, color);
		}
		return;
	}
	
	if (x1 > x2) {
		dx = x1 - x2;
	} else {
		dx = x2 - x1;
	}
	
	if (y1 > y2) {
		dy = y1 - y2;
	} else {
		dy = y2 - y1;
	}
	
	if (dy > dx) {
		swapxy = true;
		std::swap(x1, y1);
		std::swap(x2, y2);
		std::swap(dx, dy);
	}
	
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	
	err = dx >> 1;
	
	if (y2 > y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	
	y = y1;
	
	for (x = x1; x <= x2; x++) {
		if (swapxy == 0) {
			drawPixel(x, y, color);
		} else {
			drawPixel(y, x, color);
		}
		
		err -= dy;
		
		if (err < 0) {
			y += ystep;
			err += dx;
		}
	}
}

/*
 * ARC
 * */
void Painter::strokeArc(int x, int y, int w, int h, int start_angle, int sweep_angle, uint32_t color) {
	int end_angle = start_angle + sweep_angle;
	
	int x_radius = w / 2;
	int y_radius = h / 2;
	
	int xl = x + x_radius;
	int yu = y + y_radius;
	
	int xr = x + w - x_radius - 1;
	int yl = y + h - y_radius - 1;
	
	const auto drawArcHelper = [&](int x0, int y0, uint8_t option, int section_angle) {
		auto [part_angle_start, part_angle_end] = getEllipseSectionRegion(start_angle, end_angle, section_angle, section_angle + 90);
		if (part_angle_start != -1) {
			if (part_angle_end - part_angle_start == 90) {
				strokeEllipseHelper(x0, y0, x_radius, y_radius, option, color, -1, -1);
			} else {
				strokeEllipseHelper(x0, y0, x_radius, y_radius, option, color, part_angle_start, part_angle_end);
			}
		}
	};
	
	drawArcHelper(xr, yu, CIRCLE_DRAW_UPPER_RIGHT, 0);
	drawArcHelper(xl, yu, CIRCLE_DRAW_UPPER_LEFT, 90);
	drawArcHelper(xl, yl, CIRCLE_DRAW_LOWER_LEFT, 180);
	drawArcHelper(xr, yl, CIRCLE_DRAW_LOWER_RIGHT, 270);
}

void Painter::fillArc(int x, int y, int w, int h, int start_angle, int sweep_angle, uint32_t color) {
	int end_angle = start_angle + sweep_angle;
	
	int x_radius = w / 2;
	int y_radius = h / 2;
	
	int xl = x + x_radius;
	int yu = y + y_radius;
	
	int xr = x + w - x_radius - 1;
	int yl = y + h - y_radius - 1;
	
	const auto fillArcHelper = [&](int x0, int y0, uint8_t option, int section_angle) {
		auto [part_angle_start, part_angle_end] = getEllipseSectionRegion(start_angle, end_angle, section_angle, section_angle + 90);
		if (part_angle_start != -1) {
			if (part_angle_end - part_angle_start == 90) {
				fillEllipseHelper(x0, y0, x_radius, y_radius, option, color, -1, -1);
			} else {
				fillEllipseHelper(x0, y0, x_radius, y_radius, option, color, part_angle_start, part_angle_end);
			}
		}
	};
	
	fillArcHelper(xr, yu, CIRCLE_DRAW_UPPER_RIGHT, 0);
	fillArcHelper(xl, yu, CIRCLE_DRAW_UPPER_LEFT, 90);
	fillArcHelper(xl, yl, CIRCLE_DRAW_LOWER_LEFT, 180);
	fillArcHelper(xr, yl, CIRCLE_DRAW_LOWER_RIGHT, 270);
}

void Painter::drawArc(int x, int y, int w, int h, int start, int end, uint32_t fill_color, uint32_t stroke_color) {
	if (w <= 0 || h <= 0)
		return;
	
	if ((fill_color & 0xFF000000) != 0) {
		startPerfectDrawing(fill_color);
		fillArc(x, y, w, h, start, end, fill_color);
		stopPerfectDrawing();
	}
	
	if ((stroke_color & 0xFF000000) != 0 && stroke_color != fill_color) {
		startPerfectDrawing(fill_color);
		strokeArc(x, y, w, h, start, end, stroke_color);
		stopPerfectDrawing();
	}
}

/*
 * Rect
 * */
void Painter::strokeRect(int x, int y, int w, int h, uint32_t color) {
	drawHLine(x, y, w, color);
	
	if (h >= 2) {
		drawHLine(x, y + h - 1, w, color);
		drawVLine(x, y + 1, h - 2, color);
		drawVLine(x + w - 1, y + 1, h - 2, color);
	}
}

void Painter::fillRect(int x, int y, int w, int h, uint32_t color) {
	for (int i = 0; i < h; i++)
		drawHLine(x, y + i, w, color);
}

void Painter::drawRect(int x, int y, int w, int h, uint32_t fill_color, uint32_t stroke_color) {
	if (w <= 0 || h <= 0)
		return;
	
	if ((fill_color & 0xFF000000) != 0)
		fillRect(x, y, w, h, fill_color);
	
	if ((stroke_color & 0xFF000000) != 0 && stroke_color != fill_color)
		strokeRect(x, y, w, h, stroke_color);
}

/*
 * Pattern
 * */
void Painter::drawPattern(int x, int y, int w, int h, uint8_t pattern, uint32_t fill_color, uint32_t stroke_color) {
	for (int draw_y = 0; draw_y < h; draw_y++) {
		for (int draw_x = 0; draw_x < w; draw_x++) {
			bool flag = ((pattern >> (7 - (draw_x % 8))) & 1) == 0;
			if (draw_y % 2 == 0) {
				drawPixel(x + draw_x, y + draw_y, (flag ? stroke_color : fill_color));
			} else {
				drawPixel(x + draw_x, y + draw_y, (flag ? fill_color : stroke_color));
			}
		}
	}
}

/*
 * Rounded Rect
 * */
void Painter::strokeRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint32_t color) {
	if (x_radius <= 0 || y_radius <= 0) {
		strokeRect(x, y, w, h, color);
		return;
	}
	
	auto limitRadius = [](int r, int size) -> int {
		if (size <= 3) {
			return 0;
		} else if (size <= 7) {
			return std::min(r, 1);
		} else if (size <= 9) {
			return std::min(r, 3);
		}
		return std::min(r, size / 2);
	};
	
	x_radius = limitRadius(x_radius, w);
	y_radius = limitRadius(y_radius, h);
	
	int xl = x + x_radius;
	int yu = y + y_radius;
	
	int xr = x + w - x_radius - 1;
	int yl = y + h - y_radius - 1;
	
	if (!x_radius || !y_radius) {
		strokeCircleHelper(xl, yu, 0, CIRCLE_DRAW_UPPER_LEFT, color);
		strokeCircleHelper(xr, yu, 0, CIRCLE_DRAW_UPPER_RIGHT, color);
		strokeCircleHelper(xl, yl, 0, CIRCLE_DRAW_LOWER_LEFT, color);
		strokeCircleHelper(xr, yl, 0, CIRCLE_DRAW_LOWER_RIGHT, color);
	} else {
		strokeEllipseHelper(xl, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_LEFT, color);
		strokeEllipseHelper(xr, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_RIGHT, color);
		strokeEllipseHelper(xl, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_LEFT, color);
		strokeEllipseHelper(xr, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_RIGHT, color);
	}
	
	int ww = w - x_radius * 2;
	int hh = h - y_radius * 2;

	xl++;
	yu++;

	if (ww >= 3) {
		ww -= 2;
		h--;
		drawHLine(xl, y, ww, color);
		drawHLine(xl, y + h, ww, color);
	}

	if (hh >= 3) {
		hh -= 2;
		w--;
		drawVLine(x, yu, hh, color);
		drawVLine(x + w, yu, hh, color);
	}
}
	
void Painter::fillRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint32_t color) {
	if (x_radius <= 0 || y_radius <= 0) {
		fillRect(x, y, w, h, color);
		return;
	}
	
	auto limitRadius = [](int r, int size) -> int {
		if (size <= 3) {
			return 0;
		} else if (size <= 7) {
			return std::min(r, 1);
		} else if (size <= 9) {
			return std::min(r, 3);
		}
		return std::min(r, size / 2);
	};
	
	x_radius = limitRadius(x_radius, w);
	y_radius = limitRadius(y_radius, h);
	
	int xl = x + x_radius;
	int yu = y + y_radius;
	
	int xr = x + w - x_radius - 1;
	int yl = y + h - y_radius - 1;
	
	if (!x_radius || !y_radius) {
		fillCircleHelper(xl, yu, 0, CIRCLE_DRAW_UPPER_LEFT, color);
		fillCircleHelper(xr, yu, 0, CIRCLE_DRAW_UPPER_RIGHT, color);
		fillCircleHelper(xl, yl, 0, CIRCLE_DRAW_LOWER_LEFT, color);
		fillCircleHelper(xr, yl, 0, CIRCLE_DRAW_LOWER_RIGHT, color);
	} else {
		fillEllipseHelper(xl, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_LEFT, color);
		fillEllipseHelper(xr, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_RIGHT, color);
		fillEllipseHelper(xl, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_LEFT, color);
		fillEllipseHelper(xr, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_RIGHT, color);
	}
	
	int ww = w - x_radius * 2;
	xl++;
	yu++;
	
	if (ww >= 3) {
		ww -= 2;
		fillRect(xl, y, ww, y_radius + 1, color);
		fillRect(xl, yl, ww, y_radius + 1, color);
	}
	
	int hh = h - y_radius * 2;
	//h--;
	
	if (hh >= 3) {
		hh -= 2;
		fillRect(x, yu, w, hh, color);
	}
}

void Painter::drawRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint32_t fill_color, uint32_t stroke_color) {
	if (w <= 0 || h <= 0)
		return;
	
	if ((fill_color & 0xFF000000) != 0) {
		startPerfectDrawing(fill_color);
		fillRoundedRect(x, y, w, h, x_radius, y_radius, fill_color);
		stopPerfectDrawing();
	}
	
	if ((stroke_color & 0xFF000000) != 0 && stroke_color != fill_color) {
		startPerfectDrawing(stroke_color);
		strokeRoundedRect(x, y, w, h, x_radius, y_radius, stroke_color);
		stopPerfectDrawing();
	}
}

/*
 * Triangle
 * */
void Painter::strokeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x3, y3, x1, y1, color);
}

void Painter::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    if (y2 < y1) {
		std::swap(x2, x1);
		std::swap(y2, y1);
	}
    
    if (y3 < y1) {
		std::swap(x3, x1);
		std::swap(y3, y1);
	}
	
    if (y3 < y2) {
		std::swap(x3, x2);
		std::swap(y3, y2);
	}
	
	std::vector<std::pair<int, int>> points;
	getLinePoints(points, x1, y1, x2, y2);
	getLinePoints(points, x2, y2, x3, y3);
	getLinePoints(points, x3, y3, x1, y1);
	
	std::vector<int> min_x(y3 - y1 + 1, -1);
	std::vector<int> max_x(y3 - y1 + 1, 0);
	
	for (auto it: points) {
		int key = it.second - y1;
		
		if (min_x[key] == -1 || min_x[key] > it.first)
			min_x[key] = it.first;
		
		if (max_x[key] < it.first)
			max_x[key] = it.first;
	}
	
	for (int i = 0; i < min_x.size(); i++) {
		if (min_x[i] != -1)
			drawHLine(min_x[i], y1 + i, max_x[i] - min_x[i] + 1, color);
	}
}

void Painter::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t fill_color, uint32_t stroke_color) {
	if ((fill_color & 0xFF000000) != 0) {
		startPerfectDrawing(fill_color);
		fillTriangle(x1, y1, x2, y2, x3, y3, fill_color);
		stopPerfectDrawing();
	}
	
	if ((stroke_color & 0xFF000000) != 0 && stroke_color != fill_color) {
		startPerfectDrawing(stroke_color);
		strokeTriangle(x1, y1, x2, y2, x3, y3, stroke_color);
		stopPerfectDrawing();
	}
}

/*
 * Helpers
 * */

// From u8g2
void Painter::strokeCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color) {
	/* upper right */
	if ((option & CIRCLE_DRAW_UPPER_RIGHT)) {
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 + y, y0 - x, color);
	}
	
	/* upper left */
	if ((option & CIRCLE_DRAW_UPPER_LEFT)) {
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 - y, y0 - x, color);
	}
	
	/* lower right */
	if ((option & CIRCLE_DRAW_LOWER_RIGHT)) {
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 + y, y0 + x, color);
	}
	
	/* lower left */
	if ((option & CIRCLE_DRAW_LOWER_LEFT)) {
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 - y, y0 + x, color);
	}
}

// From u8g2
void Painter::strokeCircleHelper(int x0, int y0, int rad, uint8_t option, uint32_t color) {
	int f;
	int ddF_x;
	int ddF_y;
	int x;
	int y;
	
	f = 1;
	f -= rad;
	ddF_x = 1;
	ddF_y = 0;
	ddF_y -= rad;
	ddF_y *= 2;
	x = 0;
	y = rad;
	
	strokeCircleSectionHelper(x, y, x0, y0, option, color);
	
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		strokeCircleSectionHelper(x, y, x0, y0, option, color);    
	}
}

// From u8g2
void Painter::fillCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color) {
	/* upper right */
	if ((option & CIRCLE_DRAW_UPPER_RIGHT)) {
		drawVLine(x0 + x, y0 - y, y + 1, color);
		drawVLine(x0 + y, y0 - x, x + 1, color);
	}
	
	/* upper left */
	if ((option & CIRCLE_DRAW_UPPER_LEFT)) {
		drawVLine(x0 - x, y0 - y, y + 1, color);
		drawVLine(x0 - y, y0 - x, x + 1, color);
	}

	/* lower right */
	if ((option & CIRCLE_DRAW_LOWER_RIGHT)) {
		drawVLine(x0 + x, y0, y + 1, color);
		drawVLine(x0 + y, y0, x + 1, color);
	}

	/* lower left */
	if ((option & CIRCLE_DRAW_LOWER_LEFT)) {
		drawVLine(x0 - x, y0, y + 1, color);
		drawVLine(x0 - y, y0, x + 1, color);
	}
}

// From u8g2
void Painter::fillCircleHelper(int x0, int y0, int rad, uint8_t option, uint32_t color) {
	int f;
	int ddF_x;
	int ddF_y;
	int x;
	int y;

	f = 1;
	f -= rad;
	ddF_x = 1;
	ddF_y = 0;
	ddF_y -= rad;
	ddF_y *= 2;
	x = 0;
	y = rad;

	fillCircleSectionHelper(x, y, x0, y0, option, color);
	
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		fillCircleSectionHelper(x, y, x0, y0, option, color);
	}
}

// From u8g2
void Painter::strokeEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color, int start, int end) {
	if (start == -1) { // fast
		/* upper right */
		if ((option & CIRCLE_DRAW_UPPER_RIGHT))
			drawPixel(x0 + x, y0 - y, color);
		
		/* upper left */
		if ((option & CIRCLE_DRAW_UPPER_LEFT))
			drawPixel(x0 - x, y0 - y, color);
		
		/* lower left */
		if ((option & CIRCLE_DRAW_LOWER_LEFT))
			drawPixel(x0 - x, y0 + y, color);
		
		/* lower right */
		if ((option & CIRCLE_DRAW_LOWER_RIGHT))
			drawPixel(x0 + x, y0 + y, color);
	} else { // slow
		/* upper right */
		if ((option & CIRCLE_DRAW_UPPER_RIGHT) && isInEllipseRange(x, y, start, end))
			drawPixel(x0 + x, y0 - y, color);
		
		/* upper left */
		if ((option & CIRCLE_DRAW_UPPER_LEFT) && isInEllipseRange(-x, y, start, end))
			drawPixel(x0 - x, y0 - y, color);
		
		/* lower left */
		if ((option & CIRCLE_DRAW_LOWER_LEFT) && isInEllipseRange(-x, -y, start, end))
			drawPixel(x0 - x, y0 + y, color);
		
		/* lower right */
		if ((option & CIRCLE_DRAW_LOWER_RIGHT) && isInEllipseRange(x, -y, start, end))
			drawPixel(x0 + x, y0 + y, color);
	}
}

// From u8g2
void Painter::strokeEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint32_t color, int start, int end) {
	int64_t rxrx2 = rx * rx * 2;
	int64_t ryry2 = ry * ry * 2;
	
	int x = rx;
	int y = 0;
	
	int64_t xchg = (1 - rx - rx) * ry * ry;
	int64_t ychg = rx * rx;
	
	int64_t err = 0;
	
	int64_t stopx = ryry2 * rx;
	int64_t stopy = 0;
	
	while (stopx >= stopy) {
		strokeEllipseSectionHelper(x, y, x0, y0, option, color, start, end);
		
		y++;
		stopy += rxrx2;
		err += ychg;
		ychg += rxrx2;
		
		if (2 * err + xchg > 0) {
			x--;
			stopx -= ryry2;
			err += xchg;
			xchg += ryry2;      
		}
	}
	
	x = 0;
	y = ry;
	
	xchg = ry * ry;
	ychg = (1 - ry - ry) * rx * rx;
	
	err = 0;
	
	stopx = 0;
	stopy = rxrx2 * ry;
	
	while (stopx <= stopy) {
		strokeEllipseSectionHelper(x, y, x0, y0, option, color, start, end);
		
		x++;
		stopx += ryry2;
		err += xchg;
		xchg += ryry2;
		
		if (2 * err + ychg > 0) {
			y--;
			stopy -= rxrx2;
			err += ychg;
			ychg += rxrx2;
		}
	}
}

// From u8g2
void Painter::fillEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color, int start, int end) {
	if (start == -1) { // fast
		/* upper right */
		if ((option & CIRCLE_DRAW_UPPER_RIGHT))
			drawVLine(x0 + x, y0 - y, y + 1, color);
		
		/* upper left */
		if ((option & CIRCLE_DRAW_UPPER_LEFT))
			drawVLine(x0 - x, y0 - y, y + 1, color);
		
		/* lower left */
		if ((option & CIRCLE_DRAW_LOWER_LEFT))
			drawVLine(x0 - x, y0, y + 1, color);
		
		/* lower right */
		if ((option & CIRCLE_DRAW_LOWER_RIGHT))
			drawVLine(x0 + x, y0, y + 1, color);
	} else { // slow
		drawPixel(x0, y0, color);
		
		for (int i = 0; i < y + 1; i++) {
			/* upper right */
			if ((option & CIRCLE_DRAW_UPPER_RIGHT) && isInEllipseRange(x, y - i, start, end))
				drawPixel(x0 + x, y0 - y + i, color);
			
			/* upper left */
			if ((option & CIRCLE_DRAW_UPPER_LEFT) && isInEllipseRange(-x, y - i, start, end))
				drawPixel(x0 - x, y0 - y + i, color);
			
			/* lower left */
			if ((option & CIRCLE_DRAW_LOWER_LEFT) && isInEllipseRange(-x, -y + i, start, end))
				drawPixel(x0 - x, y0 + y - i, color);
			
			/* lower right */
			if ((option & CIRCLE_DRAW_LOWER_RIGHT) && isInEllipseRange(x, -y + i, start, end))
				drawPixel(x0 + x, y0 + y - i, color);
		}
	}
}

// From u8g2
void Painter::fillEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint32_t color, int start, int end) {
	int64_t rxrx2 = rx * rx * 2;
	int64_t ryry2 = ry * ry * 2;
	
	int x = rx;
	int y = 0;
	
	int64_t xchg = (1 - rx - rx) * ry * ry;
	int64_t ychg = rx * rx;
	
	int64_t err = 0;
	
	int64_t stopx = ryry2 * rx;
	int64_t stopy = 0;
	
	while (stopx >= stopy) {
		fillEllipseSectionHelper(x, y, x0, y0, option, color, start, end);
		y++;
		stopy += rxrx2;
		err += ychg;
		ychg += rxrx2;
		if (2 * err + xchg > 0) {
			x--;
			stopx -= ryry2;
			err += xchg;
			xchg += ryry2;      
		}
	}

	x = 0;
	y = ry;
	
	xchg = ry * ry;
	
	ychg = (1 - ry - ry) * rx * rx;
	
	err = 0;
	
	stopx = 0;
	stopy = rxrx2 * ry;
	
	while (stopx <= stopy) {
		fillEllipseSectionHelper(x, y, x0, y0, option, color, start, end);
		x++;
		stopx += ryry2;
		err += xchg;
		xchg += ryry2;
		
		if (2 * err + ychg > 0) {
			y--;
			stopy -= rxrx2;
			err += ychg;
			ychg += rxrx2;
		}
	}
}

/*
 * Utils
 * */
bool Painter::isInEllipseRange(int x, int y, int start, int end) {
	int angle = round(atan2(y, x) * (180 / M_PI));
	if (angle < 0)
		angle = 360 + angle;
	return angle >= start && angle <= end;
}

std::tuple<int, int> Painter::getEllipseSectionRegion(int x1, int x2, int y1, int y2) {
	int from = std::max(x1, y1);
	int to = std::min(x2, y2);
	if (from < to)
		return { from, to };
	
	if (x2 > 360) {
		x1 = 0;
		x2 = x2 % 360;
		return getEllipseSectionRegion(x1, x2, y1, y2);
	}
	
	return { -1, -1 };
}

Painter::~Painter() {
	
}
