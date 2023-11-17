#include "MaskPainter.h"

#include <algorithm>
#include <cmath>

void MaskPainter::setCanvasSize(int width, int height) {
	m_width = width;
	m_height = height;
	m_buffer.resize(m_width * m_height);
	std::fill(m_buffer.begin(), m_buffer.end(), 0);
}

void MaskPainter::drawPixel(int x, int y, uint8_t color) {
	if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
		printf("%d, %d ignored [%dx%d]\n", x, y, m_width, m_height);
		return;
	}
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
void MaskPainter::drawEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color, int start, int end) {
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
void MaskPainter::drawEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint8_t color, int start, int end) {
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
		drawEllipseSectionHelper(x, y, x0, y0, option, color, start, end);
		
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
		drawEllipseSectionHelper(x, y, x0, y0, option, color, start, end);
		
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
void MaskPainter::fillEllipseSectionHelper(int x, int y, int x0, int y0, uint8_t option, uint8_t color, int start, int end) {
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
void MaskPainter::fillEllipseHelper(int x0, int y0, int rx, int ry, uint8_t option, uint8_t color, int start, int end) {
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

void MaskPainter::drawArc(int x, int y, int w, int h, int start_angle, int sweep_angle, uint8_t color) {
	int end_angle = start_angle + sweep_angle;
	
	int x_radius = w / 2;
	int y_radius = h / 2;
	
	int xl = x + x_radius;
	int yu = y + y_radius;
	
	int xr = x + w - x_radius - 1;
	int yl = y + h - y_radius - 1;
	
	bool w_is_mod2 = w % 2 == 0;
	bool h_is_mod2 = h % 2 == 0;
	
	const auto drawArcHelper = [&](int x0, int y0, uint8_t option, int section_angle) {
		auto [part_angle_start, part_angle_end] = getEllipseSectionRegion(start_angle, end_angle, section_angle, section_angle + 90);
		if (part_angle_start != -1) {
			if (part_angle_end - part_angle_start == 90) {
				drawEllipseHelper(x0, y0, x_radius, y_radius, option, color, -1, -1);
			} else {
				drawEllipseHelper(x0, y0, x_radius, y_radius, option, color, part_angle_start, part_angle_end);
			}
		}
	};
	
	drawArcHelper(xr, yu, CIRCLE_DRAW_UPPER_RIGHT, 0);
	drawArcHelper(xl, yu, CIRCLE_DRAW_UPPER_LEFT, 90);
	drawArcHelper(xl, yl, CIRCLE_DRAW_LOWER_LEFT, 180);
	drawArcHelper(xr, yl, CIRCLE_DRAW_LOWER_RIGHT, 270);
}

void MaskPainter::fillArc(int x, int y, int w, int h, int start_angle, int sweep_angle, uint8_t color) {
	int end_angle = start_angle + sweep_angle;
	
	int x_radius = w / 2;
	int y_radius = h / 2;
	
	int xl = x + x_radius;
	int yu = y + y_radius;
	
	int xr = x + w - x_radius - 1;
	int yl = y + h - y_radius - 1;
	
	bool w_is_mod2 = w % 2 == 0;
	bool h_is_mod2 = h % 2 == 0;
	
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

// From u8g2
void MaskPainter::drawLine(int x1, int y1, int x2, int y2, uint8_t color) {
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
		drawVLine(x1, y1, dx, color);
		return;
	} else if (!dx) {
		drawHLine(x1, y1, dy, color);
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

bool MaskPainter::isInEllipseRange(int x, int y, int start, int end) {
	int angle = round(atan2(y, x) * (180 / M_PI));
	if (angle < 0)
		angle = 360 + angle;
	return angle >= start && angle <= end;
}

std::tuple<int, int> MaskPainter::getEllipseSectionRegion(int x1, int x2, int y1, int y2) {
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
