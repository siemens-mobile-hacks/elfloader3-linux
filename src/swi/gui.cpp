#include "swi.h"
#include "utils.h"
#include "log.h"
#include "gui/Painter.h"
#include "gui/Theme.h"

#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <functional>

static std::map<int, GUI_RAM *> id2gui = {};
static std::map<int, size_t> id2gui_index = {};
static std::vector<GUI_RAM *> sorted_gui_list = {};
static Painter *painter = nullptr;

static int global_gui_id = 1;

void GUI_Init() {
	painter = new Painter(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void GUI_WalkRenderTree(const std::function<bool(GUI_RAM *)> &callback, bool reverse) {
	CSMROOT *root = CSM_root();
	if (reverse) {
		CSM_RAM *cursor = (CSM_RAM *) root->csm_q->csm.last;
		while (cursor) {
			GUI_RAM *gui_cursor = (GUI_RAM *) cursor->gui_ll.last;
			while (gui_cursor) {
				if (!callback(gui_cursor))
					return;
				gui_cursor = gui_cursor->prev;
			}
			cursor = cursor->prev;
		}
	} else {
		CSM_RAM *cursor = (CSM_RAM *) root->csm_q->csm.first;
		while (cursor) {
			GUI_RAM *gui_cursor = (GUI_RAM *) cursor->gui_ll.first;
			while (gui_cursor) {
				if (!callback(gui_cursor))
					return;
				gui_cursor = gui_cursor->next;
			}
			cursor = cursor->next;
		}
	}
}

void GUI_SyncStates() {
	id2gui.clear();
	id2gui_index.clear();
	sorted_gui_list.clear();
	
	CSM_RAM *cursor = (CSM_RAM *) CSM_root()->csm_q->csm.first;
	while (cursor) {
		GUI_RAM *gui_cursor = (GUI_RAM *) cursor->gui_ll.first;
		while (gui_cursor) {
			id2gui_index[gui_cursor->id] = sorted_gui_list.size();
			id2gui[gui_cursor->id] = gui_cursor;
			sorted_gui_list.push_back(gui_cursor);
			
			gui_cursor = gui_cursor->next;
		}
		cursor = cursor->next;
	}
	
	if (sorted_gui_list.size() > 0)
		LOGD("GUI on top: %d\n", sorted_gui_list.back()->id);
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

int GUI_Create_ID(GUI *gui, int id) {
	CSM_RAM *csm = CSM_Current();
	int prev_top_gui = GUI_GetTopID();
	
	assert(csm != nullptr);
	
	GUI_RAM *gui_ram = (GUI_RAM *) malloc(sizeof(GUI_RAM));
	memset(gui_ram, 0, sizeof(gui_ram));
	
	gui_ram->id = id;
	gui_ram->gui = gui;
	
	linked_list_push(&csm->gui_ll, gui_ram);
	
	GUI_SyncStates();
	
	if (prev_top_gui >= 0)
		GUI_DoUnFocus(prev_top_gui);
	
	gui->methods->onCreate(gui, malloc);
	
	GUI_DirectRedrawGUI();
	
	return id;
}

int GUI_Create(GUI *gui) {
	int id = global_gui_id++;
	return GUI_Create_ID(gui, id);
}

int GUI_Create_30or2(GUI *gui) { // WTF
	return GUI_Create(gui);
}

int GUI_CreateWithDummyCSM(GUI *gui, int flag) { // WTF
	return GUI_Create(gui);
}

int GUI_CreateWithDummyCSM_30or2(GUI *gui, int flag) { // WTF
	return GUI_Create(gui);
}

void GeneralFuncF1(int cmd) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GeneralFuncF0(int cmd) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GeneralFunc_flag1(int id, int cmd) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GeneralFunc_flag0(int id, int cmd) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GUI_DirectRedrawGUI() {
	GUI_DirectRedrawGUI_ID(GUI_GetTopID());
}

void GUI_DoFocus(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);
	
	assert(gui_ram->gui->state != CSM_GUI_STATE_CLOSED);
	
	if (gui_ram->gui->state != CSM_GUI_STATE_FOCUSED) {
		LOGD("[GUI:%d] onFocus\n", gui_ram->id);
		gui_ram->gui->methods->onFocus(gui_ram->gui, malloc, free);
	}
}

void GUI_DoUnFocus(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);
	
	assert(gui_ram->gui->state != CSM_GUI_STATE_CLOSED);
	
	if (gui_ram->gui->state == CSM_GUI_STATE_FOCUSED) {
		LOGD("[GUI:%d] onUnfocus\n", gui_ram->id);
		gui_ram->gui->methods->onUnfocus(gui_ram->gui, free);
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
	if (gui_index < sorted_gui_list.size() - 1)
		return sorted_gui_list[gui_index + 1];
	return nullptr;
}

void GUI_DirectRedrawGUI_ID(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);
	GUI_RAM *prev_gui_ram = GUI_GetPrev(id);
	
	GUI_DoFocus(id);
	
	if (gui_ram->gui->state != CSM_GUI_STATE_FOCUSED)
		return;
	
	if (prev_gui_ram)
		GUI_DirectRedrawGUI_ID(prev_gui_ram->id);
	
	LOGD("[GUI:%d] onRedraw\n", gui_ram->id);
	gui_ram->gui->methods->onRedraw(gui_ram->gui);
	
	painter->save();
	
	if (!GUI_IsOnTop(id))
		GUI_DoUnFocus(id);
}

void GUI_PendedRedrawGUI() {
	// TODO: subproc
	GUI_DirectRedrawGUI_ID(GUI_GetTopID());
}

void GUI_DrawString(WSHDR *wshdr, int x1, int y1, int x2, int y2, int font, int text_attribute, const char *Pen, const char *Brush) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void GUI_DrawPixel(int x1, int y1, const char *color) {
	painter->drawPixel(x1, y1, GUI_Color2Int(color));
}

void GUI_DrawLine(int x1, int y1, int x2, int y2, int type, const char *pen) {
	painter->drawLine(x1, y1, x2, y2, GUI_Color2Int(pen));
}

void GUI_DrawRectangle(int x1, int y1, int x2, int y2, int flags, const char *pen, const char *brush) {
	painter->drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, GUI_Color2Int(brush), GUI_Color2Int(pen));
}

void GUI_DrawRoundedFrame(int x1, int y1, int x2, int y2, int x_round, int y_round, int flags, const char *pen, const char *brush) {
	painter->drawRoundedRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, x_round, y_round, GUI_Color2Int(brush), GUI_Color2Int(pen));
}

void GUI_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

void GUI_DrawArc(int x1, int y1, int x2, int y2, int start, int end, int flags, char *pen, char *brush) {
	painter->drawArc(x1, y1, x2 - x1 + 1, y2 - y1 + 1, start, end, GUI_Color2Int(brush), GUI_Color2Int(pen));
}

uint32_t GUI_Color2Int(const char *color) {
	const uint8_t *u8 = reinterpret_cast<const uint8_t *>(color);
	uint32_t a = static_cast<uint32_t>(u8[3]) * 0xFF / 0x64;
	return (a << 24) | (u8[0] << 16) | (u8[1] << 8) | u8[2]; // RGBA
}

char *GUI_GetPaletteAdrByColorIndex(int index) {
	return Theme::instance()->getColorPtr(index);
}

void GUI_GetRGBcolor(int index, char *dest) {
	GUI_GetRGBbyPaletteAdr(GUI_GetPaletteAdrByColorIndex(index), dest);
}

void GUI_GetRGBbyPaletteAdr(char *addr, char *dest) {
	assert(dest != nullptr && addr != nullptr);
	memcpy(dest, addr, 4);
}

void GUI_SetColor(int a, int r, int g, int b, char *dest) {
	assert(dest != nullptr);
	dest[0] = r;
	dest[1] = g;
	dest[2] = b;
	dest[3] = a;
}

int GUI_ScreenW() {
	return SCREEN_WIDTH;
}

int GUI_ScreenH() {
	return SCREEN_HEIGHT;
}

int GUI_HeaderH() {
	return 32; // ELKA
}

int GUI_SoftkeyH() {
	return 32; // ELKA
}

void GUI_SetIDLETMR(int time_ms, int msg) {
	// stub
}

void GUI_RestartIDLETMR(void) {
	// stub
}

void GUI_DisableIDLETMR(void) {
	// stub
}

void GUI_DisableIconBar(int disable) {
	// stub
}

void GUI_AddIconToIconBar(int pic, short *num) {
	// stub
}
