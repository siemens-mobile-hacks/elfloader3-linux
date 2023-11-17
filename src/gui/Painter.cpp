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
	uint8_t colorSpace[0x24];
	uint32_t gamma[3];
};
#pragma pack(pop)

static void _writeBMP(const char *filename, const uint32_t *pixels, int width, int height);

Painter::Painter(int width, int height) {
	m_width = width;
	m_height = height;
	m_buffer = new uint32_t[m_width * m_height];
	clear(0xFFFFFFFF);
}

void Painter::clear(uint32_t color) {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++)
			drawPixel(x, y, color);
	}
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

void Painter::drawPixel(int x, int y, uint32_t color) {
	if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
		printf("ignored pixel %d x %d\n", x, y);
		return;
	}
	
	uint32_t index = (m_height - y - 1) * m_width + x;
	uint32_t old_color = m_buffer[index];
	
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

void Painter::drawMask(const uint8_t *mask, int x, int y, int w, int h, const uint32_t *colors) {
	for (int mask_y = 0; mask_y < h; mask_y++) {
		int line_start = mask_y * w;
		for (int mask_x = 0; mask_x < w; mask_x++) {
			uint8_t pixel = mask[line_start + mask_x];
			if (pixel)
				drawPixel(x + mask_x, y + mask_y, colors[pixel]);
		}
	}
}

void Painter::drawRect(int x, int y, int x2, int y2, uint32_t fill_color, uint32_t stroke_color) {
	int w = x2 - x + 1;
	int h = y2 - y + 1;
	
	if (w <= 0 || h <= 0)
		return;
	
	bool is_fill_transparent = (fill_color & 0xFF000000) == 0;
	bool is_stroke_transparent = (stroke_color & 0xFF000000) == 0;
	
	if (is_fill_transparent && is_stroke_transparent)
		return;
	
	if (!is_stroke_transparent)
		drawHLine(x, y, w, stroke_color);
	
	if (h >= 2) {
		if (!is_stroke_transparent) {
			drawHLine(x, y + h - 1, w, stroke_color);
			drawVLine(x, y + 1, h - 2, stroke_color);
			drawVLine(x + w - 1, y + 1, h - 2, stroke_color);
		}
		
		if (!is_fill_transparent) {
			for (int i = 0; i < h - 2; i++)
				drawHLine(x + 1, y + 1 + i, w - 2, fill_color);
		}
	}
}

void Painter::drawRoundedRect(int x, int y, int x2, int y2, int x_radius, int y_radius, uint32_t fill_color, uint32_t stroke_color) {
	int w = x2 - x + 1;
	int h = y2 - y + 1;
	
	if (w <= 0 || h <= 0)
		return;
	
	bool is_fill_transparent = (fill_color & 0xFF000000) == 0;
	bool is_stroke_transparent = (stroke_color & 0xFF000000) == 0;
	
	if (is_fill_transparent && is_stroke_transparent)
		return;
	
	m_mask.setCanvasSize(w, h);
	
	if (!is_fill_transparent)
		m_mask.fillRoundedRect(0, 0, w, h, x_radius, y_radius, 1);
	
	if (!is_stroke_transparent)
		m_mask.drawRoundedRect(0, 0, w, h, x_radius, y_radius, 2);
	
	uint32_t colors[] = { 0, fill_color, stroke_color };
	drawMask(m_mask.data(), x, y, m_mask.width(), m_mask.height(), colors);
}

void Painter::drawArc(int x, int y, int x2, int y2, int start, int end, uint32_t fill_color, uint32_t stroke_color) {
	int w = x2 - x + 1;
	int h = y2 - y + 1;
	
	if (w <= 0 || h <= 0)
		return;
	
	uint32_t colors[] = { 0, fill_color, stroke_color };
	
	m_mask.setCanvasSize(w, h);
	m_mask.fillArc(0, 0, w, h, start, end, 1);
	drawMask(m_mask.data(), x, y, m_mask.width(), m_mask.height(), colors);
	
	m_mask.setCanvasSize(w, h);
	m_mask.drawArc(0, 0, w, h, start, end, 2);
	drawMask(m_mask.data(), x, y, m_mask.width(), m_mask.height(), colors);
}

// From libgd2
void Painter::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int wid;
	int w, wstart;
	int thick = 1;
	
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	if (dx == 0) {
		drawVLine(x1, y1, dy + 1, color);
		return;
	} else if (dy == 0) {
		drawHLine(x1, y1, dx + 1, color);
		return;
	}

	if (dy <= dx) {
		/* More-or-less horizontal. use wid for vertical stroke */
		/* Doug Claar: watch out for NaN in atan2 (2.0.5) */

		/* 2.0.12: Michael Schwartz: divide rather than multiply;
			  TBB: but watch out for /0! */
		double ac = cos(atan2(dy, dx));
		if (ac != 0) {
			wid = thick / ac;
		} else {
			wid = 1;
		}
		if (wid == 0) {
			wid = 1;
		}
		d = 2 * dy - dx;
		incr1 = 2 * dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}

		/* Set up line thickness */
		wstart = y - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			drawPixel(x, w, color);

		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y++;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPixel(x, w, color);
			}
		} else {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y--;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPixel(x, w, color);
			}
		}
	} else {
		/* More-or-less vertical. use wid for horizontal stroke */
		/* 2.0.12: Michael Schwartz: divide rather than multiply;
		   TBB: but watch out for /0! */
		double as = sin(atan2(dy, dx));
		if (as != 0) {
			wid = thick / as;
		} else {
			wid = 1;
		}
		if (wid == 0)
			wid = 1;

		d = 2 * dx - dy;
		incr1 = 2 * dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}

		/* Set up line thickness */
		wstart = x - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			drawPixel(w, y, color);

		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x++;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPixel(w, y, color);
			}
		} else {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x--;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPixel(w, y, color);
			}
		}
	}
}

void Painter::save() {
	_writeBMP("/tmp/sie.bmp", m_buffer, m_width, m_height);
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
	
	printf("BMPHeader %d\n", sizeof(BMPHeader));
	
	std::ofstream file(filename, std::ios::binary);
	if (file) {
		file.write(reinterpret_cast<const char *>(&header), sizeof(BMPHeader));
		file.write(reinterpret_cast<const char *>(pixels), header.dataSize);
		file.close();
	}
}

Painter::~Painter() {
	delete m_buffer;
}
