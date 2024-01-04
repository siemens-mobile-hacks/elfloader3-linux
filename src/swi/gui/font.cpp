#include "swi.h"
#include "utils.h"
#include "log.h"
#include "charset.h"
#include "gui/Painter.h"
#include "swi/gui/TextRender.h"
#include "Resources.h"

#include <cassert>
#include <vector>

int GUI_GetStringWidth(WSHDR *ws, int font_id) {
	auto [line_width, line_height] = TextRender::measureString(font_id, ws->body->data, ws->body->len);
	return line_width;
}

int GUI_GetFontYSIZE(int font_id) {
	return Resources::instance()->getFont(font_id)->h;
}

int GUI_GetSymbolWidth(int ch, int font_id) {
	return Resources::instance()->getFontChar(font_id, ch)->w;
}
