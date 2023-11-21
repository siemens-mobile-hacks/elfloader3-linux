#include "Painter.h"

#include <cmath>
#include <new>
#include <fstream>
#include <ostream>

#pragma pack(push, 1) 
struct BMPHeader {
	char signature[2];
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t dataOffset;
	uint32_t headerSize;
	int width;
	int height;
	uint16_t planes;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t dataSize;
	int horizontalResolution;
	int verticalResolution;
	uint32_t colorsUsed;
	uint32_t importantColors;
	// BI_BITFIELDS
	uint32_t mask[4];
	uint32_t colorSpaceType;
	uint32_t colorSpace[0x24];
	uint32_t gamma[3];
};
#pragma pack(pop)

static void _writeBMP(const char *filename, const uint32_t *pixels, int width, int height);

Painter::Painter(int width, int height) {
	m_width = width;
	m_height = height;
	m_buffer.resize(m_width * m_height);
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
	std::fill(m_buffer.begin(), m_buffer.end(), color);
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
 * Bitmap
 * */
void Painter::drawBitmap(int x, int y, int w, int h, uint8_t *bitmap, Bitmap::Type type, int offset_x, int offset_y) {
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
			drawPixel(x + img_x - offset_x, y + img_y - offset_y, color);
		}
	}
}

/*
 * Lines
 * */
void Painter::drawPixel(int x, int y, uint32_t color) {
	x += m_window_x;
	y += m_window_y;
	
	if (x < 0 || y < 0 || x > m_window_x2 || y > m_window_y2) {
		// printf("ignored pixel %d x %d\n", x, y);
		return;
	}
	
	uint32_t index = (m_height - y - 1) * m_width + x;
	uint32_t old_color = m_buffer[index];
	
	if (m_perfect_drawing) {
		if (m_mask[index])
			return;
		m_mask[index] = 1;
	}
	
	m_buffer[index] = blendColors(old_color, color);
}

void Painter::drawHLine(int x, int y, int width, uint32_t color) {
	for (int i = 0; i < width; i++)
		drawPixel(x + i, y, color);
}

void Painter::drawVLine(int x, int y, int height, uint32_t color) {
	for (int i = 0; i < height; i++)
		drawPixel(x, y + i, color);
}

// From u8g2
void Painter::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
	int tmp;
	int x,y;
	int dx, dy;
	int err;
	int ystep;
	
	uint8_t swapxy = 0;
	
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
	
	if (!dy) {
		drawHLine(x1, y1, dx, color);
		return;
	} else if (!dx) {
		drawVLine(x1, y1, dy, color);
		return;
	}
	
	if (dy > dx) {
		swapxy = 1;
		tmp = dx; dx = dy; dy = tmp;
		tmp = x1; x1 = y1; y1 = tmp;
		tmp = x2; x2 = y2; y2 = tmp;
	}
	
	if (x1 > x2) {
		tmp = x1; x1 =x2; x2 = tmp;
		tmp = y1; y1 =y2; y2 = tmp;
	}
	
	err = dx >> 1;
	
	if (y2 > y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	
	y = y1;
	
	if (x2 == 0xffff)
		x2--;
	
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
	
	if ((fill_color & 0xFF000000) != 0 && stroke_color != fill_color) {
		startPerfectDrawing(stroke_color);
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
	// Sort vertices based on y-coordinate
	if (y1 > y2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	
	if (y1 > y3) {
		std::swap(x1, x3);
		std::swap(y1, y3);
	}
	
	if (y2 > y3) {
		std::swap(x2, x3);
		std::swap(y2, y3);
	}
	
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x3, y3, x1, y1, color);
}

// By ChatGPT, lol
void Painter::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
	// Sort vertices based on y-coordinate
	if (y1 > y2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	
	if (y1 > y3) {
		std::swap(x1, x3);
		std::swap(y1, y3);
	}
	
	if (y2 > y3) {
		std::swap(x2, x3);
		std::swap(y2, y3);
	}
	
	// Draw upper part of the triangle
	for (int scanlineY = y1; scanlineY <= y2; ++scanlineY) {
		int xL = x1 + ((x2 - x1) * (scanlineY - y1)) / (y2 - y1);
		int xR = x1 + ((x3 - x1) * (scanlineY - y1)) / (y3 - y1);
		for (int x = xL; x <= xR; ++x)
			drawPixel(x, scanlineY, color);
	}
	
	// Draw lower part of the triangle
	for (int scanlineY = y2 + 1; scanlineY <= y3; ++scanlineY) {
		int xL = x2 + ((x3 - x2) * (scanlineY - y2)) / (y3 - y2);
		int xR = x1 + ((x3 - x1) * (scanlineY - y1)) / (y3 - y1);
		for (int x = xL; x <= xR; ++x)
			drawPixel(x, scanlineY, color);
	}
}

void Painter::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t fill_color, uint32_t stroke_color) {
	if ((fill_color & 0xFF000000) != 0) {
		startPerfectDrawing(fill_color);
		strokeTriangle(x1, y1, x2, y2, x3, y3, fill_color);
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

void Painter::save() {
	_writeBMP("/tmp/sie.bmp", &m_buffer[0], m_width, m_height);
}

static void _writeBMP(const char *filename, const uint32_t *pixels, int width, int height) {
	BMPHeader header = {};
	header.signature[0] = 'B';
	header.signature[1] = 'M';
	header.headerSize = 40;
	header.width = width;
	header.height = height;
	header.planes = 1;
	header.bitsPerPixel = 32;
	header.compression = 3;
	header.dataOffset = sizeof(BMPHeader);
	header.dataSize = 4 * width * height;
	header.fileSize = sizeof(BMPHeader) + header.dataSize;
	header.horizontalResolution = 0;
	header.verticalResolution = 0;
	header.colorsUsed = 0;
	header.importantColors = 0;
	
	// BI_BITFIELDS
	header.mask[0] = 0x00FF0000;
	header.mask[1] = 0x0000FF00;
	header.mask[2] = 0x000000FF;
	header.mask[3] = 0xFF000000;
	header.colorSpaceType = 0x57696E20;
	
	std::ofstream file(filename, std::ios::binary);
	if (file) {
		file.write(reinterpret_cast<const char *>(&header), sizeof(BMPHeader));
		file.write(reinterpret_cast<const char *>(pixels), header.dataSize);
		file.close();
	}
}

Painter::~Painter() {
	
}
