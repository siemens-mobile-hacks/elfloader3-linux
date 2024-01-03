#include "swi.h"
#include "utils.h"
#include "log.h"
#include "charset.h"
#include "gui/Painter.h"
#include "Resources.h"

#include <cassert>
#include <vector>

struct WordItem {
	uint16_t start;
	uint16_t end;
};

bool _isWordSeparator(uint16_t ch) {
	switch (ch) {
		case 0x0009: // \t
		case 0x000A: // \n
		case 0x000B: // \v
		case 0x000C: // \f
		case 0x000D: // \r
		case 0x0020: // SP
		case 0x00A0: // NBSP
		case 0x002D: // -
			return true;
	}
	return false;
}

std::vector<WordItem> _explodeStringToWords(WSHDR *ws) {
	std::vector<WordItem> words;
	words.resize(1);
	
	words.back().start = 0;
	words.back().end = 0;
	
	for (int i = 0; i < ws->body->len; i++) {
		uint16_t ch = ws->body->data[i];
		
		words.back().end = i;
		
		if (_isWordSeparator(ch)) {
			if (i < ws->body->len - 1) {
				words.resize(words.size() + 1);
				words.back().start = i + 1;
				words.back().end = i + 1;
			}
		}
	}
	
	return words;
	/*
	char tmp[4096 * 10];
	
	printf("words: %d\n", words.size());
	
	for (auto &w: words) {
		size_t new_len = utf16_to_utf8(ws->body->data + w.start, w.end - w.start + 1, tmp, sizeof(tmp));
		tmp[new_len] = 0;
		
		printf("word: %d %d '%s'\n", w.start, w.end, tmp);
	}
	*/
}

int GUI_GetFontByEXXX(uint16_t ch) {
	switch (ch) {
		case UTF16_FONT_SMALL:			return FONT_SMALL;
		case UTF16_FONT_SMALL_BOLD:		return FONT_SMALL_BOLD;
		case UTF16_FONT_MEDIUM:			return FONT_MEDIUM;
		case UTF16_FONT_MEDIUM_BOLD:	return FONT_MEDIUM_BOLD;
		case UTF16_FONT_LARGE_BOLD:		return FONT_LARGE_BOLD;
		case UTF16_FONT_LARGE:			return FONT_LARGE;
		case UTF16_FONT_UNK0:			return FONT_SMALL;
		case UTF16_FONT_UNK1:			return FONT_LARGE_BOLD;
		case UTF16_FONT_UNK2:			return FONT_SMALL;
		case UTF16_FONT_UNK3:			return FONT_SMALL_BOLD;
		case UTF16_FONT_UNK4:			return FONT_SMALL;
	}
	return FONT_SMALL;
}

int GUI_HandleTextModifiers(DrawTextState *state, uint16_t *str, uint16_t max_len) {
	switch (str[0]) {
		case UTF16_UNDERLINE:
			state->flags |= TEXT_UNDERLINE;
			return 1;
		
		case UTF16_NO_UNDERLINE:
			state->flags &= ~TEXT_UNDERLINE;
			return 1;
		
		case UTF16_BG_INVERTION:
			state->flags |= TEXT_INVERT;
			return 1;
		
		case UTF16_BG_INVERTION2:
			state->flags |= TEXT_INVERT2;
			return 1;
		
		case UTF16_NO_INVERTION:
			state->flags &= (TEXT_INVERT | TEXT_INVERT2);
			return 1;
		
		case UTF16_TEXT_COLOR_RGBA:
			if (max_len >= 3)
				state->pen = (str[1] << 16) | str[2];
			return 3;
		
		case UTF16_BG_COLOR_RGBA:
			if (max_len >= 3)
				state->brush = (str[1] << 16) | str[2];
			return 3;
		
		case UTF16_TEXT_COLOR:
			if (max_len >= 2)
				state->pen = GUI_Color2Int(GUI_GetPaletteAdrByColorIndex(str[1]));
			return 2;
		
		case UTF16_BG_COLOR:
			if (max_len >= 2)
				state->brush = GUI_Color2Int(GUI_GetPaletteAdrByColorIndex(str[1]));
			return 2;
		
		case UTF16_FONT_RESET:
			state->font = state->default_font;
			return 1;
		
		case UTF16_ALIGN_NONE:
			state->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
			return 1;
		
		case UTF16_ALIGN_RIGHT:
			state->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
			state->flags |= TEXT_ALIGNRIGHT;
			return 1;
		
		case UTF16_ALIGN_LEFT:
			state->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
			state->flags |= TEXT_ALIGNLEFT;
			return 1;
		
		case UTF16_ALIGN_CENTER:
			state->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
			state->flags |= TEXT_ALIGNMIDDLE;
			return 1;
		
		case UTF16_FONT_SMALL:
		case UTF16_FONT_SMALL_BOLD:
		case UTF16_FONT_MEDIUM:
		case UTF16_FONT_MEDIUM_BOLD:
		case UTF16_FONT_LARGE_BOLD:
		case UTF16_FONT_LARGE:
		case UTF16_FONT_UNK0:
		case UTF16_FONT_UNK1:
		case UTF16_FONT_UNK2:
		case UTF16_FONT_UNK3:
		case UTF16_FONT_UNK4:
			state->font = GUI_GetFontByEXXX(str[0]);
			return 1;
	}
	return 0;
}

void GUI_DrawObject_ScrollString(Painter *painter, DrawTextState *state, WSHDR *ws, RECT *rect, int x_offset) {
	auto *res = Resources::instance();
	
	int i = 0;
	int x = 0;
	int virtual_x = 0;
	int max_width = rect->x2 - rect->x + 1;
	
	int ctrl_chars;
	while ((ctrl_chars = GUI_HandleTextModifiers(state, &ws->body->data[i], ws->body->len - 1)) > 0)
		i += ctrl_chars;
	
	while (i < ws->body->len) {
		uint16_t ch = ws->body->data[i];
		
		IMGHDR *img = res->getFontChar(state->font, ch, false);
		if (virtual_x >= (x_offset - 1)) {
			painter->drawBitmap(x, 0, img->w, img->h, img->bitmap, IMG_GetBitmapType(img->bpnum), 0, 0, state->brush, state->pen);
			x += img->w;
			
			if (x >= max_width)
				break;
		}
		
		virtual_x += img->w;
		
		i++;
	}
}

int GUI_GetStringWidth(WSHDR *ws, int font_id) {
	auto *res = Resources::instance();
	
	DrawTextState state = {
		.pen			= 0,
		.brush			= 0,
		.default_font	= font_id,
		.font			= font_id,
		.flags			= 0,
	};
	
	int i = 0;
	int x = 0;
	
	int ctrl_chars;
	while ((ctrl_chars = GUI_HandleTextModifiers(&state, &ws->body->data[i], ws->body->len - 1)) > 0)
		i += ctrl_chars;
	
	while (i < ws->body->len) {
		x += res->getFontChar(state.font, ws->body->data[i])->w;
		i++;
	}
	
	return x;
}

int GUI_GetFontYSIZE(int font_id) {
	return Resources::instance()->getFont(font_id)->h;
}

int GUI_GetSymbolWidth(int ch, int font_id) {
	return Resources::instance()->getFontChar(font_id, ch)->w;
}
