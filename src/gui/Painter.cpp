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
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t colorsUsed;
    uint32_t importantColors;
};
#pragma pack(pop)

static void _writeBMP(const char *filename, const uint32_t *pixels, int width, int height);

Painter::Painter(int width, int height) {
	m_width = width;
	m_height = height;
	m_buffer = new uint32_t[m_width * m_height];
	clear(0xFFFFFFFF);
}

void Painter::drawPixel(int x, int y, uint32_t color) {
	m_buffer[y * m_width + x] = color; // TODO: alpha!
}

void Painter::clear(uint32_t color) {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++)
			drawPixel(x, y, color);
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

// From Arduino_GFX
void Painter::drawRoundedRect(int x1, int y1, int x2, int y2, int x_radius, int y_radius, int stroke_size, const color_t &stroke_color, const color_t &fill) {
	int w = x1 - x2 + 1;
	int h = y1 - y2 + 1;
	
	int max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
	r = std::min(r, max_radius);
	
	// smarter version
	writeFastHLine(x + r, y, w - 2 * r, stroke_color);			// Top
	writeFastHLine(x + r, y + h - 1, w - 2 * r, stroke_color);	// Bottom
	writeFastVLine(x, y + r, h - 2 * r, stroke_color);			// Left
	writeFastVLine(x + w - 1, y + r, h - 2 * r, stroke_color);	// Right
	
	// draw four corners
	writeEllipseHelper(x + r, y + r, r, r, 1, color);
	writeEllipseHelper(x + w - r - 1, y + r, r, r, 2, color);
	writeEllipseHelper(x + w - r - 1, y + h - r - 1, r, r, 4, color);
	writeEllipseHelper(x + r, y + h - r - 1, r, r, 8, color);
}

// From Arduino_GFX
void Painter::writeEllipseHelper(int32_t x, int32_t y, int32_t rx, int32_t ry, uint8_t cornername, const color_t color) {
	if (rx < 0 || ry < 0 || ((rx == 0) && (ry == 0))) {
		return;
	}
	
	if (ry == 0) {
		writeFastHLine(x - rx, y, (ry << 2) + 1, color);
		return;
	}
	
	if (rx == 0) {
		writeFastVLine(x, y - ry, (rx << 2) + 1, color);
		return;
	}
	
	int32_t xt, yt, s, i;
	int32_t rx2 = rx * rx;
	int32_t ry2 = ry * ry;
	
	i = -1;
	xt = 0;
	yt = ry;
	s = (ry2 << 1) + rx2 * (1 - (ry << 1));
	
	do {
		while (s < 0)
			s += ry2 * ((++xt << 2) + 2);
		if (cornername & 0x1) {
			writeFastHLine(x - xt, y - yt, xt - i, color);
		}
		if (cornername & 0x2) {
			writeFastHLine(x + i + 1, y - yt, xt - i, color);
		}
		if (cornername & 0x4) {
			writeFastHLine(x + i + 1, y + yt, xt - i, color);
		}
		if (cornername & 0x8) {
			writeFastHLine(x - xt, y + yt, xt - i, color);
		}
		i = xt;
		s -= (--yt) * rx2 << 2;
	} while (ry2 * xt <= rx2 * yt);
	
	i = -1;
	yt = 0;
	xt = rx;
	s = (rx2 << 1) + ry2 * (1 - (rx << 1));
	
	do {
		while (s < 0)
			s += rx2 * ((++yt << 2) + 2);
		if (cornername & 0x1) {
			writeFastVLine(x - xt, y - yt, yt - i, color);
		}
		if (cornername & 0x2) {
			writeFastVLine(x + xt, y - yt, yt - i, color);
		}
		if (cornername & 0x4) {
			writeFastVLine(x + xt, y + i + 1, yt - i, color);
		}
		if (cornername & 0x8) {
			writeFastVLine(x - xt, y + i + 1, yt - i, color);
		}
		i = yt;
		s -= (--xt) * ry2 << 2;
	} while (rx2 * yt <= ry2 * xt);
}


	
	
	m_canvas->drawRoundedRect<blendmode::Normal, porterduff::FastSourceOverOnOpaque, false>(stroke_color, bg, x1, y1, x2, y2, x_radius, stroke_size);
}

void Painter::drawRoundedRectDotted(int x1, int y1, int x2, int y2, int x_radius, int y_radius, int stroke_size, const Color24 &bg, const Color24 &stroke_color) {
	m_canvas->drawRoundedRect<>(stroke_color, bg, x1, y1, x2, y2, x_radius, stroke_size);
}

void Painter::save() {
	_writeBMP("/tmp/sie.bmp", m_canvas->pixels(), m_canvas->width(), m_canvas->height());
	
}

Painter::~Painter() {
	delete m_canvas;
}

static void _writeBMP(const char *filename, const uint32_t *pixels, int width, int height) {
	BMPHeader header = {};
	header.signature[0] = 'B';
	header.signature[1] = 'M';
	header.headerSize = 40;
	header.width = width;
	header.height = height;
	header.planes = 1;
	header.bitsPerPixel = 24;
	header.compression = 0;
	header.dataSize = 3 * width * height;
	header.fileSize = sizeof(BMPHeader) + header.dataSize;
	header.horizontalResolution = 0;
	header.verticalResolution = 0;
	header.colorsUsed = 0;
	header.importantColors = 0;
	
	std::ofstream file(filename, std::ios::binary);
	if (file) {
		file.write(reinterpret_cast<const char *>(&header), sizeof(BMPHeader));
		for (size_t i = 0; i < (width * height); i++) {
			uint8_t r = (pixels[i] >> 16) & 0xFF;
			uint8_t g = (pixels[i] >> 8) & 0xFF;
			uint8_t b = pixels[i] & 0xFF;
			file.put(r);
			file.put(g);
			file.put(b);
		}
		file.close();
	}
}
