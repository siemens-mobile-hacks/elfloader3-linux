#include "swi.h"
#include "utils.h"
#include "log.h"
#include "gui/Painter.h"
#include "TextRender.h"

#include <cassert>

DRWOBJ *GUI_SetProp2Text(DRWOBJ *drw, RECT *rect, int rect_flag, WSHDR *wshdr, int font, int flags) {
	assert(drw != nullptr && rect != nullptr && wshdr != nullptr);
	
	drw->type = DRWOBJ_TYPE_MULTILINE_TEXT;
	drw->rect_flags = rect_flag;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->text.flags = flags;
	drw->text.font = font;
	drw->text.value = AllocWS(wshdr->body->len + 1);
	memcpy(drw->text.value->wsbody, wshdr->wsbody, (wshdr->body->len + 1) * sizeof(uint16_t));
	
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
	memcpy(drw->text.value->wsbody, wshdr->wsbody, (wshdr->body->len + 1) * sizeof(uint16_t));
	
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

DRWOBJ *GUI_SetProp2RectEx(DRWOBJ *drw, RECT *rect, int flags, int fill_mode, int fill_value) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_RECT_EX;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->rectangle.fill_mode = fill_mode;
	drw->rectangle.fill_value = fill_value;
	
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
	drw->arc.flags = 0;
	
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
	drw->arc.flags = flags;
	
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
	drw->line.y2 = y2;
	
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
	if (drw->type == DRWOBJ_TYPE_MULTILINE_TEXT) {
		if (drw->text.value) {
			FreeWS(drw->text.value);
			drw->text.value = nullptr;
		}
	}
}

DRWOBJ *GUI_SetProp2StrokeEllipseSection(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int radius_x, int radius_y, int type) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_STROKE_ELLIPSE_SECTION;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->ellipse_section.x = x;
	drw->ellipse_section.y = y;
	drw->ellipse_section.radius_x = radius_x;
	drw->ellipse_section.radius_y = radius_y;
	drw->ellipse_section.flags = type;
	
	return drw;
}

DRWOBJ *GUI_SetProp2FilledEllipseSection(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int radius_x, int radius_y, int type) {
	assert(drw != nullptr);
	
	drw->type = DRWOBJ_TYPE_FILLED_ELLIPSE_SECTION;
	drw->rect_flags = flags;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	drw->ellipse_section.x = x;
	drw->ellipse_section.y = y;
	drw->ellipse_section.radius_x = radius_x;
	drw->ellipse_section.radius_y = radius_y;
	drw->ellipse_section.flags = type;
	
	return drw;
}

static int _ellipseHelperFlagsToPainter(int type) {
	switch (type) {
		case DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT:		return Painter::CIRCLE_DRAW_UPPER_LEFT;
		case DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT:		return Painter::CIRCLE_DRAW_LOWER_LEFT;
		case DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT:	return Painter::CIRCLE_DRAW_UPPER_RIGHT;
		case DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT:	return Painter::CIRCLE_DRAW_LOWER_RIGHT;
	}
	return -1;
}

void _explodeStringToWords(WSHDR *ws);

void GUI_DrawObject(DRWOBJ *drw) {
	assert(drw != nullptr);
	
	Painter *painter = GUI_GetPainter();
	
	RECT *rect = &drw->rect;
	painter->setWindow(rect->x, rect->y, rect->x2, rect->y2);
	
	if ((drw->rect_flags & DRWOBJ_RECT_FLAG_INVERT_BG))
		painter->setBlendMode(Painter::BLEND_MODE_INVERT);
	
	switch (drw->type) {
		case DRWOBJ_TYPE_SCROLLING_TEXT:
		{
			TextRender tr(painter, rect, drw->text.value->body->data, drw->text.value->body->len);
			tr.setPen(GUI_Color2Int(drw->color1));
			tr.setBrush(GUI_Color2Int(drw->color2));
			tr.setFlags(drw->text.flags);
			tr.setFont(drw->text.font);
			tr.renderInline(drw->text.xdisp);
		}
		break;
		
		case DRWOBJ_TYPE_MULTILINE_TEXT:
		{
			TextRender tr(painter, rect, drw->text.value->body->data, drw->text.value->body->len);
			tr.setPen(GUI_Color2Int(drw->color1));
			tr.setBrush(GUI_Color2Int(drw->color2));
			tr.setFlags(drw->text.flags);
			tr.setFont(drw->text.font);
			tr.render();
		}
		break;
		
		case DRWOBJ_TYPE_RECT:
		{
			int w = rect->x2 - rect->x + 1;
			int h = rect->y2 - rect->y + 1;
			painter->fillRect(0, 0, w, h, GUI_Color2Int(drw->color1));
			painter->strokeRect(0, 0, w, h, GUI_Color2Int(drw->color2));
		}
		break;
		
		case DRWOBJ_TYPE_RECT_EX:
		{
			int w = rect->x2 - rect->x + 1;
			int h = rect->y2 - rect->y + 1;
			
			switch (drw->rectangle.fill_mode) {
				case DRWOBJ_RECT_BG_TYPE_FILL:
					painter->fillRect(0, 0, w, h, GUI_Color2Int(drw->color1));
				break;
				
				case DRWOBJ_RECT_BG_TYPE_FILL2:
					painter->fillRect(0, 0, w, h, GUI_Color2Int(drw->color2));
				break;
				
				case DRWOBJ_RECT_BG_TYPE_PATTERN:
					painter->drawPattern(0, 0, w, h, drw->rectangle.fill_value, GUI_Color2Int(drw->color1), GUI_Color2Int(drw->color2));
				break;
			}
		}
		break;
		
		case DRWOBJ_TYPE_IMG:
		{
			IMGHDR *img = drw->img.value;
			painter->drawBitmap(0, 0, img->w, img->h, img->bitmap, IMG_GetBitmapType(img->bpnum),
				drw->img.offset_x, drw->img.offset_y, GUI_Color2Int(drw->color1), GUI_Color2Int(drw->color2));
		}
		break;
		
		case DRWOBJ_TYPE_STROKE_ELLIPSE_SECTION:
		case DRWOBJ_TYPE_FILLED_ELLIPSE_SECTION:
		{
			int x = drw->ellipse_section.x - rect->x;
			int y = drw->ellipse_section.y - rect->y;
			int draw_position;
			
			switch (drw->ellipse_section.flags) {
				case DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT:
					draw_position = Painter::CIRCLE_DRAW_UPPER_LEFT;
				break;
				
				case DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT:
					draw_position = Painter::CIRCLE_DRAW_LOWER_LEFT;
				break;
				
				case DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT:
					draw_position = Painter::CIRCLE_DRAW_UPPER_RIGHT;
				break;
				
				case DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT:
					draw_position = Painter::CIRCLE_DRAW_LOWER_RIGHT;
				break;
				
				default:
					LOGE("Invalid drw->ellipse_section.flags: %d\n", drw->ellipse_section.flags);
					abort();
				break;
			}
			
			uint32_t color = GUI_Color2Int(drw->color1);
			painter->startPerfectDrawing(color);
			if (drw->type == DRWOBJ_TYPE_FILLED_ELLIPSE_SECTION) {
				painter->fillEllipseHelper(x, y, drw->ellipse_section.radius_x - 1, drw->ellipse_section.radius_y - 1, draw_position, GUI_Color2Int(drw->color1));
			} else {
				painter->strokeEllipseHelper(x, y, drw->ellipse_section.radius_x - 1, drw->ellipse_section.radius_y - 1, draw_position, GUI_Color2Int(drw->color1));
			}
			painter->stopPerfectDrawing();
		}
		break;
		
		case DRWOBJ_TYPE_PIXEL:
			painter->drawPixel(drw->line.x - rect->x, drw->line.y - rect->y, GUI_Color2Int(drw->color1));
		break;
		
		case DRWOBJ_TYPE_LINE:
		{
			bool dotted = (drw->rect_flags & (LINE_DOTTED | LINE_DOTTED2)) != 0;
			painter->drawLine(drw->line.x - rect->x, drw->line.y - rect->y, drw->line.x2 - rect->x, drw->line.y2 - rect->y, GUI_Color2Int(drw->color1), dotted);
		}
		break;
		
		case DRWOBJ_TYPE_PIE:
			painter->fillArc(drw->arc.x - rect->x, drw->arc.y - rect->y, drw->arc.w, drw->arc.h, drw->arc.start, drw->arc.end, GUI_Color2Int(drw->color1));
		break;
		
		case DRWOBJ_TYPE_ARC:
		{
			bool dotted = (drw->arc.flags & RECT_DOT_OUTLINE) != 0;
			painter->strokeArc(drw->arc.x - rect->x, drw->arc.y - rect->y, drw->arc.w, drw->arc.h, drw->arc.start, drw->arc.end, GUI_Color2Int(drw->color1), dotted);
		}
		break;
		
		case DRWOBJ_TYPE_TRIANGLE:
		{
			painter->fillTriangle(
				drw->triangle.x1 - rect->x, drw->triangle.y1 - rect->y,
				drw->triangle.x2 - rect->x, drw->triangle.y2 - rect->y,
				drw->triangle.x3 - rect->x, drw->triangle.y3 - rect->y,
				GUI_Color2Int(drw->color2)
			);
			painter->strokeTriangle(
				drw->triangle.x1 - rect->x, drw->triangle.y1 - rect->y,
				drw->triangle.x2 - rect->x, drw->triangle.y2 - rect->y,
				drw->triangle.x3 - rect->x, drw->triangle.y3 - rect->y,
				GUI_Color2Int(drw->color1)
			);
		}
		break;
	}
	
	if ((drw->rect_flags & DRWOBJ_RECT_FLAG_INVERT_BG))
		painter->setBlendMode(Painter::BLEND_MODE_NORMAL);
	
	GUI_IpcRedrawScreen();
}
