#include "swi.h"
#include "utils.h"
#include "log.h"
#include "gui/Painter.h"

#include <cassert>

DRWOBJ *GUI_SetProp2Text(DRWOBJ *drw, RECT *rect, int rect_flag, WSHDR *wshdr, int font, int flags) {
	assert(drw != nullptr && rect != nullptr && wshdr != nullptr);
	
	drw->type = DRWOBJ_TYPE_TEXT;
	drw->rect_flags = rect_flag;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->text.flags = flags;
	drw->text.font = font;
	drw->text.value = AllocWS(wshdr->body->len + 1);
	memcpy(drw->text.value->wsbody, wshdr->wsbody, wshdr->body->len + 1);
	
	return drw;
}

DRWOBJ *GUI_SetProp2ScrollingText(DRWOBJ *drw, RECT *rect, int rect_flag, WSHDR *wshdr, int xdisp, int font, int flags) {
	assert(drw != nullptr && rect != nullptr && wshdr != nullptr);
	
	drw->type = DRWOBJ_TYPE_SCROLLING_TEXT;
	drw->rect_flags = rect_flag;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->text.flags = flags;
	drw->text.font = font;
	drw->text.value = AllocWS(wshdr->body->len + 1);
	drw->text.xdisp = xdisp;
	memcpy(drw->text.value->wsbody, wshdr->wsbody, wshdr->body->len + 1);
	
	return drw;
}

DRWOBJ *GUI_SetProp2ImageOrCanvas(DRWOBJ *drw, RECT *rect, int flags, IMGHDR *img, int offset_x, int offset_y) {
	assert(drw != nullptr);
	assert(rect != nullptr);
	assert(img != nullptr);
	
	drw->type = DRWOBJ_TYPE_IMG;
	drw->rect_flags = 0;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->img.value = img;
	drw->img.flags = flags;
	drw->img.offset_x = offset_x;
	drw->img.offset_y = offset_y;
	
	return drw;
}

DRWOBJ *GUI_SetProp2Image(DRWOBJ *drw, RECT *rect, int flags, IMGHDR *img) {
	assert(drw != nullptr);
	assert(rect != nullptr);
	assert(img != nullptr);
	
	drw->type = DRWOBJ_TYPE_IMG;
	drw->rect_flags = 0;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->img.value = img;
	drw->img.flags = flags;
	drw->img.offset_x = 0;
	drw->img.offset_y = 0;
	
	return drw;
}

DRWOBJ *GUI_SetProp2Triangle(DRWOBJ *drw, int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_TRIANGLE;
	drw->rect_flags = 0;
	
	drw->triangle.flags = flags;
	drw->triangle.x1 = x1;
	drw->triangle.y1 = y1;
	drw->triangle.x2 = x2;
	drw->triangle.y2 = y2;
	drw->triangle.x3 = x3;
	drw->triangle.y3 = y3;
	
	drw->rect = {
		.x = std::min({x1, x2, x3}),
		.y = std::min({y1, y2, y3}),
		.x2 = std::max({x1, x2, x3}),
		.y2 = std::max({y1, y2, y3})
	};
	
	GUI_DrawObjectSetColor(drw, pen, brush);
	
	return drw;
}

DRWOBJ *GUI_SetProp2Rect(DRWOBJ *drw, RECT *rect, int flags) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_RECT;
	memcpy(&drw->rect, rect, sizeof(RECT));
	drw->rect_flags = flags;
	
	return drw;
}

DRWOBJ *GUI_SetProp2RoundedRect(DRWOBJ *drw, RECT *rect, int flags, int round_x, int round_y) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_ROUNDED_RECT;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->rectangle.round_x = round_x;
	drw->rectangle.round_y = round_y;
	
	return drw;
}

DRWOBJ *GUI_SetProp2Arc(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int w, int h, int start, int end) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_ARC;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->arc.x = x;
	drw->arc.y = y;
	drw->arc.w = w;
	drw->arc.h = h;
	drw->arc.start = start;
	drw->arc.end = end;
	
	return drw;
}

DRWOBJ *GUI_SetProp2Pie(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int w, int h, int start, int end) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_PIE;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->arc.x = x;
	drw->arc.y = y;
	drw->arc.w = w;
	drw->arc.h = h;
	drw->arc.start = start;
	drw->arc.end = end;
	
	return drw;
}

DRWOBJ *GUI_SetProp2Pixel(DRWOBJ *drw, RECT *rect, int flags, int x, int y) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_PIXEL;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->line.x = x;
	drw->line.y = y;
	
	return drw;
}

DRWOBJ *GUI_SetProp2Line(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int x2, int y2) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_LINE;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->line.x = x;
	drw->line.y = y;
	drw->line.x2 = x2;
	drw->line.x2 = y2;
	
	return drw;
}

DRWOBJ *GUI_SetProp2EImage(DRWOBJ *drw, RECT *rect, int flags, EIMGHDR *img, int offset_x, int offset_y) {
	assert(drw != nullptr);
	assert(rect != nullptr);
	assert(img != nullptr);
	
	drw->type = DRWOBJ_TYPE_EIMG;
	drw->rect_flags = 0;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->eimg.value = img;
	drw->eimg.flags = flags;
	drw->eimg.offset_x = offset_x;
	drw->eimg.offset_y = offset_y;
	
	return drw;
}

DRWOBJ *GUI_DrawObjectSetColor(DRWOBJ *drw, const char *color1, const char *color2) {
	assert(drw != nullptr);
	
	if (color1) {
		memcpy(drw->color1, color1, 4);
	} else {
		memset(drw->color1, 0, 4);
	}
	
	if (color2) {
		memcpy(drw->color2, color2, 4);
	} else {
		memset(drw->color2, 0, 4);
	}
	
	return drw;
}

void GUI_FreeDrawObject(DRWOBJ *drw) {
	assert(drw != nullptr);
	if (drw->type == DRWOBJ_TYPE_TEXT) {
		if (drw->text.value) {
			FreeWS(drw->text.value);
			drw->text.value = nullptr;
		}
	}
}

void GUI_DrawObject(DRWOBJ *drw) {
	assert(drw != nullptr);
	
	Painter *painter = GUI_GetPainter();
	
	RECT *rect = &drw->rect;
	painter->setWindow(rect->x, rect->y, rect->x2, rect->y2);
	
	switch (drw->type) {
		case DRWOBJ_TYPE_RECT:
		{
			int w = rect->x2 - rect->x + 1;
			int h = rect->y2 - rect->y + 1;
			painter->drawRect(0, 0, w, h, GUI_Color2Int(drw->color1), GUI_Color2Int(drw->color2));
		}
		break;
		
		case DRWOBJ_TYPE_ROUNDED_RECT:
		{
			int w = rect->x2 - rect->x + 1;
			int h = rect->y2 - rect->y + 1;
			painter->drawRoundedRect(0, 0, w, h, drw->rectangle.round_x, drw->rectangle.round_y, GUI_Color2Int(drw->color1), GUI_Color2Int(drw->color2));
		}
		break;
		
		case DRWOBJ_TYPE_IMG:
		{
			IMGHDR *img = drw->img.value;
			painter->drawBitmap(0, 0, img->w, img->h, img->bitmap, IMG_GetBitmapType(img->bpnum),
				drw->img.offset_x, drw->img.offset_y, GUI_Color2Int(drw->color1), GUI_Color2Int(drw->color2));
		}
		break;
		
		case DRWOBJ_TYPE_PIXEL:
			painter->drawPixel(drw->line.x - rect->x, drw->line.y - rect->y, GUI_Color2Int(drw->color1));
		break;
		
		case DRWOBJ_TYPE_LINE:
			painter->drawLine(drw->line.x - rect->x, drw->line.y - rect->y, drw->line.x2 - rect->x, drw->line.y2 - rect->y, GUI_Color2Int(drw->color1));
		break;
		
		case DRWOBJ_TYPE_PIE:
			painter->drawArc(drw->arc.x - rect->x, drw->arc.y - rect->y, drw->arc.w, drw->arc.h, drw->arc.start, drw->arc.end, GUI_Color2Int(drw->color1), 0x00000000);
		break;
		
		case DRWOBJ_TYPE_ARC:
			painter->drawArc(drw->arc.x - rect->x, drw->arc.y - rect->y, drw->arc.w, drw->arc.h, drw->arc.start, drw->arc.end, 0x00000000, GUI_Color2Int(drw->color1));
		break;
		
		case DRWOBJ_TYPE_TRIANGLE:
		{
			painter->drawTriangle(
				drw->triangle.x1 - rect->x, drw->triangle.y1 - rect->y,
				drw->triangle.x2 - rect->x, drw->triangle.y2 - rect->y,
				drw->triangle.x3 - rect->x, drw->triangle.y3 - rect->y,
				GUI_Color2Int(drw->color2), GUI_Color2Int(drw->color1)
			);
		}
		break;
	}
	
	GUI_IpcRedrawScreen();
}
