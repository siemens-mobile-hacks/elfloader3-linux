#include "swi.h"
#include "utils.h"
#include "log.h"
#include "gui/Painter.h"
#include "Resources.h"

#include <cassert>

void GUI_DrawString(WSHDR *ws, int x1, int y1, int x2, int y2, int font_id, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect = {
		.x = std::min(x1, x2),
		.y = std::min(y1, y2),
		.x2 = std::max(x1, x2),
		.y2 = std::max(y1, y2),
	};
	
	GUI_SetProp2Text(&drw, &rect, 0, ws, font_id, flags);
	GUI_DrawObjectSetColor(&drw, brush, pen);
	GUI_DrawObject(&drw);
	GUI_FreeDrawObject(&drw);
	
	/*
	fprintf(stderr, "%s not implemented!\n", __func__);
	
	Font *font = Resources::instance()->getFont(font_id);
	assert(font);
	
	Painter *painter = GUI_GetPainter();
	painter->drawText(x1, y1, x2 - x1 + 1, y2 - y1 + 1, font, ws->body->data, ws->body->len, GUI_Color2Int(brush), GUI_Color2Int(pen));
	
	GUI_IpcRedrawScreen();
	*/
}

void GUI_DrawScrollString(WSHDR *ws, int x1, int y1, int x2, int y2, int xdisp, int font_id, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect = {
		.x = std::min(x1, x2),
		.y = std::min(y1, y2),
		.x2 = std::max(x1, x2),
		.y2 = std::max(y1, y2),
	};
	
	GUI_SetProp2ScrollingText(&drw, &rect, 0, ws, xdisp, font_id, flags);
	GUI_DrawObjectSetColor(&drw, brush, pen);
	GUI_DrawObject(&drw);
	GUI_FreeDrawObject(&drw);
}

void GUI_DrawPixel(int x1, int y1, const char *pen) {
	DRWOBJ drw;
	RECT rect = { 0, 0, x1, y1 };
	
	if (!GUI_ColorIsTransparent(pen)) {
		GUI_SetProp2Pixel(&drw, &rect, 0, x1, y1);
		GUI_DrawObjectSetColor(&drw, pen, nullptr);
		GUI_DrawObject(&drw);
		GUI_FreeDrawObject(&drw);
	}
}

void GUI_DrawLine(int x1, int y1, int x2, int y2, int flags, const char *pen) {
	DRWOBJ drw;
	RECT rect = {
		.x = std::min(x1, x2),
		.y = std::min(y1, y2),
		.x2 = std::max(x1, x2),
		.y2 = std::max(y1, y2),
	};
	
	if (!GUI_ColorIsTransparent(pen)) {
		GUI_SetProp2Line(&drw, &rect, (flags & RECT_DOT_OUTLINE), x1, y1, x2, y2);
		drw.line.flags = flags;
		GUI_DrawObjectSetColor(&drw, pen, nullptr);
		GUI_DrawObject(&drw);
		GUI_FreeDrawObject(&drw);
	}
}

void GUI_DrawRectangle(int x1, int y1, int x2, int y2, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect = {
		.x = std::min(x1, x2),
		.y = std::min(y1, y2),
		.x2 = std::max(x1, x2),
		.y2 = std::max(y1, y2),
	};
	
	if (!GUI_ColorIsTransparent(brush)) {
		if ((flags & RECT_DRAW_INVERT)) {
			GUI_SetProp2Rect(&drw, &rect, DRWOBJ_RECT_FLAG_INVERT_BG);
		} else if ((flags & RECT_FILL_WITH_PEN)) {
			GUI_SetProp2RectEx(&drw, &rect, 0, DRWOBJ_RECT_BG_TYPE_PATTERN, 0x55);
		} else {
			GUI_SetProp2RectEx(&drw, &rect, 0, DRWOBJ_RECT_BG_TYPE_FILL, 0);
		}
		GUI_DrawObjectSetColor(&drw, brush, pen);
		GUI_DrawObject(&drw);
		GUI_FreeDrawObject(&drw);
	}
	
	bool only_bg = (flags & RECT_DRAW_INVERT) || ((flags & RECT_FILL_WITH_PEN) && (flags & RECT_DOT_OUTLINE));
	if (pen != brush && !GUI_ColorIsTransparent(pen) && !only_bg) {
		int line_flags = flags & ~RECT_FILL_WITH_PEN;
		GUI_DrawLine(rect.x, rect.y, rect.x2, rect.y, line_flags, pen);
		GUI_DrawLine(rect.x, rect.y2, rect.x2, rect.y2, line_flags, pen);
		GUI_DrawLine(rect.x, rect.y, rect.x, rect.y2, line_flags, pen);
		GUI_DrawLine(rect.x2, rect.y, rect.x2, rect.y2, line_flags, pen);
	}
}

void GUI_DrawRoundedFrame(int x1, int y1, int x2, int y2, int round_x, int round_y, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect2;
	RECT rect = {
		.x = std::min(x1, x2),
		.y = std::min(y1, y2),
		.x2 = std::max(x1, x2),
		.y2 = std::max(y1, y2),
	};
	
	int w = rect.x2 - rect.x + 1;
	int h = rect.y2 - rect.y + 1;
	
	if (round_x < 2 || round_y < 2 || w < 4 || h < 4) {
		GUI_DrawRectangle(x1, y1, x2, y2, flags, pen, brush);
		return;
	}
	
	round_x = std::min(w / 2, round_x);
	round_y = std::min(h / 2, round_y);
	
	int xl = rect.x + round_x;
	int yu = rect.y + round_y;
	
	int xr = rect.x2 - round_x;
	int yl = rect.y2 - round_y;
	
	if (!GUI_ColorIsTransparent(brush)) {
		GUI_DrawObjectSetColor(&drw, brush, brush);
		
		GUI_SetProp2FilledEllipseSection(&drw, &rect, 1, xr + 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT);
		GUI_DrawObject(&drw);
		
		GUI_SetProp2FilledEllipseSection(&drw, &rect, 1, xl - 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT);
		GUI_DrawObject(&drw);
		
		GUI_SetProp2FilledEllipseSection(&drw, &rect, 1, xl - 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT);
		GUI_DrawObject(&drw);
		
		GUI_SetProp2FilledEllipseSection(&drw, &rect, 1, xr + 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT);
		GUI_DrawObject(&drw);
		
		if (xr - xl >= 0) {
			GUI_StoreXYXYtoRECT(&rect2, xl, rect.y, xr, yu - 1);
			GUI_SetProp2RectEx(&drw, &rect2, 0, 1, 0);
			GUI_DrawObject(&drw);
		}
		
		if (yl - yu >= 0) {
			GUI_StoreXYXYtoRECT(&rect2, rect.x, yu, rect.x2, yl);
			GUI_SetProp2RectEx(&drw, &rect2, 0, 1, 0);
			GUI_DrawObject(&drw);
		}
		
		if (xr - xl >= 0) {
			GUI_StoreXYXYtoRECT(&rect2, xl, yl + 1, xr, rect.y2);
			GUI_SetProp2RectEx(&drw, &rect2, 0, 1, 0);
			GUI_DrawObject(&drw);
		}
	}
	
	if (!GUI_ColorIsTransparent(pen) && !(flags & DRWOBJ_RECT_FLAG_INVERT_BG)) {
		GUI_DrawObjectSetColor(&drw, pen, pen);
		
		GUI_SetProp2StrokeEllipseSection(&drw, &rect, 1, xr + 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT);
		GUI_DrawObject(&drw);
		
		GUI_SetProp2StrokeEllipseSection(&drw, &rect, 1, xl - 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT);
		GUI_DrawObject(&drw);
		
		GUI_SetProp2StrokeEllipseSection(&drw, &rect, 1, xl - 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT);
		GUI_DrawObject(&drw);
		
		GUI_SetProp2StrokeEllipseSection(&drw, &rect, 1, xr + 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT);
		GUI_DrawObject(&drw);
		
		if (xr - xl >= 0) {
			GUI_DrawLine(xl, rect.y, xr + 1, rect.y, 0, pen);
			GUI_DrawLine(xl, rect.y2, xr + 1, rect.y2, 0, pen);
		}
		
		if (yl - yu >= 0) {
			GUI_DrawLine(rect.x, yu, rect.x, yl + 1, 0, pen);
			GUI_DrawLine(rect.x2, yu, rect.x2, yl + 1, 0, pen);
		}
	}
}

void GUI_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush) {
	DRWOBJ drw;
	GUI_SetProp2Triangle(&drw, x1, y1, x2, y2, x3, y3, flags, pen, brush);
	GUI_DrawObject(&drw);
	GUI_FreeDrawObject(&drw);
}

void GUI_DrawArc(int x1, int y1, int x2, int y2, int start, int end, int flags, char *pen, char *brush) {
	DRWOBJ drw;
	RECT rect = { x1, y1, x2, y2 };
	
	if (!GUI_ColorIsTransparent(brush)) {
		GUI_SetProp2Pie(&drw, &rect, 0, x1, y1, x2 - x1 + 1, y2 - y1 + 1, start, end);
		GUI_DrawObjectSetColor(&drw, brush, brush);
		GUI_DrawObject(&drw);
		GUI_FreeDrawObject(&drw);
	}
	
	if (!GUI_ColorIsTransparent(pen)) {
		GUI_SetProp2Arc(&drw, &rect, 0, x1, y1, x2 - x1 + 1, y2 - y1 + 1, start, end);
		GUI_DrawObjectSetColor(&drw, pen, pen);
		drw.arc.flags = flags;
		GUI_DrawObject(&drw);
		GUI_FreeDrawObject(&drw);
	}
}

void GUI_DrawIMGHDR(int x, int y, IMGHDR *img, const char *pen, const char *brush, uint16_t offset_x, uint16_t offset_y, int w, int h, int flags) {
	DRWOBJ drw;
	RECT rect = { x, y, x + w - 1, y + h - 1 };
	
	assert(img != nullptr);
	assert(img->bitmap != nullptr);
	
	static const char black[] = { 0x00, 0x00, 0x00, 0x64 };
	static const char transparent[] = { 0x00, 0x00, 0x00, 0x00 };
	
	pen = pen ?: black;
	brush = brush ?: transparent;
	
	GUI_SetProp2ImageOrCanvas(&drw, &rect, 0, img, offset_x, offset_y);
	GUI_DrawObjectSetColor(&drw, pen, brush);
	GUI_DrawObject(&drw);
	GUI_FreeDrawObject(&drw);
}

void GUI_DrawImg(int x, int y, int picture) {
	IMGHDR *img = Resources::instance()->getPicture(picture);
	if (img)
		GUI_DrawIMGHDR(x, y, img, nullptr, nullptr, 0, 0, img->w, img->h, 0);
}

void GUI_DrawCanvas(void *data, int x1, int y1, int x2, int y2, int flags) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GUI_DrawImgBW(int x, int y, int picture, char *pen, char *brush) {
	IMGHDR *img = Resources::instance()->getPicture(picture);
	if (img)
		GUI_DrawIMGHDR(x, y, img, pen, brush, 0, 0, img->w, img->h, 0);
}
