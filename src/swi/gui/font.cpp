#include "swi.h"
#include "utils.h"
#include "log.h"
#include "gui/Painter.h"
#include "gui/Font.h"
#include "Resources.h"

#include <cassert>

int GUI_GetFontYSIZE(int font_id) {
	Font *font = Resources::instance()->getFont(font_id);
	assert(font);
	return font->h;
}

int GUI_GetSymbolWidth(int ch, int font_id) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int GUI_GetStringWidth(WSHDR *ws, int font_id) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}
