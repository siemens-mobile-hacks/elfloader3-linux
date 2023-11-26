#include "swi.h"
#include "utils.h"
#include "log.h"
#include "IPC.h"

static int _getKeyCodeFromEvent(IpcPacketKeyEvent *ev);

void MMI_Init() {
	GBS_CreateProc(MMI_CEPID, "MMI", +[]() {
		GBS_MSG msg;
		while (GBS_RecActDstMessage(&msg)) {
			printf("[MMI] %04X %04X\n", msg.msg, msg.submess);
			
			CSM_RAM *cursor = (CSM_RAM *) CSM_root()->csm_q->csm.last;
			while (cursor) {
				cursor->desc->onMessage(cursor, &msg);
				cursor = cursor->prev;
			}
			
			if (msg.msg == KEY_DOWN || msg.msg == KEY_UP || msg.msg == LONG_PRESS)
				GUI_HandleKeyPress(&msg);
			
			CSM_GarbageCollector();
			GUI_GarbageCollector();
		}
	}, 0, 0);
	
	IPC::instance()->addHandler(IPC_CMD_KEY_EVENT, [](IpcPacket *pkt) {
		IpcPacketKeyEvent *ev = reinterpret_cast<IpcPacketKeyEvent *>(pkt);
		
		int code = _getKeyCodeFromEvent(ev);
		if (code != -1) {
			if (ev->state == IPC_KEY_PRESS) {
				GBS_SendMessage(MMI_CEPID, KEY_DOWN, code, nullptr, nullptr);
			} else if (ev->state == IPC_KEY_RELEASE) {
				GBS_SendMessage(MMI_CEPID, KEY_UP, code, nullptr, nullptr);
			} else if (ev->state == IPC_KEY_REPEAT) {
				GBS_SendMessage(MMI_CEPID, LONG_PRESS, code, nullptr, nullptr);
			}
		}
	});
}

static int _getKeyCodeFromEvent(IpcPacketKeyEvent *ev) {
	switch (ev->keycode) {
		case IPC_KEY_INSERT:	return LEFT_SOFT;
		case IPC_KEY_F1:		return LEFT_SOFT;
		
		case IPC_KEY_PAGE_UP:	return RIGHT_SOFT;
		case IPC_KEY_F2:		return RIGHT_SOFT;
		
		case IPC_KEY_DELETE:	return GREEN_BUTTON;
		case IPC_KEY_F3:		return GREEN_BUTTON;
		
		case IPC_KEY_PAGE_DOWN:	return RED_BUTTON;
		case IPC_KEY_F4:		return RED_BUTTON;
		
		case IPC_KEY_HOME:		return ENTER_BUTTON;
		case IPC_KEY_ENTER:		return ENTER_BUTTON;
		case IPC_KEY_RETURN:	return ENTER_BUTTON;
		
		case IPC_KEY_PLUS:		return VOL_UP_BUTTON;
		case IPC_KEY_MINUS:		return VOL_DOWN_BUTTON;
		
		case IPC_KEY_UP:		return UP_BUTTON;
		case IPC_KEY_DOWN:		return DOWN_BUTTON;
		case IPC_KEY_LEFT:		return LEFT_BUTTON;
		case IPC_KEY_RIGHT:		return RIGHT_BUTTON;
	}
	return -1;
}
