#include "Painter.h"

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
	clear(0xFFFF00FF);
}

void Painter::clear(uint32_t color) {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++)
			drawPixel(x, y, color);
	}
}

void Painter::drawPixel(int x, int y, uint32_t color) {
	if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
		printf("ignored pixel %d x %d\n", x, y);
		return;
	}
	m_buffer[(m_height - y - 1) * m_width + x] = color; // TODO: alpha!
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

void Painter::save() {
	_writeBMP("/tmp/sie.bmp", m_buffer, m_width, m_height);
}

Painter::~Painter() {
	delete m_buffer;
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

// From u8g2
void Painter::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
	if (x1 == x2) {
		drawVLine(x1, y1, y2 - y1 + 1, color);
		return;
	} else if (y1 == y2) {
		drawHLine(x1, y1, x2 - x1 + 1, color);
		return;
	}
	
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
