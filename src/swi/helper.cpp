#include "swi.h"

#include <cstdlib>

#define MSG_HELPER_RUN 0x0001

static PNGTOP_DESC png_top = {};

void Helper_Init() {
	GBS_CreateProc(HELPER_CEPID, "HELPER", +[]() {
		GBS_MSG msg;
		if (GBS_RecActDstMessage(&msg)) {
			if (msg.msg == MSG_HELPER_RUN) {
				if (msg.data0) {
					((void (*)(int, void *))(msg.data0))(msg.submess, msg.data1);
				}
			} else {
				GBS_SendMessage(MMI_CEPID, MSG_HELPER_TRANSLATOR, msg.pid_from, (void *) msg.msg, (void *) msg.submess);
			}
		}
	}, 0, 0);
}

void SUBPROC(void *func, int p2, void *p1) {
	GBS_SendMessage(HELPER_CEPID, MSG_HELPER_RUN, p2, func, p1);
}

PNGTOP_DESC *Helper_PngTop() {
	return &png_top;
}

void *Helper_LibTop() {
	return nullptr;
}
