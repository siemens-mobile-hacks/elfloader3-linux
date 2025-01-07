#include <cstring>
#include <swilib/csm.h>
#include <swilib/gui.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "src/swi/csm.h"
#include "src/io/Loop.h"
#include "src/swi/ll.h"
#include "src/swi/gui.h"

static int global_csm_id = 0;
static CSMROOT csm_root = {};

void CSM_Init() {
	CSMQ *csm_q = (CSMQ *) malloc(sizeof(CSMQ));
	memset(csm_q, 0, sizeof(CSMQ));
	
	csm_root.idle_id = 0;
	csm_root.under_idle_id = -1;
	csm_root.csm_q = csm_q;
	
	LL_Init(&csm_q->csm);
	LL_Init(&csm_q->cmd);
	LL_Init(&csm_q->csm_background);
}

static void CSM_Destroy(CSM_RAM *csm) {
	assert(csm->state == CSM_STATE_CLOSED);

	GUI_RAM *gui_cursor = (GUI_RAM *) csm->gui_ll.last;
	while (gui_cursor) {
		GUI_RAM *prev = gui_cursor->prev;
		GeneralFunc_flag1(gui_cursor->id, 1);
		gui_cursor = prev;
	}

	LL_Remove(&csm_root.csm_q->csm, csm);
	GUI_SyncStates();

	spdlog::debug("[CSM:{}] onClose", csm->id);

	csm->constr->onClose(csm);
	GBS_SendMessage(MMI_CEPID, MSG_CSM_DESTROYED, 0, (void *) csm->id, nullptr);

	free(csm);
}

void CSM_GarbageCollector() {
	CSM_RAM *cursor = (CSM_RAM *) CSM_root()->csm_q->csm.first;
	while (cursor) {
		CSM_RAM *next = reinterpret_cast<CSM_RAM *>(cursor->next);
		if (cursor->state == CSM_STATE_CLOSED)
			CSM_Destroy(cursor);
		cursor = next;
	}

	if (!CSM_root()->csm_q->csm.first) {
		spdlog::debug("No active CSM!");
		Loop::instance()->stop();
	}
}

CSM_RAM *CSM_Current() {
	return csm_root.csm_q->current_msg_processing_csm;
}

CSMROOT *CSM_root() {
	return &csm_root;
}

int CreateCSM(const CSM_DESC *desc, const void *initData, int unk) {
	int id = global_csm_id++;

	CSM_RAM *csm = (CSM_RAM *) malloc(desc->datasize);
	memcpy(csm, initData, desc->datasize);

	csm->id = id;
	csm->constr = (CSM_DESC *) desc;
	csm->state = 0;
	csm->unk1 = 0;
	csm->unk2 = 0;

	LL_Init(&csm->gui_ll);
	LL_Push(&csm_root.csm_q->csm, csm);

	csm_root.csm_q->id = csm->id;
	csm_root.csm_q->current_msg_processing_csm = csm;

	spdlog::debug("[CSM:{}] onCreate", csm->id);
	csm->constr->onCreate(csm);

	return id;
}

CSM_RAM *FindCSMbyID(int id) {
	return FindCSM(csm_root.csm_q, id);
}

CSM_RAM *FindCSM(CSMQ *csm_q, int id) {
	CSM_RAM *curr = reinterpret_cast<CSM_RAM *>(csm_q->csm.first);
	while (curr && curr->id != id)
		curr = reinterpret_cast<CSM_RAM *>(curr->next);
	return curr;
}

void CloseCSM(int id) {
	auto *csm = FindCSMbyID(id);
	if (!csm) {
		spdlog::error("{}: CSM {} not found!!", __func__, id);
		return;
	}
	csm->state = CSM_STATE_CLOSED;
	CSM_GarbageCollector();
}
