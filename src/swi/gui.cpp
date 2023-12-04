#include "swi.h"
#include "utils.h"
#include "log.h"
#include "IPC.h"
#include "Resources.h"
#include "gui/Painter.h"

#include <map>
#include <queue>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <functional>

static std::queue<GUI_RAM *> gui_to_destroy;
static std::map<int, GUI_RAM *> id2gui = {};
static std::map<int, CSM_RAM *> id2csm = {};
static std::map<int, size_t> id2gui_index = {};
static std::vector<GUI_RAM *> sorted_gui_list = {};
static Painter *painter = nullptr;
static bool screen_redraw_requested = false;

static int global_gui_id = 1;

void GUI_Init() {
	uint8_t *buffer = IPC::instance()->getScreenBuffer();
	painter = new Painter(buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void GUI_SyncStates() {
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

void GUI_HandleKeyPress(GBS_MSG *msg) {
	int top_id = GUI_GetTopID();
	if (top_id < 0)
		return;
	
	GUI_RAM *gui_ram = GUI_GetById(top_id);
	
	GUI_MSG gui_msg = {};
	gui_msg.gbsmsg = msg;
	
	if (gui_ram->gui->state == CSM_GUI_STATE_FOCUSED) {
		LOGD("[GUI:%d] onKey\n", gui_ram->id);
		int ret = gui_ram->gui->methods->onKey(gui_ram->gui, &gui_msg);
		if (ret) {
			LOGD("[GUI:%d] onKey ret=%d (gui close)\n", gui_ram->id, ret);
			int id = gui_ram->id;
			GBS_RunInContext(MMI_CEPID, [id]() {
				GUI_GeneralFunc_flag1(id, 1);
			});
		}
	}
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

void GUI_Close(int id) {
	GUI_RAM *gui_ram = GUI_GetById(id);
	
	assert(gui_ram->gui->state != CSM_GUI_STATE_CLOSED);
	
	if (gui_ram->gui->state == CSM_GUI_STATE_FOCUSED)
		GUI_DoUnFocus(gui_ram->id);
	
	LOGD("[GUI:%d] onClose\n", gui_ram->id);
	gui_ram->gui->methods->onClose(gui_ram->gui, free);
	
	if (gui_ram->gui->state == CSM_GUI_STATE_CLOSED) {
		linked_list_remove(&id2csm[id]->gui_ll, gui_ram);
		GUI_SyncStates();
		
		gui_to_destroy.push(gui_ram);
		
		int next_top = GUI_GetTopID();
		if (next_top >= 0) {
			GUI_DoFocus(next_top);
			GUI_DirectRedrawGUI_ID(next_top);
		}
	}
}

void GUI_GarbageCollector() {
	while (gui_to_destroy.size() > 0) {
		GUI_RAM *gui_ram = gui_to_destroy.front();
		
		assert(gui_ram->gui->state == CSM_GUI_STATE_CLOSED);
		
		LOGD("[GUI:%d] destroy\n", gui_ram->id);
		gui_ram->gui->methods->onDestroy(gui_ram->gui, free);
		free(gui_ram);
		
		gui_to_destroy.pop();
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

void GUI_DirectRedrawGUI() {
	GUI_DirectRedrawGUI_ID(GUI_GetTopID());
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
	
	RECT *rect = gui_ram->gui->canvas;
	painter->setWindow(rect->x, rect->y, rect->x2, rect->y2);
	
	gui_ram->gui->methods->onRedraw(gui_ram->gui);
	
	if (!GUI_IsOnTop(id))
		GUI_DoUnFocus(id);
}

void GUI_PendedRedrawGUI() {
	GBS_RunInContext(MMI_CEPID, []() {
		GUI_DirectRedrawGUI_ID(GUI_GetTopID());
	});
}

void GUI_GeneralFuncF0(int code) {
	int id = GUI_GetTopID();
	if (id != -1)
		GUI_GeneralFunc_flag0(id, code);
}

void GUI_GeneralFuncF1(int code) {
	int id = GUI_GetTopID();
	if (id != -1)
		GUI_GeneralFunc_flag1(id, code);
}

void GUI_GeneralFunc_flag0(int id, int code) {
	fprintf(stderr, "%s: unimplemented!\n", __func__);
	GUI_GeneralFunc_flag1(id, code);
}

void GUI_GeneralFunc_flag1(int id, int code) {
	fprintf(stderr, "%s: destroy GUI\n", __func__);
	
	GUI_Close(id);
	GBS_SendMessage(MMI_CEPID, MSG_GUI_DESTROYED, 0, (void *) id, (void *) code);
}

void GUI_REDRAW() {
	// WTF?
	GUI_DirectRedrawGUI();
}

void GUI_DrawString(WSHDR *wshdr, int x1, int y1, int x2, int y2, int font, int text_attribute, const char *Pen, const char *Brush) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	GUI_IpcRedrawScreen();
}

void GUI_DrawPixel(int x1, int y1, const char *color) {
	painter->drawPixel(x1, y1, GUI_Color2Int(color));
	GUI_IpcRedrawScreen();
}

void GUI_DrawLine(int x1, int y1, int x2, int y2, int type, const char *pen) {
	painter->drawLine(x1, y1, x2, y2, GUI_Color2Int(pen));
	GUI_IpcRedrawScreen();
}

void GUI_DrawRectangle(int x1, int y1, int x2, int y2, int flags, const char *pen, const char *brush) {
	painter->drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, GUI_Color2Int(brush), GUI_Color2Int(pen));
	GUI_IpcRedrawScreen();
}

void GUI_DrawRoundedFrame(int x1, int y1, int x2, int y2, int x_round, int y_round, int flags, const char *pen, const char *brush) {
	painter->drawRoundedRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, x_round, y_round, GUI_Color2Int(brush), GUI_Color2Int(pen));
	GUI_IpcRedrawScreen();
}

void GUI_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush) {
	painter->drawTriangle(x1, y1, x2, y2, x3, y3, GUI_Color2Int(brush), GUI_Color2Int(pen));
	GUI_IpcRedrawScreen();
}

void GUI_DrawArc(int x1, int y1, int x2, int y2, int start, int end, int flags, char *pen, char *brush) {
	painter->drawArc(x1, y1, x2 - x1 + 1, y2 - y1 + 1, start, end, GUI_Color2Int(brush), GUI_Color2Int(pen));
	GUI_IpcRedrawScreen();
}

void GUI_DrawObject(DRWOBJ *drw) {
	auto [x1, y1, x2, y2] = painter->getWindow();
	
	if (drw->type == DRWOBJ_TYPE_IMG) {
		assert(drw->img != nullptr);
		painter->setWindow(drw->rect.x, drw->rect.y, drw->rect.x2, drw->rect.y2);
		painter->drawBitmap(0, 0, drw->img->w, drw->img->h, drw->img->bitmap, IMG_GetBitmapType(drw->img->bpnum), drw->offset_x, drw->offset_y);
	}
	
	painter->setWindow(x1, y1, x2, y2);
	
	GUI_IpcRedrawScreen();
}

void GUI_SetPropTo_Obj1(DRWOBJ *drw, RECT *rect, int rect_flag, WSHDR *wshdr, int font, int flags) {
	assert(drw != nullptr && rect != nullptr && wshdr != nullptr);
	
	drw->type = DRWOBJ_TYPE_TEXT;
	drw->rect_flags = rect_flag;
	drw->flags = flags;
	drw->font = font;
	memcpy(&drw->rect, rect, sizeof(RECT));
	
	// Clone string
	drw->ws = AllocWS(wshdr->body->len + 1);
	memcpy(drw->ws->wsbody, wshdr->wsbody, wshdr->body->len + 1);
}

void GUI_ObjSetColor(DRWOBJ *drw, const char *color1, const char *color2) {
	assert(drw != nullptr);
	
	if (color1) {
		memcpy(drw->color1, color1, 4);
	} else {
		memset(drw->color1, 0, 4);
	}
	
	if (color2) {
		memcpy(drw->color2, color2, 4);
	} else {
		memset(drw->color2, 0, 4);
	}
}

void GUI_SetProp2ImageOrCanvas(DRWOBJ *drw, RECT *rect, int flags, IMGHDR *img, int offset_x, int offset_y) {
	assert(drw != nullptr);
	assert(rect != nullptr);
	assert(img != nullptr);
	
	drw->type = DRWOBJ_TYPE_IMG;
	drw->img = img;
	drw->rect_flags = 0;
	drw->flags = flags;
	drw->offset_x = offset_x;
	drw->offset_y = offset_y;
	memcpy(&drw->rect, rect, sizeof(RECT));
}

void GUI_SetPropTo_Obj5(DRWOBJ *drw, RECT *rect, int flags, IMGHDR *img) {
	GUI_SetProp2ImageOrCanvas(drw, rect, flags, img, 0, 0);
}

void GUI_FreeDrawObject_subobj(DRWOBJ *drw) {
	assert(drw != nullptr);
	if (drw->type == DRWOBJ_TYPE_TEXT) {
		if (drw->ws) {
			FreeWS(drw->ws);
			drw->ws = nullptr;
		}
	}
}

uint32_t GUI_Color2Int(const char *color) {
	const uint8_t *u8 = reinterpret_cast<const uint8_t *>(color);
	uint32_t a = static_cast<uint32_t>(u8[3]) * 0xFF / 0x64;
	return (a << 24) | (u8[0] << 16) | (u8[1] << 8) | u8[2]; // RGBA
}

char *GUI_GetPaletteAdrByColorIndex(int index) {
	return Resources::instance()->getColorPtr(index);
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

void GUI_StoreXYWHtoRECT(RECT *rect, int x, int y, int w, int h) {
	rect->x = x;
	rect->y = y;
	rect->x2 = x + w - 1;
	rect->y2 = y + h - 1;
}

void GUI_StoreXYXYtoRECT(RECT *rect, int x, int y, int x2, int y2) {
	rect->x = x;
	rect->y = y;
	rect->x2 = x2;
	rect->y2 = y2;
}

int GUI_GetFontYSIZE(int font) {
	return 16;
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
