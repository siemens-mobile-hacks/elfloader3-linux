#pragma once

#include "src/gui/Painter.h"
#include <swilib/gui.h>

#define LCD_DISPLAYQUEUE_CEPID			0x4200
#define LCD_DISPLAYQUEUE_CMD_REDRAW		0x6423
#define MMI_CMD_REDRAW					0x6404

enum {
	DRWOBJ_RECT_FLAG_INVERT_BG	= 0x20,
};

enum {
	DRWOBJ_TYPE_RECT						= 0x00,
	DRWOBJ_TYPE_MULTILINE_TEXT				= 0x01,
	DRWOBJ_TYPE_SCROLLING_TEXT				= 0x03,
	DRWOBJ_TYPE_RECT_EX						= 0x04,
	DRWOBJ_TYPE_IMG							= 0x05,
	DRWOBJ_TYPE_PIXEL						= 0x0B,
	DRWOBJ_TYPE_LINE						= 0x0F,
	DRWOBJ_TYPE_STROKE_ELLIPSE_SECTION		= 0x10,
	DRWOBJ_TYPE_FILLED_ELLIPSE_SECTION		= 0x11,
	DRWOBJ_TYPE_PIE							= 0x13,
	DRWOBJ_TYPE_ARC							= 0x12,
	DRWOBJ_TYPE_TRIANGLE					= 0x15,
	DRWOBJ_TYPE_EIMG						= 0x17,
};

struct DRWOBJ_PRIVATE {
	uint8_t type;
	uint8_t rect_flags;
	uint8_t unk2;
	char color1[4];
	char color2[4];
	uint8_t unk3;
	RECT rect;

	union {
		char dummy[0x10];

		struct {
			WSHDR *value;
			int font;
			uint16_t flags;
			uint16_t xdisp;
		} text;

		struct {
			const IMGHDR *value;
			uint8_t font;
			uint8_t offset_x;
			uint8_t offset_y;
			uint8_t unk5;
			uint16_t flags;
		} img;

		struct {
			const void *value;
			int offset_x;
			int offset_y;
			uint16_t flags;
		} eimg;

		struct {
			int16_t x;
			int16_t y;
			int16_t x2;
			int16_t y2;
			uint16_t flags;
		} line;

		struct {
			int16_t x1;
			int16_t y1;
			int16_t x2;
			int16_t y2;
			int16_t x3;
			int16_t y3;
			uint16_t flags;
		} triangle;

		struct {
			int16_t x;
			int16_t y;
			int16_t w;
			int16_t h;
			int16_t start;
			int16_t end;
			uint16_t flags;
		} arc;

		struct {
			int16_t x;
			int16_t y;
			int16_t radius_x;
			int16_t radius_y;
			uint16_t flags;
		} ellipse_section;

		struct {
			uint8_t fill_mode;
			uint8_t fill_value;
		} rectangle;
	};
};

static_assert(sizeof(DRWOBJ_PRIVATE) == sizeof(DRWOBJ), "Invalid DRWOBJ_PRIVATE size!");

void GUI_Init();
void GUI_SyncStates();
void GUI_GarbageCollector();

void LCD_GetDrawingRegion(int *x, int *y, int *x2, int *y2);
void LCDLAYER_Init();
void LCDLAYER_Create(LCDLAYER *layer, void *unk, int depth, uint8_t type, void *buffer, int width, int height);
void LCDLAYER_InvalidateRegion(LCDLAYER *layer, int x, int y, int x2, int y2);
void LCDLAYER_ResetInvalidateRegion(LCDLAYER *layer);

static inline Bitmap::Type LCDLAYER_GetBitmapType(LCDLAYER *layer) {
	switch (layer->depth) {
		case LCDLAYER_DEPTH_BGR233:		return Bitmap::TYPE_BGR233;
		case LCDLAYER_DEPTH_BGRA4444:	return Bitmap::TYPE_BGRA4444;
		case LCDLAYER_DEPTH_BGR565:		return Bitmap::TYPE_BGR565;
		default:
		case LCDLAYER_DEPTH_BGRA8888:	return Bitmap::TYPE_BGRA8888S;
	}
}

int GUI_GetTopID();
GUI *GUI_GetFocusedTop();
bool GUI_IsOnTop(int id);
void GUI_DoFocus(int id);
void GUI_DoUnFocus(int id);
GUI_RAM *GUI_GetById(int id);
GUI_RAM *GUI_GetPrev(int id);
GUI_RAM *GUI_GetNext(int id);
void GUI_Close(int id);
void GUI_HandleKeyPress(GBS_MSG *msg);
void GUI_HandlePendedRedraws();
uint32_t GUI_Color2Int(const char *color);
Painter *GUI_GetPainter();

void DrwObj_InitScrollingText(DRWOBJ *drw, const RECT *rect, int rect_flag, const WSHDR *wshdr, int xdisp, int font, int flags);
void DrwObj_InitStrokeEllipseSection(DRWOBJ *drw, const RECT *rect, int flags, int x, int y, int radius_x, int radius_y, int type);
void DrwObj_InitFilledEllipseSection(DRWOBJ *drw, const RECT *rect, int flags, int x, int y, int radius_x, int radius_y, int type);
void DrwObj_InitTriangle(DRWOBJ *drw, int x1, int y1, int x2, int y2, int x3, int y3, int flags, const char *pen, const char *brush);
void DrwObj_InitLine(DRWOBJ *drw, const RECT *rect, int flags, int x, int y, int x2, int y2);
void DrwObj_InitPixel(DRWOBJ *drw, const RECT *rect, int flags, int x, int y);
void DrwObj_InitArc(DRWOBJ *drw, const RECT *rect, int flags, int x, int y, int w, int h, int start, int end);
void DrwObj_InitPie(DRWOBJ *drw, const RECT *rect, int flags, int x, int y, int w, int h, int start, int end);

void DrawIMGHDREx(int x, int y, const IMGHDR *img, const char *pen, const char *brush, uint16_t offset_x, uint16_t offset_y, int w, int h, int flags);

inline bool GUI_ColorIsTransparent(const char *color) {
	return !color || color[3] == 0;
}
