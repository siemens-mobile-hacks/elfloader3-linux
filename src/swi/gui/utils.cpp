#include <cstring>
#include <swilib/gui.h>
#include <spdlog/spdlog.h>

#include "src/Resources.h"

char *GetPaletteAdrByColorIndex(int index) {
	return Resources::instance()->getColorPtr(index);
}

void getRGBcolor(int index, void *color) {
	getRGBbyPaletteAdr(GetPaletteAdrByColorIndex(index), color);
}

void getRGBbyPaletteAdr(const void *src_color, void *dst_color) {
	assert(dst_color != nullptr && src_color != nullptr);
	memcpy(dst_color, src_color, 4);
}

void setColor(int a, int r, int g, int b, void *dst) {
	uint8_t *u8 = (uint8_t *) dst;
	assert(u8 != nullptr);
	u8[0] = r;
	u8[1] = g;
	u8[2] = b;
	u8[3] = a;
}

void StoreXYWHtoRECT(RECT *rect, int x, int y, int w, int h) {
	rect->x = x;
	rect->y = y;
	rect->x2 = x + w - 1;
	rect->y2 = y + h - 1;
}

void StoreXYXYtoRECT(RECT *rect, int x, int y, int x2, int y2) {
	rect->x = x;
	rect->y = y;
	rect->x2 = x2;
	rect->y2 = y2;
}

int ScreenW(void) {
#if defined(ELKA)
	return 240;
#else
	return 132;
#endif
}

int ScreenH(void) {
#if defined(ELKA)
	return 320;
#else
	return 176;
#endif
}

int HeaderH(void) {
#if defined(ELKA)
	return 32;
#elif defined(NEWSGOLD)
	return 22;
#elif defined(X75)
	return 18;
#else
	return 22;
#endif
}

int SoftkeyH(void) {
#if defined(ELKA)
	return 32;
#elif defined(NEWSGOLD)
	return 22;
#elif defined(X75)
	return 18;
#else
	return 22;
#endif
}
