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
	m_tmp_buffer = new uint32_t[m_width * m_height];
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
	
	//printf("drawPixel(%d, %d, %08X)\n", x, y, color);
	
	if (m_buffer[(m_height - y - 1) * m_width + x] != 0xFFFF00FF && color != 0xFFFF00FF)
		color = 0xFF0000FF;
	
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

// From u8g2
void Painter::drawEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color) {
	/* upper right */
	if ((option & CIRCLE_DRAW_UPPER_RIGHT))
		drawPixel(x0 + x, y0 - y, color);
	
	/* upper left */
	if ((option & CIRCLE_DRAW_UPPER_LEFT))
		drawPixel(x0 - x, y0 - y, color);
	
	/* lower right */
	if ((option & CIRCLE_DRAW_LOWER_RIGHT))
		drawPixel(x0 + x, y0 + y, color);
	
	/* lower left */
	if ((option & CIRCLE_DRAW_LOWER_LEFT))
		drawPixel(x0 - x, y0 + y, color);
}

// From u8g2
void Painter::drawEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint32_t color) {
	int x, y;
	int64_t xchg, ychg;
	int64_t err;
	int64_t rxrx2;
	int64_t ryry2;
	int64_t stopx, stopy;
	
	rxrx2 = rx;
	rxrx2 *= rx;
	rxrx2 *= 2;
	
	ryry2 = ry;
	ryry2 *= ry;
	ryry2 *= 2;
	
	x = rx;
	y = 0;
	
	xchg = 1;
	xchg -= rx;
	xchg -= rx;
	xchg *= ry;
	xchg *= ry;
	
	ychg = rx;
	ychg *= rx;
	
	err = 0;
	
	stopx = ryry2;
	stopx *= rx;
	stopy = 0;
	
	while (stopx >= stopy) {
		drawEllipseSectionHelper(x, y, x0, y0, option, color);
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
	
	xchg = ry;
	xchg *= ry;
	
	ychg = 1;
	ychg -= ry;
	ychg -= ry;
	ychg *= rx;
	ychg *= rx;
	
	err = 0;
	
	stopx = 0;
	
	stopy = rxrx2;
	stopy *= ry;
	
	while (stopx <= stopy) {
		drawEllipseSectionHelper(x, y, x0, y0, option, color);
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
void Painter::drawCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint32_t color) {
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
void Painter::drawCircleHelper(int x0, int y0, int rad, uint8_t option, uint32_t color) {
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
	
	drawCircleSectionHelper(x, y, x0, y0, option, color);
	
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		drawCircleSectionHelper(x, y, x0, y0, option, color);    
	}
}

// From u8g2
void Painter::drawRect(int x, int y, int x2, int y2, uint32_t fill_color, uint32_t stroke_color) {
	int w = x2 - x + 1;
	int h = y2 - y + 1;
	
	int xtmp = x;
	
	drawHLine(x, y, w, stroke_color);
	
	if (h >= 2) {
		h -= 2;
		y++;
		
		if (h > 0) {
			drawVLine(x, y, h, stroke_color);
			x += w;
			x--;
			drawVLine(x, y, h, stroke_color);
			y += h;
		}
		
		drawHLine(xtmp, y, w, stroke_color);
	}
}

// From u8g2
void Painter::drawRoundedRect(int x, int y, int x2, int y2, int x_radius, int y_radius, uint32_t fill_color, uint32_t stroke_color) {
	int xl, yu;
	
	if (x_radius <= 0 || y_radius <= 0) {
		drawRect(x, y, x2, y2, fill_color, stroke_color);
		return;
	}
	
	printf("drawRoundedRect %d %d %d %d\n", x, y, x2, y2);
	
	int w = x2 - x + 1;
	int h = y2 - y + 1;
	
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
	
	xl = x;
	xl += x_radius;
	yu = y;
	yu += y_radius;
	
	int yl, xr;
	
	xr = x;
	xr += w;
	xr -= x_radius;
	xr -= 1;
	
	yl = y;
	yl += h;
	yl -= y_radius; 
	yl -= 1;
	
	if (!x_radius || !y_radius) {
		drawCircleHelper(xl, yu, 0, CIRCLE_DRAW_UPPER_LEFT, stroke_color);
		drawCircleHelper(xr, yu, 0, CIRCLE_DRAW_UPPER_RIGHT, stroke_color);
		drawCircleHelper(xl, yl, 0, CIRCLE_DRAW_LOWER_LEFT, stroke_color);
		drawCircleHelper(xr, yl, 0, CIRCLE_DRAW_LOWER_RIGHT, stroke_color);
	} else {
		drawEllipseHelper(xl, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_LEFT, stroke_color);
		drawEllipseHelper(xr, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_RIGHT, stroke_color);
		drawEllipseHelper(xl, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_LEFT, stroke_color);
		drawEllipseHelper(xr, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_RIGHT, stroke_color);
	}
	
	int ww, hh;

	ww = w;
	ww -= x_radius;
	ww -= x_radius;
	hh = h;
	hh -= y_radius;
	hh -= y_radius;

	xl++;
	yu++;

	if (ww >= 3) {
		ww -= 2;
		h--;
		drawHLine(xl, y, ww, stroke_color);
		drawHLine(xl, y + h, ww, stroke_color);
	}

	if (hh >= 3) {
		hh -= 2;
		w--;
		drawVLine(x, yu, hh, stroke_color);
		drawVLine(x + w, yu, hh, stroke_color);
	}
}

void Painter::save() {
	_writeBMP("/tmp/sie.bmp", m_buffer, m_width, m_height);
}

Painter::~Painter() {
	delete m_buffer;
	delete m_tmp_buffer;
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
