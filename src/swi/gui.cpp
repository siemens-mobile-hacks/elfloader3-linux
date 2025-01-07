#include "src/gui/Painter.h"
#include "src/swi/gbs.h"
#include "src/swi/ll.h"
#include "src/swi/gui.h"
#include "src/swi/csm.h"
#include "swilib/gbs.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <queue>
#include <spdlog/common.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <swilib/gui.h>
#include <swilib/system.h>

static std::queue<int> gui_to_redraw;
static std::queue<GUI_RAM *> gui_to_destroy;
static std::unordered_map<int, GUI_RAM *> id2gui = {};
static std::unordered_map<int, CSM_RAM *> id2csm = {};
static std::unordered_map<int, size_t> id2gui_index = {};
static std::vector<GUI_RAM *> sorted_gui_list = {};
std::atomic<bool> redraw_requested = false;
static Painter *painter = nullptr;

static int global_gui_id = 1;

void GUI_Init() {
	LCDLAYER_Init();
}

void GUI_SyncStates() {
	LockSched();
	id2csm.clear();
	id2gui.clear();
	id2gui_index.clear();
	sorted_gui_list.clear();

	CSM_RAM *cursor = (CSM_RAM *) CSM_root()->csm_q->csm.first;
	while (cursor) {
		GUI_RAM *gui_cursor = (GUI_RAM *) cursor->gui_ll.first;
		while (gui_cursor) {
			id2gui_index[gui_cursor->id] = sorted_gui_list.size();
			id2gui[gui_cursor->id] = gui_cursor;
			id2csm[gui_cursor->id] = cursor;
			sorted_gui_list.push_back(gui_cursor);

			gui_cursor = gui_cursor->next;
		}
		cursor = (CSM_RAM *) cursor->next;
	}

	if (sorted_gui_list.size() > 0)
		spdlog::debug("GUI on top: {}", sorted_gui_list.back()->id);
	UnlockSched();
}

int GUI_GetTopID() {
	if (sorted_gui_list.size() > 0)
		return sorted_gui_list.back()->id;
	return -1;
}

GUI *GUI_GetFocusedTop() {
	if (sorted_gui_list.size() > 0) {
		if (sorted_gui_list.back()->gui->state == CSM_GUI_STATE_FOCUSED)
			return sorted_gui_list.back()->gui;
	}
	return nullptr;
}

bool GUI_IsOnTop(int id) {
	if (!sorted_gui_list.size())
		return false;
	return sorted_gui_list.back()->id == id;
}

void GUI_DoFocus(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);

	assert(gui_ram->gui->state != CSM_GUI_STATE_CLOSED);

	if (gui_ram->gui->state != CSM_GUI_STATE_FOCUSED) {
		spdlog::debug("[GUI:{}] onFocus", gui_ram->id);
		auto *gui_methods = (GUI_METHODS *) gui_ram->gui->methods;
		gui_methods->onFocus(gui_ram->gui, malloc, free);
	}
}

void GUI_DoUnFocus(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);

	assert(gui_ram->gui->state != CSM_GUI_STATE_CLOSED);

	if (gui_ram->gui->state == CSM_GUI_STATE_FOCUSED) {
		spdlog::debug("[GUI:{}] onUnfocus", gui_ram->id);
		auto *gui_methods = (GUI_METHODS *) gui_ram->gui->methods;
		gui_methods->onUnfocus(gui_ram->gui, free);
	}
}

GUI_RAM *GUI_GetById(int id) {
	return id2gui[id];
}

GUI_RAM *GUI_GetPrev(int id) {
	int gui_index = id2gui_index[id];
	if (gui_index > 0)
		return sorted_gui_list[gui_index - 1];
	return nullptr;
}

GUI_RAM *GUI_GetNext(int id) {
	int gui_index = id2gui_index[id];
	if (gui_index < (int) sorted_gui_list.size() - 1)
		return sorted_gui_list[gui_index + 1];
	return nullptr;
}

void GUI_Close(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);

	assert(gui_ram->gui->state != CSM_GUI_STATE_CLOSED);

	if (gui_ram->gui->state == CSM_GUI_STATE_FOCUSED)
		GUI_DoUnFocus(gui_ram->id);

	spdlog::debug("[GUI:{}] onClose", gui_ram->id);
	auto *gui_methods = (GUI_METHODS *) gui_ram->gui->methods;
	gui_methods->onClose(gui_ram->gui, free);

	if (gui_ram->gui->state == CSM_GUI_STATE_CLOSED) {
		LL_Remove(&id2csm[id]->gui_ll, gui_ram);
		GUI_SyncStates();

		gui_to_destroy.push(gui_ram);

		int next_top = GUI_GetTopID();
		if (next_top >= 0) {
			GUI_DoFocus(next_top);
			DirectRedrawGUI_ID(next_top);
		}
	}
}

void GUI_HandleKeyPress(GBS_MSG *msg) {
	int top_id = GUI_GetTopID();
	if (top_id < 0 && msg->submess == 60) {
		spdlog::debug("msg {}", msg->submess);
		CloseCSM(0);
		return;
	}

	GUI_RAM *gui_ram = GUI_GetById(top_id);

	GUI_MSG gui_msg = {};
	gui_msg.gbsmsg = msg;

	if (gui_ram->gui->state == CSM_GUI_STATE_FOCUSED) {
		spdlog::debug("[GUI:{}] onKey", gui_ram->id);
		auto *gui_methods = (GUI_METHODS *) gui_ram->gui->methods;
		int ret = gui_methods->onKey(gui_ram->gui, &gui_msg);
		if (ret) {
			spdlog::debug("[GUI:{}] onKey ret={} (gui close)", gui_ram->id, ret);
			int id = gui_ram->id;
			GBS_RunInContext(MMI_CEPID, [id]() {
				GeneralFunc_flag1(id, 1);
			});
		} else {
			spdlog::debug("[GUI:{}] onKey ret={}", gui_ram->id, ret);
		}
	}
}

void GUI_GarbageCollector() {
	while (gui_to_destroy.size() > 0) {
		GUI_RAM *gui_ram = gui_to_destroy.front();

		assert(gui_ram->gui->state == CSM_GUI_STATE_CLOSED);

		spdlog::debug("[GUI:{}] destroy", gui_ram->id);
		auto *gui_methods = (GUI_METHODS *) gui_ram->gui->methods;
		gui_methods->onDestroy(gui_ram->gui, free);
		free(gui_ram);

		gui_to_destroy.pop();
	}
}

uint32_t GUI_Color2Int(const char *color) {
	color = color ?: GetPaletteAdrByColorIndex(23); // fallback
	const uint8_t *u8 = reinterpret_cast<const uint8_t *>(color);
	uint32_t a = static_cast<uint32_t>(u8[3]) * 0xFF / 0x64;
	return (a << 24) | (u8[0] << 16) | (u8[1] << 8) | u8[2]; // ABGR8888
}

Painter *GUI_GetPainter() {
	return painter;
}

/*
 * Firmware methods
 */

char *RamPressedKey() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *RamScreenBuffer() {
	return RamLcdMainLayersList()[0].layer->buffer;
}

void *BuildCanvas(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *Ram_LCD_Overlay_Layer() {
	return reinterpret_cast<uint8_t *>(RamLcdMainLayersList()[0].layer) + 0xBC;
}

void AddKeybMsgHook(KeybMsgHookProc callback) {
	spdlog::debug("{}: not implemented!", __func__);
}

int AddKeybMsgHook_end(KeybMsgHookProc callback) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void RemoveKeybMsgHook(KeybMsgHookProc callback) {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsGuiOnTop(int id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreateGUI_ID(void *gui, int id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreateGUI(void *gui_ptr) {
	auto *gui = (GUI *) gui_ptr;

	LockSched();
	CSM_RAM *csm = CSM_Current();
	int prev_top_gui = GUI_GetTopID();

	assert(csm != nullptr);

	GUI_RAM *gui_ram = (GUI_RAM *) malloc(sizeof(GUI_RAM));
	memset(gui_ram, 0, sizeof(GUI_RAM));

	gui_ram->id = global_gui_id++;
	gui_ram->gui = gui;

	LL_Push(&csm->gui_ll, gui_ram);

	GUI_SyncStates();
	UnlockSched();

	if (prev_top_gui >= 0)
		GUI_DoUnFocus(prev_top_gui);

	auto *gui_methods = (GUI_METHODS *) gui->methods;
	gui_methods->onCreate(gui, malloc);
	DirectRedrawGUI();

	return gui_ram->id;
}

int CreateGUI_30or2(void *gui) {
	return CreateGUI(gui);
}

int CreateGUIWithDummyCSM(void *gui, int flag) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreateGUIWithDummyCSM_30or2(void *gui, int flag) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void GeneralFuncF1(int cmd) {
	int id = GUI_GetTopID();
	if (id != -1)
		GeneralFunc_flag1(id, cmd);
}

void GeneralFuncF0(int cmd) {
	int id = GUI_GetTopID();
	if (id != -1)
		GeneralFunc_flag0(id, cmd);
}

void GeneralFunc_flag1(int id, int cmd) {
	GUI_Close(id);
	GBS_SendMessage(MMI_CEPID, MSG_GUI_DESTROYED, 0, (void *) id, (void *) cmd);
}

void GeneralFunc_flag0(int id, int cmd) {
	GUI_Close(id);
	GBS_SendMessage(MMI_CEPID, MSG_GUI_DESTROYED, 0, (void *) id, (void *) cmd);
}

void DirectRedrawGUI(void) {
	redraw_requested = false;
	DirectRedrawGUI_ID(GUI_GetTopID());
}

void DirectRedrawGUI_ID(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);
	GUI_RAM *prev_gui_ram = GUI_GetPrev(id);

	GUI_DoFocus(id);

	if (gui_ram->gui->state != CSM_GUI_STATE_FOCUSED)
		return;

	if (prev_gui_ram)
		DirectRedrawGUI_ID(prev_gui_ram->id);

	if (spdlog::get_level() <= spdlog::level::debug)
		spdlog::debug("[GUI:{}] onRedraw", gui_ram->id);

	auto *layer = LCD_GetCurrentLayer();
	layer->rect = *gui_ram->gui->canvas;

	auto *gui_methods = (GUI_METHODS *) gui_ram->gui->methods;
	gui_methods->onRedraw(gui_ram->gui);

	if (!GUI_IsOnTop(id))
		GUI_DoUnFocus(id);
}

void PendedRedrawGUI(void) {
	if (!redraw_requested) {
		redraw_requested = true;
		GBS_SendMessage(MMI_CEPID, MMI_CMD_REDRAW);
	}
}

GUI *GetTopGUI(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void REDRAW(void) {
	PendedRedrawGUI();
}

void *FindGUIbyId(int id, CSM_RAM **csm) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetCurGuiID() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void FocusGUI(int id) {
	spdlog::debug("{}: not implemented!", __func__);
}

void UnfocusGUI(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GUI_NewTimer(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GUI_DeleteTimer(void *gui, int id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void GUI_StartTimerProc(void *gui, int id, long timeout_ms, GUI_TimerProc callback) {
	spdlog::debug("{}: not implemented!", __func__);
}

#if 0
// Currently we have only one layer
static LCDLAYER mmi_layer = {};
static LCDLAYER *mmi_layers[10] = {
	&mmi_layer, nullptr
};








void GUI_REDRAW() {
	// WTF?
	GUI_DirectRedrawGUI();
}

LCDLAYER **GUI_GetLCDLayerList() {
	return mmi_layers;
}

void *GUI_RamScreenBuffer() {
	return IPC::instance()->getScreenBuffer();
}

void GUI_IpcRedrawScreen() {
	if (!screen_redraw_requested) {
		screen_redraw_requested = true;
		GBS_RunInContext(MMI_CEPID, []() {
			screen_redraw_requested = false;
			IPC::instance()->sendRedraw();
		});
	}
}
#endif
