#include "swi.h"
#include "log.h"
#include "utils.h"
#include "SieFs.h"
#include "gui/Painter.h"

#include <stb_image.h>
#include <cstdio>

IMGHDR *IMG_LoadAny(const std::string &path) {
	int w, h, n;
	uint8_t *bitmap = stbi_load(path.c_str(), &w, &h, &n, 4);
	
	if (!bitmap) {
		LOGE("%s(%s): failed to load images.\n", __func__, path.c_str());
		return nullptr;
	}
	
	IMGHDR *new_img = (IMGHDR *) malloc(sizeof(IMGHDR));
	new_img->w = w;
	new_img->h = h;
	new_img->bpnum = IMGHDR_TYPE_RGB8888;
	new_img->bitmap = bitmap;
	
	// ABGR -> ARGB
	uint32_t *pixels = reinterpret_cast<uint32_t *>(bitmap);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			uint32_t index = w * y + x;
			uint32_t pixel = pixels[index];
			
			uint8_t r = pixel & 0xFF;
			uint8_t g = (pixel >> 8) & 0xFF;
			uint8_t b = (pixel >> 16) & 0xFF;
			uint8_t a = (pixel >> 24) & 0xFF;
			
			pixels[index] = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}
	
	return new_img;
}

int IMG_CalcBitmapSize(short w, short h, char type) {
	switch (type) {
		case IMGHDR_TYPE_WB:		return (w * h + 7) / 8;
		case IMGHDR_TYPE_RGB332:	return w * h;
		case IMGHDR_TYPE_RGB565:	return w * h * 2;
		case IMGHDR_TYPE_RGB8888:	return w * h * 4;
	}
	fprintf(stderr, "%s: unknown bitmap type: %d\n", __func__, type);
	abort();
	return 0;
}

IMGHDR *IMG_CreateIMGHDRFromPngFile(const char *fname, int type) {
	return IMG_LoadAny(SieFs::sie2path(fname));
}

Bitmap::Type IMG_GetBitmapType(int bpnum) {
	switch (bpnum) {
		case IMGHDR_TYPE_WB:		return Bitmap::TYPE_WB;
		case IMGHDR_TYPE_RGB332:	return Bitmap::TYPE_RGB332;
		case IMGHDR_TYPE_RGB565:	return Bitmap::TYPE_RGB565;
		case IMGHDR_TYPE_RGB8888:	return Bitmap::TYPE_RGB8888;
	}
	LOGE("Invalid bitmap type: %d\n", bpnum);
	abort();
	return Bitmap::TYPE_RGB8888;
}
