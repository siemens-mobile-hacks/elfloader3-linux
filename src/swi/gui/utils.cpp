#include "swi.h"
#include "utils.h"
#include "log.h"
#include "Resources.h"

#include <cassert>

uint32_t GUI_Color2Int(const char *color) {
	color = color ?: GUI_GetPaletteAdrByColorIndex(23); // fallback
	const uint8_t *u8 = reinterpret_cast<const uint8_t *>(color);
	uint32_t a = static_cast<uint32_t>(u8[3]) * 0xFF / 0x64;
	return (a << 24) | (u8[0] << 16) | (u8[1] << 8) | u8[2]; // RGBA
}

char *GUI_GetPaletteAdrByColorIndex(int index) {
	return Resources::instance()->getColorPtr(index);
}

void GUI_GetRGBcolor(int index, char *dest) {
	GUI_GetRGBbyPaletteAdr(GUI_GetPaletteAdrByColorIndex(index), dest);
}

void GUI_GetRGBbyPaletteAdr(char *addr, char *dest) {
	assert(dest != nullptr && addr != nullptr);
	memcpy(dest, addr, 4);
}

void GUI_SetColor(int a, int r, int g, int b, char *dest) {
	assert(dest != nullptr);
	dest[0] = r;
	dest[1] = g;
	dest[2] = b;
	dest[3] = a;
}

int GUI_ScreenW() {
	return SCREEN_WIDTH;
}

int GUI_ScreenH() {
	return SCREEN_HEIGHT;
}

int GUI_HeaderH() {
	return 32; // ELKA
}

int GUI_SoftkeyH() {
	return 32; // ELKA
}

void GUI_StoreXYWHtoRECT(RECT *rect, int x, int y, int w, int h) {
	rect->x = x;
	rect->y = y;
	rect->x2 = x + w - 1;
	rect->y2 = y + h - 1;
}

void GUI_StoreXYXYtoRECT(RECT *rect, int x, int y, int x2, int y2) {
	rect->x = x;
	rect->y = y;
	rect->x2 = x2;
	rect->y2 = y2;
}
