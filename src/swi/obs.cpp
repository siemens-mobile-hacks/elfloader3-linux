#include "swi.h"
#include "utils.h"
#include "log.h"
#include "SieFs.h"

#include <cstdio>
#include <cassert>
#include <stb_image_resize2.h>

struct HObj {
	int uid_in = 0;
	int uid_out = 0;
	bool sync = false;
	std::string input_file;
	int msg_id = 0;
	int image_out_width = 0;
	int image_out_height = 0;
	int image_rotation = 0;
	IMGHDR *image = nullptr;
};

HObj *Obs_CreateObject(int uid_in, int uid_out, int prio, int msg_callback, int emb4, int sync, int *errp) {
	int err = 0;
	HObj *hobj = new HObj();
	hobj->uid_in = uid_in;
	hobj->uid_out = uid_out;
	hobj->sync = sync;
	hobj->msg_id = msg_callback;
	
	if (errp)
		*errp = err;
	
	return (err == 0 ? hobj : nullptr);
}

int Obs_SetInput_File(HObj *hobj, int unk_zero, WSHDR *path) {
	hobj->input_file = ws2string(path);
	if (!isFileExists(SieFs::sie2path(hobj->input_file))) {
		LOGE("%s: %s not found\n", __func__, hobj->input_file.c_str());
		return 1;
	}
	return 0;
}

int Obs_Start(HObj *hobj) {
	if (hobj->uid_out == UID_ZOOM_PIC) {
		hobj->image = IMG_LoadAny(SieFs::sie2path(hobj->input_file));
		if (!hobj->image)
			return 1;
		
		// Supported only RGB8888
		assert(hobj->image->bpnum == IMGHDR_TYPE_RGB8888);
		
		// Do resize
		if (hobj->image_out_width && hobj->image_out_height) {
			if (hobj->image_out_width != hobj->image->w || hobj->image_out_height != hobj->image->h) {
				uint8_t *new_bitmap = (uint8_t *) malloc(IMG_CalcBitmapSize(hobj->image_out_width, hobj->image_out_height, hobj->image->bpnum));
				
				stbir_resize_uint8_linear(
					hobj->image->bitmap, hobj->image->w, hobj->image->h, 0,
					new_bitmap, hobj->image_out_width, hobj->image_out_height, 0,
					STBIR_RGBA
				);
				
				free(hobj->image->bitmap);
				hobj->image->bitmap = new_bitmap;
			}
		}
		
		// TODO: gbs message
	} else {
		fprintf(stderr, "%s not implemented! (uid_out=%04X)\n", __func__, hobj->uid_out);
		abort();
	}
	return 0;
}

int Obs_DestroyObject(HObj *hobj) {
	assert(hobj != nullptr);
	
	if (hobj->image) {
		free(hobj->image->bitmap);
		free(hobj->image);
	}
	
	delete hobj;
	return 0;
}

int Obs_TranslateMessageGBS(GBS_MSG *msg, OBSevent *event_handler) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

/*
 * Image processing
 * */
int Obs_GetInputImageSize(HObj *hobj, short *w, short *h) {
	if (!hobj->image)
		return 0;
	
	*w = hobj->image->w;
	*h = hobj->image->h;
	
	return 0;
}

int Obs_SetOutputImageSize(HObj *hobj, short w, short h) {
	if (!w || !h)
		return 1;
	hobj->image_out_width = w;
	hobj->image_out_height = h;
	return 0;
}

int Obs_Output_GetPictstruct(HObj *hobj, IMGHDR **img) {
	*img = hobj->image;
	return hobj->image ? 0 : 1;
}

int Obs_Graphics_SetClipping(HObj *hobj, short x, short y, short w, short h) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetRotation(HObj *hobj, int angle) {
	if (angle != 90 && angle != 180 && angle != 270 && angle != 360)
		return 1;
	hobj->image_rotation = angle;
	return 0;
}

int Obs_GetInfo(HObj *hobj, int unk_0or1) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetScaling(HObj *hobj, int method) {
	// stub
	return 0;
}

int Obs_SetRenderOffset(HObj *hobj, short x, short y) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

/*
 * Sound processing
 * */
int Obs_Pause(HObj *hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Resume(HObj *hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Stop(HObj *hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Prepare(HObj *hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetPosition(HObj *hObj, int ms) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Sound_SetVolumeEx(HObj *hObj, char vol, char delta) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Sound_GetVolume(HObj *hObj, char *vol) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Mam_SetPurpose(HObj *hObj, char purpose) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Sound_SetPurpose(HObj *hObj, int purpose) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}
