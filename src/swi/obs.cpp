#include <filesystem>
#include <spdlog/spdlog.h>
#include <swilib/image.h>
#include <swilib/obs.h>

#include <cstdio>
#include <cassert>
#include <stb_image_resize2.h>

#include "src/FFS.h"
#include "src/swi/explorer.h"
#include "src/swi/image.h"
#include "src/swi/wstring.h"

struct HObjPrivate {
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

HObj Obs_CreateObject(int uid_in, int uid_out, int prio, int msg_callback, int emb4, int sync, uint32_t *err) {
	HObjPrivate *hobj = new HObjPrivate();
	hobj->uid_in = uid_in;
	hobj->uid_out = uid_out;
	hobj->sync = sync;
	hobj->msg_id = msg_callback;
	if (err)
		*err = 0;
	return reinterpret_cast<HObj>(hobj);
}

int Obs_DestroyObject(HObj obs) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	assert(hobj != nullptr);

	if (hobj->image) {
		free(hobj->image->bitmap);
		free(hobj->image);
	}

	delete hobj;
	return 0;
}

int Obs_SetInput_File(HObj obs, int unk, WSHDR *path) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	hobj->input_file = ws2string(path);
	if (!std::filesystem::exists(FFS::dos2unix(hobj->input_file))) {
		spdlog::error("{}: {} not found\n", __func__, hobj->input_file);
		return 1;
	}
	return 0;
}

int Obs_Start(HObj obs) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	if (hobj->uid_out == UID_ZOOM_PIC) {
		hobj->image = IMG_LoadAny(FFS::dos2unix(hobj->input_file));
		if (!hobj->image)
			return 1;

		// Supported only RGB8888
		assert(hobj->image->bpnum == IMGHDR_TYPE_RGB8888);

		// Do resize
		if (hobj->image_out_width && hobj->image_out_height) {
			if (hobj->image_out_width != hobj->image->w || hobj->image_out_height != hobj->image->h) {
				uint8_t *new_bitmap = (uint8_t *) malloc(CalcBitmapSize(hobj->image_out_width, hobj->image_out_height, hobj->image->bpnum));

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
		spdlog::error("{} not implemented! (uid_out={:04X})\n", __func__, hobj->uid_out);
		abort();
	}
	return 0;
}

/**
 * Images
 * */
int Obs_GetInputImageSize(HObj obs, short *w, short *h) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	if (!hobj->image)
		return 1;
	*w = hobj->image->w;
	*h = hobj->image->h;
	return 0;
}

int Obs_SetOutputImageSize(HObj obs, short w, short h) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	if (!w || !h)
		return 1;
	hobj->image_out_width = w;
	hobj->image_out_height = h;
	return 0;
}

int Obs_Output_GetPictstruct(HObj obs, IMGHDR **img) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	*img = hobj->image;
	return hobj->image ? 0 : 1;
}

int Obs_Graphics_SetClipping(HObj obs, short x, short y, short w, short h) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetRotation(HObj obs, int angle) {
	auto *hobj = reinterpret_cast<HObjPrivate *>(obs);
	if (angle != 90 && angle != 180 && angle != 270 && angle != 360)
		return 1;
	hobj->image_rotation = angle;
	return 0;
}

int Obs_GetInfo(HObj obs, int unk) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetScaling(HObj obs, int method) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_TranslateMessageGBS(const GBS_MSG *msg, const OBSevent *handlers) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Pause(HObj obs) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Resume(HObj obs) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Stop(HObj obs) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Prepare(HObj obs) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetRenderOffset(HObj obs, short x, short y) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetPosition(HObj obs, int position) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Mam_SetPurpose(HObj obs, char purpose) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetVolumeEx(HObj obs, char volume, char delta) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_GetVolume(HObj obs, char *volume) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetPurpose(HObj obs, int purpose) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetAMRFormat(HObj obs, int format) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetAMRDTX(HObj obs, int dtx) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetNofChannels(HObj obs, int num_channels) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetAMRMode(HObj obs, int data_rate) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetFIsRecording(HObj obs, int is_recording) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetMaxFileSize(HObj obs, int max_file_size) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetRecordingMode(HObj obs, int recording_mode) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetOutput_File(HObj obs, const WSHDR *filename) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetOutput_Uid(HObj obs, int uid) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_Sound_SetBitrate(HObj obs, int bitrate) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetInput_Memory(HObj obs, int unk, char *buf, int len) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Obs_SetCSM(HObj obs, CSM_RAM *csm) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}
