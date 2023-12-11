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
		GUI_SetProp2Line(&drw, &rect, 0, x1, y1, x2, y2);
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
	
	GUI_SetProp2Rect(&drw, &rect, 0);
	GUI_DrawObjectSetColor(&drw, brush, pen);
	GUI_DrawObject(&drw);
	GUI_FreeDrawObject(&drw);
}

void GUI_DrawRoundedFrame(int x1, int y1, int x2, int y2, int round_x, int round_y, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect = {
		.x = std::min(x1, x2),
		.y = std::min(y1, y2),
		.x2 = std::max(x1, x2),
		.y2 = std::max(y1, y2),
	};
	
	GUI_SetProp2RoundedRect(&drw, &rect, flags, round_x, round_y);
	GUI_DrawObjectSetColor(&drw, brush, pen);
	GUI_DrawObject(&drw);
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
