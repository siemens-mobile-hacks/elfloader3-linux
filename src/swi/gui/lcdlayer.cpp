#include "src/IPC.h"
#include "src/gui/Bitmap.h"
#include "src/swi/gui.h"
#include "swilib/gbs.h"
#include <cstdint>
#include <cstring>
#include <swilib/gui.h>
#include <swilib/wstring.h>
#include <spdlog/spdlog.h>

static LCDLAYER main_lcd_layer = {};
static LCDLAYER_LIST lcd_layers_list[16] = {};
static RAP_LCDLAYER_LIST rap_lcd_layers_list[16] = {};
static int GetLayerIdByCepId(int *layer);

void LCDLAYER_Init() {
	LCDLAYER_Create(&main_lcd_layer, nullptr, LCDLAYER_DEPTH_BGR565, 0, nullptr, ScreenW(), ScreenH());
}

static int LCDLAYER_CalcBufferSize(int depth, int w, int h) {
	switch (depth) {
		case LCDLAYER_DEPTH_WB:			return (w + 7) / 8 + h;
		case LCDLAYER_DEPTH_BGR233:		return w * h;
		case LCDLAYER_DEPTH_BGRA4444:	return w * h * 2;
		case LCDLAYER_DEPTH_BGR565:		return w * h * 2;
		case LCDLAYER_DEPTH_BGRA8888:	return w * h * 4;
	}
	return 0;
}

void LCDLAYER_Create(LCDLAYER *layer, void *unk, int depth, uint8_t type, void *buffer, int width, int height) {
	memset(layer, 0, sizeof(LCDLAYER));
	layer->depth = depth;
	layer->w = width;
	layer->h = height;

	if (!buffer) {
		layer->allocated_size = LCDLAYER_CalcBufferSize(depth, width, height);
		layer->buffer = malloc(layer->allocated_size);
	} else {
		layer->allocated_size = 0;
		layer->buffer = nullptr;
	}

	LCDLAYER_ResetInvalidateRegion(layer);
	StoreXYXYtoRECT(&layer->rect, 0, 0, layer->w - 1, layer->h - 1);

	switch (type) {
		case 0:
			lcd_layers_list[0].layer = &main_lcd_layer;
			lcd_layers_list[0].malloc_func = malloc;
			lcd_layers_list[0].mfree_func = free;
			lcd_layers_list[0].cepid = 0x0500;
		break;

		case 2:
			lcd_layers_list[1].layer = &main_lcd_layer;
			lcd_layers_list[1].malloc_func = malloc;
			lcd_layers_list[1].mfree_func = free;
			lcd_layers_list[1].cepid = 0x0500;
		break;

		case 3:
			lcd_layers_list[2].layer = &main_lcd_layer;
			lcd_layers_list[2].malloc_func = malloc;
			lcd_layers_list[2].mfree_func = free;
			lcd_layers_list[2].cepid = 0x0500;
		break;
	}
}

LCDLAYER_LIST *RamLcdMainLayersList() {
	return lcd_layers_list;
}

RAP_LCDLAYER_LIST *RamLcdRapLayersList() {
	return rap_lcd_layers_list;
}

void LCDLAYER_Flush(LCDLAYER *layer) {
	uint32_t *target = reinterpret_cast<uint32_t *>(IPC::instance()->getScreenBuffer());
	int w = layer->w;
	int h = layer->h;
	int x1 = std::max(0, std::min((int) layer->invalidate.x, w - 1));
	int y1 = std::max(0, std::min((int) layer->invalidate.y, h - 1));
	int x2 = std::max(0, std::min((int) layer->invalidate.x2, w - 1));
	int y2 = std::max(0, std::min((int) layer->invalidate.y2, h - 1));
	auto *source = reinterpret_cast<uint8_t *>(layer->buffer);
	auto type = LCDLAYER_GetBitmapType(layer);
	LCDLAYER_ResetInvalidateRegion(layer);

	for (int y = y1; y <= y2; y++) {
		int index = y * w;
		for (int x = x1; x <= x2; x++) {
			target[index + x] = Bitmap::getBitmapPixel(type, x, y, w, h, source);
		}
	}

	if (x1 <= x2 && y1 <= y2) {
		IPC::instance()->sendRedraw(x1, y1, x2, y2);
	}
}

void LCDLAYER_Redraw(LCDLAYER *layer) {
	if (layer->redraw_requested == 0) {
		GBS_SendMessage(LCD_DISPLAYQUEUE_CEPID, LCD_DISPLAYQUEUE_CMD_REDRAW, 0, layer, 0);
		layer->redraw_requested = 1;
	}
}

void LCDLAYER_SetBufferDepth(LCDLAYER *layer, char depth) {
	spdlog::debug("{}: not implemented!", __func__);
}

LCDLAYER *LCD_GetCurrentLayer() {
	int layer_id;
	if (GetLayerIdByCepId(&layer_id))
		return lcd_layers_list[layer_id].layer;
	return nullptr;
}

void LCD_SetBufferDepth(char depth) {
	auto *layer = LCD_GetCurrentLayer();
	if (layer) {
		LCDLAYER_SetBufferDepth(layer, depth);
	}
}

void LCD_GetDrawingRegion(int *x, int *y, int *x2, int *y2) {
	auto *layer = LCD_GetCurrentLayer();
	assert(layer != nullptr);
	if (layer) {
		*x = layer->rect.x;
		*y = layer->rect.y;
		*x2 = layer->rect.x2;
		*y2 = layer->rect.y2;
	}
}

void LCD_SetDrawingRegion(int x, int y, int x2, int y2) {
	auto *layer = LCD_GetCurrentLayer();
	assert(layer != nullptr);
	if (layer) {
		layer->rect.x = x;
		layer->rect.y = y;
		layer->rect.x2 = x2;
		layer->rect.y2 = y2;
	}
}

void LCDLAYER_ResetInvalidateRegion(LCDLAYER *layer) {
	layer->invalidate.x = layer->w - 1;
	layer->invalidate.y = layer->h - 1;
	layer->invalidate.x2 = 0;
	layer->invalidate.y2 = 0;
}

void LCDLAYER_InvalidateRegion(LCDLAYER *layer, int x, int y, int x2, int y2) {
	if (x <= layer->rect.x)
		x = layer->rect.x;

	if (layer->rect.x2 <= x2)
		x2 = layer->rect.x2;

	if (y <= layer->rect.y)
		y = layer->rect.y;

	if (layer->rect.y2 <= y2)
		y2 = layer->rect.y2;

	if ((x <= x2) && (y <= y2)) {
		if (x < 0)
			x = 0;
		if (x < layer->invalidate.x)
			layer->invalidate.x = x;
		if (y < 0)
			y = 0;
		if (y < layer->invalidate.y)
			layer->invalidate.y = y;
		if (layer->w <= x2)
			x2 = layer->w - 1;
		if (layer->invalidate.x2 < x2)
			layer->invalidate.x2 = x2;
		if (layer->h <= y2)
			y2 = layer->h - 1;
		if (layer->invalidate.y2 < y2)
			layer->invalidate.y2 = y2;
	}
}

int GetLayerIdByCepId(int *layer) {
	int cepid = GBS_GetCurCepid();

	auto *layers = RamLcdMainLayersList();
	if (layers[0].cepid == cepid) {
		*layer = 0;
		return 1;
	}

	switch (cepid) {
		case 0x4209:
		case 0x4202:
		case 0x4200:
		case 0xFFFF:
			*layer = 0;
			return 1;
		break;

		case 0x4228:
		case 0x422D:
			*layer = 2;
			return 1;
		break;
	}

	*layer = 4;
	return 0;
}
