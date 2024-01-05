#include "TextRender.h"
#include "charset.h"

TextRender::TextRender(Painter *painter, RECT *rect, uint16_t *str, int length) {
	m_rect = rect;
	m_str = str;
	m_length = length;
	m_painter = painter;
}

void TextRender::parseWord(int start, int end) {
	auto *res = Resources::instance();
	
	int max_width = m_rect->x2 - m_rect->x + 1;
	int ctrl_chars = handleModifiers(&m_style, m_str + start, end - start + 1);
	
	int word_width = 0;
	int word_height = 0;
	int str_start = start + ctrl_chars;
	
	for (int i = start + ctrl_chars; i <= end; i++) {
		uint16_t ch = m_str[i];
		auto *img = res->getFontChar(m_style.font, ch);
		
		// The current part of the word exceeds maximum line length
		if (word_width + img->w > max_width) {
			onNewWord(str_start, i - 1, word_width, word_height);
			
			str_start = i;
			word_width = img->w;
			word_height = img->h;
		}
		
		word_width += img->w;
		if (word_height < img->h)
			word_height = img->h;
		
		// Handle end of word
		if (i == end)
			onNewWord(str_start, i, word_width, word_height);
	}
}

int TextRender::drawWord(Word *word, int x, int y) {
	auto *res = Resources::instance();
	int width = 0;
	for (int j = word->start; j <= word->end; j++) {
		auto *img = res->getFontChar(word->style.font, m_str[j]);
		if (img->w > 0) {
			m_painter->drawBitmap(x + width, y - img->h, img->w, img->h, img->bitmap, IMG_GetBitmapType(img->bpnum), 0, 0, word->style.brush, word->style.pen);
			width += img->w;
		}
	}
	return width;
}

void TextRender::renderLine() {
	auto *res = Resources::instance();
	
	if (!m_words.size() > 0)
		return;
	
	if (m_push_to_prev_line)
		m_offset_y -= m_prev_line_height;
	
	int max_width = m_rect->x2 - m_rect->x + 1;
	int align = getAlignFromStyle(&m_words.front().style);
	
	int x = 0;
	if (align == ALIGN_CENTER) {
		x = (max_width - m_line_width) / 2;
	} else if (align == ALIGN_RIGHT) {
		x = (max_width - m_line_width);
	}
	
	if (align == ALIGN_LEFT || align == ALIGN_CENTER) {
		m_prev_line_avail = max_width - (x + m_line_width);
	} else {
		m_prev_line_avail = 0;
	}
	
	bool has_underline = false;
	while (m_words.size() > 0) {
		auto &word = m_words.front();
		
		if ((word.style.flags & TEXT_UNDERLINE))
			has_underline = true;
		
		int word_width = drawWord(&word, x, m_offset_y + m_line_height);
		
		if ((word.style.flags & TEXT_UNDERLINE))
			m_painter->drawLine(x, m_offset_y + m_line_height - 1, x + word_width + 1, m_offset_y + m_line_height - 1, word.style.pen);
		
		x += word_width;
		
		m_words.pop();
	}
	
	if (has_underline)
		m_line_height++;
	
	m_prev_line_height = m_line_height;
	m_offset_y += m_line_height;
	m_line_width = 0;
	m_line_height = 0;
	m_push_to_prev_line = false;
}

void TextRender::onNewWord(int start, int end, int w, int h) {
	int max_width = m_rect->x2 - m_rect->x + 1;
	int align = getAlignFromStyle(&m_style);
	
	if ((m_line_width + w > max_width) || align != m_prev_align)
		renderLine();
	
	if (align != m_prev_align && align == ALIGN_RIGHT && m_prev_line_avail >= w)
		m_push_to_prev_line = true;
	
	if (m_push_to_prev_line && (align != ALIGN_RIGHT || m_line_width + w > m_prev_line_avail))
		renderLine();
	
	m_words.push({
		.start	= start,
		.end	= end,
		.w		= w,
		.h		= h,
		.style	= m_style
	});
	
	m_line_height = std::max(m_line_height, h);
	m_line_width += w;
	
	if (m_str[end] == 0x0A || m_str[end] == 0x0D)
		renderLine();
	
	m_prev_align = getAlignFromStyle(&m_style);
}

void TextRender::renderInline(int x_offset) {
	auto *res = Resources::instance();
	
	int x = 0;
	int virtual_x = 0;
	int max_width = m_rect->x2 - m_rect->x + 1;
	
	int ctrl_chars = handleModifiers(&m_style, m_str, m_length);
	for (int i = ctrl_chars; i < m_length; i++) {
		auto *img = res->getFontChar(m_style.font, m_str[i], false);
		if (virtual_x >= (x_offset - 1)) {
			m_painter->drawBitmap(x, 0, img->w, img->h, img->bitmap, IMG_GetBitmapType(img->bpnum), 0, 0, m_style.brush, m_style.pen);
			x += img->w;
			
			if (x >= max_width)
				break;
		}
		
		virtual_x += img->w;
	}
}

void TextRender::render() {
	auto *res = Resources::instance();
	
	int word_start = 0;
	uint16_t prev_ch = 0xE000;
	
	m_prev_align = getAlignFromStyle(&m_style);
	m_line_height = 0;
	m_line_width = 0;
	m_offset_y = 0;
	m_prev_line_avail = 0;
	m_prev_line_height = 0;
	m_push_to_prev_line = false;
	
	for (int i = 0; i < m_length; i++) {
		uint16_t ch = m_str[i];
		if ((prev_ch & 0xFF00) != 0xE000 && (ch & 0xFF00) == 0xE000) {
			parseWord(word_start, i - 1);
			word_start = i;
		} else if (isWordSeparator(ch)) {
			if (i < m_length - 1) {
				parseWord(word_start, i);
				word_start = i + 1;
			}
		}
		prev_ch = ch;
	}
	
	if (word_start < m_length)
		parseWord(word_start, m_length - 1);
	
	if (m_words.size() > 0)
		renderLine();
}

std::pair<int, int> TextRender::measureString(int font_id, uint16_t *str, int length) {
	auto *res = Resources::instance();
	
	int line_width = 0;
	int line_height = 0;
	
	Style style = {
		.pen			= 0,
		.brush			= 0,
		.default_font	= font_id,
		.font			= font_id,
		.flags			= 0,
	};
	
	int ctrl_chars = handleModifiers(&style, str, length);
	for (int i = ctrl_chars; i < length; i++) {
		auto *img = res->getFontChar(style.font, str[i]);
		line_width += img->w;
		if (line_height < img->h)
			line_height = img->h;
		i++;
	}
	
	return { line_width, line_height };
}

int TextRender::getAlignFromStyle(Style *style) {
	if ((style->flags & TEXT_ALIGNRIGHT))
		return ALIGN_RIGHT;
	if ((style->flags & TEXT_ALIGNMIDDLE))
		return ALIGN_CENTER;
	return ALIGN_LEFT;
}

int TextRender::handleModifiers(Style *style, uint16_t *str, int length) {
	int i = 0;
	while (i < length) {
		switch (str[i]) {
			case UTF16_UNDERLINE:
				style->flags |= TEXT_UNDERLINE;
			break;
			
			case UTF16_NO_UNDERLINE:
				style->flags &= ~TEXT_UNDERLINE;
			break;
			
			case UTF16_BG_INVERTION:
				style->flags |= TEXT_INVERT;
			break;
			
			case UTF16_BG_INVERTION2:
				style->flags |= TEXT_INVERT2;
			break;
			
			case UTF16_NO_INVERTION:
				style->flags &= (TEXT_INVERT | TEXT_INVERT2);
			break;
			
			case UTF16_TEXT_COLOR_RGBA:
			{
				if (i + 2 < length) {
					uint8_t r = (str[i + 1] >> 8) & 0xFF;
					uint8_t g = str[i + 1] & 0xFF;
					uint8_t b = (str[i + 2] >> 8) & 0xFF;
					uint8_t a = (str[i + 2] & 0xFF) * 0xFF / 0x64;
					style->pen = (a << 24) | (r << 16) | (g << 8) | b;
					i += 2;
				}
			}
			break;
			
			case UTF16_BG_COLOR_RGBA:
				if (i + 2 < length) {
					uint8_t r = (str[i + 1] >> 8) & 0xFF;
					uint8_t g = str[i + 1] & 0xFF;
					uint8_t b = (str[i + 2] >> 8) & 0xFF;
					uint8_t a = (str[i + 2] & 0xFF) * 0xFF / 0x64;
					style->brush = (a << 24) | (r << 16) | (g << 8) | b;
					i += 2;
				}
			break;
			
			case UTF16_TEXT_COLOR:
				if (i + 1 < length) {
					style->pen = GUI_Color2Int(GUI_GetPaletteAdrByColorIndex(str[i + 1]));
					i++;
				}
			break;
			
			case UTF16_BG_COLOR:
				if (i + 1 < length) {
					style->brush = GUI_Color2Int(GUI_GetPaletteAdrByColorIndex(str[i + 1]));
					i++;
				}
			break;
			
			case UTF16_FONT_RESET:
				style->font = style->default_font;
			break;
			
			case UTF16_ALIGN_NONE:
				style->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
			break;
			
			case UTF16_ALIGN_RIGHT:
				style->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
				style->flags |= TEXT_ALIGNRIGHT;
			break;
			
			case UTF16_ALIGN_LEFT:
				style->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
				style->flags |= TEXT_ALIGNLEFT;
			break;
			
			case UTF16_ALIGN_CENTER:
				style->flags &= ~(TEXT_ALIGNRIGHT | TEXT_ALIGNLEFT | TEXT_ALIGNMIDDLE);
				style->flags |= TEXT_ALIGNMIDDLE;
			break;
			
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
				style->font = getFontByEXXX(str[i]);
			break;
			
			default:
				return i;
		}
		i++;
	}
	return i;
}

int TextRender::getFontByEXXX(uint16_t ch) {
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

bool TextRender::isWordSeparator(uint16_t ch) {
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
