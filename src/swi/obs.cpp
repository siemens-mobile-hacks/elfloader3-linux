#include "swi.h"

#include <cstdio>

int CalcBitmapSize(short w, short h, char typy) {
	return w * h * typy;
}

HObj Obs_CreateObject(int uid_in, int uid_out, int prio, int msg_callback, int emb4, int sync, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented! %d %d\n", __func__, uid_in, uid_out);
	abort();
	return 0;
}

int Obs_DestroyObject(HObj hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetInput_File(HObj hObj, int unk_zero, WSHDR *path) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_GetInputImageSize(HObj hObj, short *w, short *h) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetOutputImageSize(HObj hObj, short w, short h) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Start(HObj hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Output_GetPictstruct(HObj hObj, IMGHDR **img) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Graphics_SetClipping(HObj hObj, short x, short y, short w, short h) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetRotation(HObj hObj, int angle) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_GetInfo(HObj hObj, int unk_0or1) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetScaling(HObj hObj, int unk5) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_TranslateMessageGBS(GBS_MSG *msg, OBSevent *event_handler) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Pause(HObj hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Resume(HObj hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Stop(HObj hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Prepare(HObj hObj) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetRenderOffset(HObj hObj,short x,short y) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_SetPosition(HObj hObj,int ms) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Mam_SetPurpose(HObj hObj,char purpose) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Sound_SetVolumeEx(HObj hObj, char vol, char delta) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Sound_GetVolume(HObj hObj, char *vol) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int Obs_Sound_SetPurpose(HObj hObj,int purpose) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}
