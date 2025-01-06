#include <swilib/gui.h>
#include <spdlog/spdlog.h>

#include "src/Resources.h"
#include "src/swi/gui.h"

void DrawImg(int x, int y, int picture) {
	DrawImgBW(x, y, picture, GetPaletteAdrByColorIndex(100), GetPaletteAdrByColorIndex(0x65));
}

void DrawCanvas(void *data, int x, int y, int x2, int y2, int flag_one) {
	spdlog::debug("{}: not implemented!", __func__);
}

void DrawImgBW(int x, int y, int picture, const char *pen, const char *brush) {
	IMGHDR *img = Resources::instance()->getPicture(picture);
	if (img)
		DrawIMGHDREx(x, y, img, pen, brush, 0, 0, img->w, img->h, 0);
}

void DrawString(WSHDR *text, int x1, int y1, int x2, int y2, int font, int text_flags, const char *pen, const char *brush) {
	assert(text != nullptr);

	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

	if ((text_flags & TEXT_OUTLINE)) {
		const char *transparent = GetPaletteAdrByColorIndex(0x17);

		text_flags &= ~(TEXT_INVERT | TEXT_INVERT2 | TEXT_OUTLINE);

		for (int y_offset = 0; y_offset < 3; y_offset++) {
			for (int x_offset = 0; x_offset < 3; x_offset++) {
				if (y_offset == 1 && x_offset == 1)
					continue;
				DrawString(text, rect.x + x_offset, rect.y + y_offset, rect.x2 + x_offset, rect.y2 + y_offset, font, text_flags, brush, transparent);
			}
		}
		DrawString(text, rect.x + 1, rect.y + 1, rect.x2 + 1, rect.y2 + 1, font, text_flags, pen, transparent);
	} else {
		DrwObj_InitText(&drw, &rect, 0, text, font, text_flags);
		DrwObj_SetColor(&drw, pen, brush);
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}
}

void DrawScrollString(WSHDR *text, int x1, int y1, int x2, int y2, int xdisp, int font, int text_flags, const char *pen, const char *brush) {
	assert(text != nullptr);

	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

	if ((text_flags & TEXT_OUTLINE)) {
		const char *transparent = GetPaletteAdrByColorIndex(0x17);

		text_flags &= ~(TEXT_INVERT | TEXT_INVERT2 | TEXT_OUTLINE);

		for (int y_offset = 0; y_offset < 3; y_offset++) {
			for (int x_offset = 0; x_offset < 3; x_offset++) {
				if (y_offset == 1 && x_offset == 1)
					continue;
				DrawScrollString(text, rect.x + x_offset, rect.y + y_offset, rect.x2 + x_offset, rect.y2 + y_offset, xdisp, font, text_flags, brush, transparent);
			}
		}
		DrawScrollString(text, rect.x + 1, rect.y + 1, rect.x2 + 1, rect.y2 + 1, xdisp, font, text_flags, pen, transparent);
	} else {
		rect.x2 += 1;
		rect.y2 += 1;

		DrwObj_InitScrollingText(&drw, &rect, 0, text, xdisp, font, text_flags);
		DrwObj_SetColor(&drw, pen, brush);
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}
}

void DrawRectangle(int x1, int y1, int x2, int y2, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

	if (!GUI_ColorIsTransparent(brush)) {
		if ((flags & RECT_DRAW_INVERT)) {
			DrwObj_InitRect(&drw, &rect, DRWOBJ_RECT_FLAG_INVERT_BG);
		} else if ((flags & RECT_FILL_WITH_PEN)) {
			DrwObj_InitRectEx(&drw, &rect, 0, DRWOBJ_RECT_FILL_TYPE_PATTERN, 0x55);
		} else {
			DrwObj_InitRectEx(&drw, &rect, 0, DRWOBJ_RECT_FILL_TYPE_BRUSH, 0);
		}
		DrwObj_SetColor(&drw, brush, pen);
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}

	bool only_bg = (flags & RECT_DRAW_INVERT) || ((flags & RECT_FILL_WITH_PEN) && (flags & RECT_DOT_OUTLINE));
	if (pen != brush && !GUI_ColorIsTransparent(pen) && !only_bg) {
		int line_flags = flags & ~RECT_FILL_WITH_PEN;
		DrawLine(rect.x, rect.y, rect.x2, rect.y, line_flags, pen);
		DrawLine(rect.x, rect.y2, rect.x2, rect.y2, line_flags, pen);
		DrawLine(rect.x, rect.y, rect.x, rect.y2, line_flags, pen);
		DrawLine(rect.x2, rect.y, rect.x2, rect.y2, line_flags, pen);
	}
}

void DrawRoundedFrame(int x1, int y1, int x2, int y2, int round_x, int round_y, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect2;
	RECT rect;
	StoreXYXYtoRECT(&rect, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

	int w = rect.x2 - rect.x + 1;
	int h = rect.y2 - rect.y + 1;

	if (round_x < 2 || round_y < 2 || w < 4 || h < 4) {
		DrawRectangle(x1, y1, x2, y2, flags, pen, brush);
		return;
	}

	round_x = std::min(w / 2, round_x);
	round_y = std::min(h / 2, round_y);

	int xl = rect.x + round_x;
	int yu = rect.y + round_y;

	int xr = rect.x2 - round_x;
	int yl = rect.y2 - round_y;

	if (!GUI_ColorIsTransparent(brush)) {
		DrwObj_SetColor(&drw, brush, brush);

		DrwObj_InitFilledEllipseSection(&drw, &rect, 1, xr + 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT);
		DrawObject(&drw);

		DrwObj_InitFilledEllipseSection(&drw, &rect, 1, xl - 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT);
		DrawObject(&drw);

		DrwObj_InitFilledEllipseSection(&drw, &rect, 1, xl - 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT);
		DrawObject(&drw);

		DrwObj_InitFilledEllipseSection(&drw, &rect, 1, xr + 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT);
		DrawObject(&drw);

		if (xr - xl >= 0) {
			StoreXYXYtoRECT(&rect2, xl, rect.y, xr, yu - 1);
			DrwObj_InitRectEx(&drw, &rect2, 0, 1, 0);
			DrawObject(&drw);
		}

		if (yl - yu >= 0) {
			StoreXYXYtoRECT(&rect2, rect.x, yu, rect.x2, yl);
			DrwObj_InitRectEx(&drw, &rect2, 0, 1, 0);
			DrawObject(&drw);
		}

		if (xr - xl >= 0) {
			StoreXYXYtoRECT(&rect2, xl, yl + 1, xr, rect.y2);
			DrwObj_InitRectEx(&drw, &rect2, 0, 1, 0);
			DrawObject(&drw);
		}
	}

	if (!GUI_ColorIsTransparent(pen) && !(flags & DRWOBJ_RECT_FLAG_INVERT_BG)) {
		DrwObj_SetColor(&drw, pen, pen);

		DrwObj_InitStrokeEllipseSection(&drw, &rect, 1, xr + 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT);
		DrawObject(&drw);

		DrwObj_InitStrokeEllipseSection(&drw, &rect, 1, xl - 1, yu - 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT);
		DrawObject(&drw);

		DrwObj_InitStrokeEllipseSection(&drw, &rect, 1, xl - 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT);
		DrawObject(&drw);

		DrwObj_InitStrokeEllipseSection(&drw, &rect, 1, xr + 1, yl + 1, round_x, round_y, DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT);
		DrawObject(&drw);

		if (xr - xl >= 0) {
			DrawLine(xl, rect.y, xr + 1, rect.y, 0, pen);
			DrawLine(xl, rect.y2, xr + 1, rect.y2, 0, pen);
		}

		if (yl - yu >= 0) {
			DrawLine(rect.x, yu, rect.x, yl + 1, 0, pen);
			DrawLine(rect.x2, yu, rect.x2, yl + 1, 0, pen);
		}
	}
}

void DrawLine(int x1, int y1, int x2, int y2, int flags, const char *pen) {
	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

	if (!GUI_ColorIsTransparent(pen)) {
		DrwObj_InitLine(&drw, &rect, (flags & RECT_DOT_OUTLINE), x1, y1, x2, y2);
		(reinterpret_cast<DRWOBJ_PRIVATE *>(&drw))->line.flags = flags;
		DrwObj_SetColor(&drw, pen, nullptr);
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}
}

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	DrwObj_InitTriangle(&drw, x1, y1, x2, y2, x3, y3, flags, pen, brush);
	DrawObject(&drw);
	DrwObj_Free(&drw);
}

void DrawPixel(int x, int y, const char *color) {
	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, 0, 0, x, y);

	if (!GUI_ColorIsTransparent(color)) {
		DrwObj_InitPixel(&drw, &rect, 0, x, y);
		DrwObj_SetColor(&drw, color, nullptr);
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}
}

void DrawArc(int x1, int y1, int x2, int y2, int start_angle, int end_start, int flags, const char *pen, const char *brush) {
	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, x1, y1, x2, y2);

	if (!GUI_ColorIsTransparent(brush)) {
		DrwObj_InitPie(&drw, &rect, 0, x1, y1, x2 - x1 + 1, y2 - y1 + 1, start_angle, end_start);
		DrwObj_SetColor(&drw, brush, brush);
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}

	if (!GUI_ColorIsTransparent(pen)) {
		DrwObj_InitArc(&drw, &rect, 0, x1, y1, x2 - x1 + 1, y2 - y1 + 1, start_angle, end_start);
		DrwObj_SetColor(&drw, pen, pen);
		(reinterpret_cast<DRWOBJ_PRIVATE *>(&drw))->arc.flags = flags;
		DrawObject(&drw);
		DrwObj_Free(&drw);
	}
}

void DrawIMGHDR(int x, int y, const IMGHDR *img) {
	auto *brush = GetPaletteAdrByColorIndex(0x65);
	auto *pen = GetPaletteAdrByColorIndex(100);
	DrawIMGHDREx(x, y, img, pen, brush, 0, 0, img->w, img->h, 0);
}

void DrawCroppedIMGHDR(int x, int y, int offset_x, int offset_y, int w, int h, int flags, const IMGHDR *img) {
	auto *brush = GetPaletteAdrByColorIndex(0x65);
	auto *pen = GetPaletteAdrByColorIndex(100);
	DrawIMGHDREx(x, y, img, pen, brush, offset_x, offset_y, w, h, flags);
}

void DrawIMGHDREx(int x, int y, const IMGHDR *img, const char *pen, const char *brush, uint16_t offset_x, uint16_t offset_y, int w, int h, int flags) {
	DRWOBJ drw;
	RECT rect;
	StoreXYXYtoRECT(&rect, x, y, x + w - 1, y + h - 1);

	assert(img != nullptr);
	assert(img->bitmap != nullptr);

	static const char black[] = { 0x00, 0x00, 0x00, 0x64 };
	static const char transparent[] = { 0x00, 0x00, 0x00, 0x00 };

	pen = pen ?: black;
	brush = brush ?: transparent;

	DrwObj_InitTiledImage(&drw, &rect, 0, img, offset_x, offset_y);
	DrwObj_SetColor(&drw, pen, brush);
	DrawObject(&drw);
	DrwObj_Free(&drw);
}
