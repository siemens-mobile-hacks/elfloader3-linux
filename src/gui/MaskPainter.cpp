#include "MaskPainter.h"

#include <algorithm>

void MaskPainter::setCanvasSize(int width, int height) {
	m_width = width;
	m_height = height;
	m_buffer.resize(m_width * m_height);
	std::fill(m_buffer.begin(), m_buffer.end(), 0);
}

void MaskPainter::drawPixel(int x, int y, uint8_t color) {
	if (x < 0 || y < 0 || x >= m_width || y >= m_height)
		return;
	m_buffer[y * m_width + x] = color;
}

void MaskPainter::drawHLine(int x, int y, int width, uint8_t color) {
	for (int i = 0; i < width; i++)
		drawPixel(x + i, y, color);
}

void MaskPainter::drawVLine(int x, int y, int height, uint8_t color) {
	for (int i = 0; i < height; i++)
		drawPixel(x, y + i, color);
}

// From u8g2
void MaskPainter::drawRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint8_t color) {
	int xl, yu;
	
	if (x_radius <= 0 || y_radius <= 0) {
		drawRect(x, y, w, h, color);
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
		drawCircleHelper(xl, yu, 0, CIRCLE_DRAW_UPPER_LEFT, color);
		drawCircleHelper(xr, yu, 0, CIRCLE_DRAW_UPPER_RIGHT, color);
		drawCircleHelper(xl, yl, 0, CIRCLE_DRAW_LOWER_LEFT, color);
		drawCircleHelper(xr, yl, 0, CIRCLE_DRAW_LOWER_RIGHT, color);
	} else {
		drawEllipseHelper(xl, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_LEFT, color);
		drawEllipseHelper(xr, yu, x_radius, y_radius, CIRCLE_DRAW_UPPER_RIGHT, color);
		drawEllipseHelper(xl, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_LEFT, color);
		drawEllipseHelper(xr, yl, x_radius, y_radius, CIRCLE_DRAW_LOWER_RIGHT, color);
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

// From u8g2
void MaskPainter::fillRoundedRect(int x, int y, int w, int h, int x_radius, int y_radius, uint8_t color) {
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
	
	int xl, yu;
	int yl, xr;
	
	xl = x;
	xl += x_radius;
	yu = y;
	yu += y_radius;
	
	xr = x;
	xr += w;
	xr -= x_radius;
	xr -= 1;
	
	yl = y;
	yl += h;
	yl -= y_radius; 
	yl -= 1;
	
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
	
	int ww, hh;
	
	ww = w;
	ww -= x_radius;
	ww -= x_radius;
	xl++;
	yu++;
	
	if (ww >= 3) {
		ww -= 2;
		fillRect(xl, y, ww, y_radius + 1, color);
		fillRect(xl, yl, ww, y_radius + 1, color);
	}
	
	hh = h;
	hh -= y_radius;
	hh -= y_radius;
	
	//h--;
	
	if (hh >= 3) {
		hh -= 2;
		fillRect(x, yu, w, hh, color);
	}
}

// From u8g2
void MaskPainter::drawCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color) {
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
void MaskPainter::drawCircleHelper(int x0, int y0, int rad, uint8_t option, uint8_t color) {
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
void MaskPainter::fillCircleSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color) {
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
void MaskPainter::fillCircleHelper(int x0, int y0, int rad, uint8_t option, uint8_t color) {
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
void MaskPainter::drawEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color) {
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
void MaskPainter::drawEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint8_t color) {
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
void MaskPainter::fillEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color) {
	/* upper right */
	if ((option & CIRCLE_DRAW_UPPER_RIGHT))
		drawVLine(x0 + x, y0 - y, y + 1, color);
	
	/* upper left */
	if ((option & CIRCLE_DRAW_UPPER_LEFT))
		drawVLine(x0 - x, y0 - y, y + 1, color);
	
	/* lower right */
	if ((option & CIRCLE_DRAW_LOWER_RIGHT))
		drawVLine(x0 + x, y0, y + 1, color);
	
	/* lower left */
	if ((option & CIRCLE_DRAW_LOWER_LEFT))
		drawVLine(x0 - x, y0, y + 1, color);
}

// From u8g2
void MaskPainter::fillEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint8_t color) {
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
		fillEllipseSectionHelper(x, y, x0, y0, option, color);
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
		fillEllipseSectionHelper(x, y, x0, y0, option, color);
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

void MaskPainter::drawRect(int x, int y, int w, int h, uint8_t color) {
	drawHLine(x, y, w, color);
	
	if (h >= 2) {
		drawHLine(x, y + h - 1, w, color);
		drawVLine(x, y + 1, h - 2, color);
		drawVLine(x + w - 1, y + 1, h - 2, color);
	}
}

void MaskPainter::fillRect(int x, int y, int w, int h, uint8_t color) {
	for (int i = 0; i < h; i++)
		drawHLine(x, y + i, w, color);
}
