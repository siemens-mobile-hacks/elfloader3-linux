#include "src/swi/proc/helper.h"
#include <spdlog/spdlog.h>
#include <swilib/gbs.h>

#define MSG_HELPER_RUN 0x0001

void Helper_Init() {
	CreateGBSproc(HELPER_CEPID, "HELPER", +[]() {
		GBS_MSG msg;
		if (!GBS_RecActDstMessage(&msg))
			return;
		if (msg.msg == MSG_HELPER_RUN) {
			if (msg.data0) {
				spdlog::debug("run subproc");
				((void (*)(int, void *))(msg.data0))(msg.submess, msg.data1);
			}
		} else {
			GBS_SendMessage(MMI_CEPID, MSG_HELPER_TRANSLATOR, msg.pid_from, (void *) msg.msg, (void *) msg.submess);
		}
	}, 0, 0);
}
