#include <swilib/gui.h>
#include <swilib/wstring.h>
#include <spdlog/spdlog.h>

#include "src/Resources.h"
#include "src/swi/gui/TextRender.h"

int GetFontYSIZE(int font_id) {
	return Resources::instance()->getFont(font_id)->h;
}

int GetSymbolWidth(int character, int font_id) {
	return Resources::instance()->getFontChar(font_id, character)->w;
}

int Get_WS_width(const WSHDR *text, int font_id) {
	auto [line_width, line_height] = TextRender::measureString(font_id, wsbody(text)->data, wsbody(text)->len);
	return line_width;
}
