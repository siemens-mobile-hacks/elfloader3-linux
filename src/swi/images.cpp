#include "swi.h"
#include "log.h"
#include "utils.h"
#include "SieFs.h"

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
	
	return new_img;
}

int IMG_CalcBitmapSize(short w, short h, char type) {
	switch (type) {
		case IMGHDR_TYPE_WB:		return (w * h + 7) / 8; // WB
		case IMGHDR_TYPE_RGB332:	return w * h; // RGB332
		case IMGHDR_TYPE_RGB565:	return w * h * 2; // RGB565
		case IMGHDR_TYPE_RGB8888:	return w * h * 4; // RGB8888
	}
	fprintf(stderr, "%s: unknown bitmap type: %d\n", __func__, type);
	abort();
	return 0;
}

IMGHDR *IMG_CreateIMGHDRFromPngFile(const char *fname, int type) {
	return IMG_LoadAny(SieFs::sie2path(fname));
}
