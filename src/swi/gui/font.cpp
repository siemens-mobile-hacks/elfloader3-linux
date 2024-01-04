#include "swi.h"
#include "utils.h"
#include "log.h"
#include "charset.h"
#include "gui/Painter.h"
#include "Resources.h"

#include <cassert>
#include <vector>

struct WordWrapItem {
	uint16_t start;
	uint16_t end;
	int w;
	int h;
	DrawTextState state;
};

bool _isWordSeparator(uint16_t ch) {
	if ((ch & 0xFF00) == 0xE000)
		return true;
	
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

std::vector<WordWrapItem> GUI_ExplodeStringToWords(DrawTextState *state, WSHDR *ws, int max_width) {
	auto *res = Resources::instance();
	std::vector<WordWrapItem> words;
	
	const auto insertWord = [state, ws, &words, max_width, res](int start, int end) {
		int length = end + 1;
		
		int word_width = 0;
		int word_height = 0;
		
		int i = start;
		int str_start = start;
		while (i <= end) {
			int ctrl_chars = GUI_HandleTextModifiers(state, &ws->body->data[i], length - i);
			if (ctrl_chars > 0)
				i += ctrl_chars - 1;
			
			uint16_t ch = ws->body->data[i];
			auto *img = res->getFontChar(state->font, ch);
			
			if (word_width + img->w > max_width) {
				words.resize(words.size() + 1);
				WordWrapItem &word = words.back();
				word.w = word_width;
				word.h = word_height;
				word.start = str_start;
				word.end = i - 1;
				word.state = *state;
				
				str_start = i;
				word_width = img->w;
				word_height = img->h;
			}
			
			word_width += img->w;
			if (word_height < img->h)
				word_height = img->h;
			
			if (i == end) {
				words.resize(words.size() + 1);
				WordWrapItem &word = words.back();
				word.w = word_width;
				word.h = word_height;
				word.start = str_start;
				word.end = i;
				word.state = *state;
			}
			
			i++;
		}
	};
	
	int line_width = 0;
	int line_height = 0;
	
	int word_start = 0;
	uint16_t prev_ch = 0xE000;
	
	for (int i = 0; i < ws->body->len; i++) {
		uint16_t ch = ws->body->data[i];
		if (_isWordSeparator(ch)) {
			if (i < ws->body->len - 1) {
				insertWord(word_start, i);
				word_start = i + 1;
			}
		}
	}
	
	if (word_start < ws->body->len)
		insertWord(word_start, ws->body->len - 1);
	
	return words;
}

void GUI_DrawObject_MultilineString(Painter *painter, DrawTextState *state, WSHDR *ws, RECT *rect) {
	auto *res = Resources::instance();
	
	int max_width = rect->x2 - rect->x + 1;
	auto words = GUI_ExplodeStringToWords(state, ws, max_width);
	
	const auto drawLineWords = [res, painter, ws, &words](int offset_y, int start, int end, int line_width, int line_height) {
		int x = 0;
		for (int i = start; i <= end; i++) {
			WordWrapItem &word = words[i];
			for (int j = word.start; j <= word.end; j++) {
				int ch = ws->body->data[j];
				if (ch != 0x0A && ch != 0x0D) {
					auto *img = res->getFontChar(word.state.font, ch);
					if (img->w > 0) {
						int y = offset_y + (line_height - img->h);
						painter->drawBitmap(x, y, img->w, img->h, img->bitmap, IMG_GetBitmapType(img->bpnum), 0, 0, word.state.brush, word.state.pen);
						x += img->w;
					}
				}
			}
		}
	};
	
	int line_word_start = 0;
	int line_width = 0;
	int line_height = 0;
	int offset_y = 0;
	int word_index = 0;
	
	for (int i = 0; i < words.size(); i++) {
		WordWrapItem &word = words[i];
		
		bool has_line_end = ws->body->data[word.end] == 0x0A || ws->body->data[word.end] == 0x0D;
		
		if (line_width + word.w > max_width) {
			drawLineWords(offset_y, line_word_start, word_index - 1, line_width, line_height);
			offset_y += line_height;
			line_width = 0;
			line_height = 0;
			line_word_start = word_index;
		}
		
		line_height = std::max(line_height, word.h);
		line_width += word.w;
		word_index++;
		
		if (i == words.size() - 1 || has_line_end) {
			drawLineWords(offset_y, line_word_start, word_index - 1, line_width, line_height);
			offset_y += line_height;
			line_width = 0;
			line_height = 0;
			line_word_start = word_index;
		}
		
		/*
		char tmp[4096 * 10];
		size_t new_len = utf16_to_utf8(ws->body->data + word.start, word.end - word.start + 1, tmp, sizeof(tmp));
		tmp[new_len] = 0;
		printf("word: %d %d '%s' [%d %d]\n", word.start, word.end, tmp, word.w, word.h);
		*/
	}
}

void GUI_DrawObject_ScrollString(Painter *painter, DrawTextState *state, WSHDR *ws, RECT *rect, int x_offset) {
	auto *res = Resources::instance();
	
	int i = 0;
	int x = 0;
	int virtual_x = 0;
	int max_width = rect->x2 - rect->x + 1;
	
	int ctrl_chars;
	while ((ctrl_chars = GUI_HandleTextModifiers(state, &ws->body->data[i], ws->body->len - i)) > 0)
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

std::pair<int, int> GUI_MeasureString(DrawTextState *state, uint16_t *str, int length) {
	auto *res = Resources::instance();
	
	int i = 0;
	int line_width = 0;
	int line_height = 0;
	
	int ctrl_chars;
	while ((ctrl_chars = GUI_HandleTextModifiers(state, &str[i], length - i)) > 0)
		i += ctrl_chars;
	
	while (i < length) {
		auto *img = res->getFontChar(state->font, str[i]);
		line_width += img->w;
		if (line_height < img->h)
			line_height = img->h;
		i++;
	}
	
	return { line_width, line_height };
}

int GUI_GetStringWidth(WSHDR *ws, int font_id) {
	DrawTextState state = {
		.pen			= 0,
		.brush			= 0,
		.default_font	= font_id,
		.font			= font_id,
		.flags			= 0,
	};
	auto [line_width, line_height] = GUI_MeasureString(&state, ws->body->data, ws->body->len);
	return line_width;
}

int GUI_GetFontYSIZE(int font_id) {
	return Resources::instance()->getFont(font_id)->h;
}

int GUI_GetSymbolWidth(int ch, int font_id) {
	return Resources::instance()->getFontChar(font_id, ch)->w;
}
