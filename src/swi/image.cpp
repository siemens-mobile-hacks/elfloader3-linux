#include <cstring>
#include <spdlog/spdlog.h>
#include <stdint.h>
#include <time.h>
#include <swilib/image.h>
#include <stb_image.h>
#include "src/FFS.h"
#include "src/swi/image.h"
#include "src/Resources.h"

static PNGTOP_DESC png_top = {};

IMGHDR *GetPITaddr(int picture) {
	return Resources::instance()->getPicture(picture);
}

int GetImgHeight(int picture) {
	IMGHDR *img = Resources::instance()->getPicture(picture);
	return img ? img->h : 0;
}

int GetImgWidth(int picture) {
	IMGHDR *img = Resources::instance()->getPicture(picture);
	return img ? img->w : 0;
}

void MMI_CanvasBuffer_FlushV(char canvas_id) {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsCanvasBufferSet(char canvas_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

IMGHDR *GetCanvasBufferPicPtr(char canvas_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

PNGTOP_DESC *PNG_TOP(void) {
	return &png_top;
}

int GetPicNByUnicodeSymbol(int symbol) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

IMGHDR *CreateIMGHDRFromPngFile(const char *path, int type) {
	return IMG_LoadAny(FFS::dos2unix(path));
}

int GetMaxDynIcon(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void FreeDynIcon(int picture) {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsDynIconBusy(int picture) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SetDynIcon(int picture, IMGHDR *img, char *bitmap) {
	spdlog::debug("{}: not implemented!", __func__);
}

IMGHDR *GetPitAdrBy0xE200Symbol(int symbol) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int CalcBitmapSize(short w, short h, char type) {
	switch (type) {
		case IMGHDR_TYPE_WB:		return ((w + 7) / 8) * h;
		case 2:						return ((w + 3) / 4) * h; // WTF
		case 3:						return ((w + 3) / 4) * h; // WTF
		case IMGHDR_TYPE_BGR233:	return w * h;
		case 6:						return ((w + 7) / 8 + 8) * h;
		case 7:						return w * h * 2; // WTF
		case IMGHDR_TYPE_BGR565:	return w * h * 2;
		case 9:						return w * h * 2; // WTF
		case IMGHDR_TYPE_BGRA8888:	return w * h * 4;
	}
	spdlog::error("{}: unknown bitmap type: {}", __func__, type);
	abort();
	return 0;
}

IMGHDR *GetIMGHDRFromThemeCache(int index) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

IMGHDR *GetIMGHDRFromCanvasCache(char canvas_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

IMGHDR *IMG_LoadAny(const std::string &path, bool only_rgb888) {
	int w, h, n;
	uint8_t *bitmap = stbi_load(path.c_str(), &w, &h, &n, 4);

	if (!bitmap) {
		spdlog::error("{}({}): failed to load image.", __func__, path);
		return nullptr;
	}

	IMGHDR *new_img = (IMGHDR *) malloc(sizeof(IMGHDR));
	new_img->w = w;
	new_img->h = h;
	new_img->bpnum = IMGHDR_TYPE_BGRA8888;
	new_img->bitmap = bitmap;

	bool is_bw = true;

	// ABGR -> ARGB
	uint32_t *pixels = reinterpret_cast<uint32_t *>(bitmap);
	for (int y = 0; y < h; y++) {
		int row_start = w * y;
		for (int x = 0; x < w; x++) {
			uint32_t index = row_start + x;
			uint32_t pixel = pixels[index];

			if (is_bw && pixel != 0xFF000000 && pixel != 0xFFFFFFFF)
				is_bw = false;

			uint8_t r = pixel & 0xFF;
			uint8_t g = (pixel >> 8) & 0xFF;
			uint8_t b = (pixel >> 16) & 0xFF;
			uint8_t a = (pixel >> 24) & 0xFF;

			pixels[index] = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}

	if (is_bw && !only_rgb888) {
		int bitmap_bw_size = CalcBitmapSize(w, h, IMGHDR_TYPE_WB);
		uint8_t *bitmap_bw = (uint8_t *) malloc(bitmap_bw_size);
		memset(bitmap_bw, 0, bitmap_bw_size);

		new_img->bpnum = IMGHDR_TYPE_WB;
		new_img->bitmap = bitmap_bw;

		int bw_row = (w + 7) / 8;

		for (int y = 0; y < h; y++) {
			int row_start = w * y;
			int bw_row_start = y * (bw_row * 8);

			for (int x = 0; x < w; x++) {
				int current_bit = bw_row_start + x;
				int current_byte = current_bit / 8;
				int current_shift = 7 - (current_bit - current_byte * 8);
				bitmap_bw[current_byte] |= pixels[row_start + x] & 0xFFFFFF ? 1 << current_shift : 0;
			}
		}

		free(bitmap);
	}

	return new_img;
}

Bitmap::Type IMG_GetBitmapType(int bpnum) {
	switch (bpnum) {
		case IMGHDR_TYPE_WB:		return Bitmap::TYPE_WB;
		case IMGHDR_TYPE_BGR233:	return Bitmap::TYPE_BGR233;
		case IMGHDR_TYPE_BGR565:	return Bitmap::TYPE_BGR565;
		case IMGHDR_TYPE_BGRA8888:	return Bitmap::TYPE_BGRA8888;
	}
	spdlog::error("Invalid bitmap type: {}", bpnum);
	abort();
}
