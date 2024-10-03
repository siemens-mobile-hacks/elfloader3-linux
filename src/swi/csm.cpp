#include "swi.h"
#include "utils.h"
#include "log.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

static int csm_global_id = 0;
static CSMROOT csm_root = {};

static void addCsmToList(CSM_RAM *csm);

void CSM_Init() {
	CSMQ *csm_q = (CSMQ *) malloc(sizeof(CSMQ));
	memset(csm_q, 0, sizeof(CSMQ));
	
	csm_root.idle_id = 0;
	csm_root.under_idle_id = -1;
	csm_root.csm_q = csm_q;
	
	linked_list_init(&csm_q->csm);
	linked_list_init(&csm_q->cmd);
	linked_list_init(&csm_q->csm_background);
}

int CSM_Create(const CSM_DESC *desc, const void *default_value, int param3) {
	int id = csm_global_id++;
	
	CSM_RAM *csm = (CSM_RAM *) malloc(desc->datasize);
	memcpy(csm, default_value, desc->datasize);
	
	csm->id = id;
	csm->desc = desc;
	csm->state = 0;
	csm->unk1 = 0;
	csm->unk2 = 0;
	
	linked_list_init(&csm->gui_ll);
	linked_list_push(&csm_root.csm_q->csm, csm);
	
	csm_root.csm_q->id = csm->id;
	csm_root.csm_q->current_msg_processing_csm = csm;
	
	LOGD("[CSM:%d] onCreate\n", csm->id);
	csm->desc->onCreate(csm);
	
	return id;
}

void CSM_Close(CSM_RAM *csm) {
	assert(csm->state == CSM_STATE_CLOSED);
	
	GUI_RAM *gui_cursor = (GUI_RAM *) csm->gui_ll.last;
	while (gui_cursor) {
		GUI_RAM *prev = gui_cursor->prev;
		GUI_GeneralFunc_flag1(gui_cursor->id, 1);
		gui_cursor = prev;
	}
	
	linked_list_remove(&csm_root.csm_q->csm, csm);
	GUI_SyncStates();
	
	LOGD("[CSM:%d] onClose\n", csm->id);
	csm->desc->onClose(csm);
	
	GBS_SendMessage(MMI_CEPID, MSG_CSM_DESTROYED, 0, (void *) csm->id, nullptr);
	
	free(csm);
}

void CSM_GarbageCollector() {
	CSM_RAM *cursor = (CSM_RAM *) CSM_root()->csm_q->csm.first;
	while (cursor) {
		CSM_RAM *next = cursor->next;
		if (cursor->state == CSM_STATE_CLOSED)
			CSM_Close(cursor);
		cursor = next;
	}
}

CSMROOT *CSM_root(void) {
	return &csm_root;
}

CSM_RAM *FindCSMbyID(int id) {
	return FindCSM(csm_root.csm_q, id);
}

void DoIDLE(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
}

CSM_RAM *FindCSM(CSMQ *q, int id) {
	CSM_RAM *curr = (CSM_RAM *) q->csm.first;
	while (curr && curr->id != id)
		curr = curr->next;
	return curr;
}

CSM_RAM *CSM_Current() {
	return csm_root.csm_q->current_msg_processing_csm;
}

void CloseCSM(int id) {
	if (FindCSMbyID(id)) {
		fprintf(stderr, "%s !!!!!\n", __func__);
		FindCSMbyID(id)->state = CSM_STATE_CLOSED;
		CSM_GarbageCollector();
	} else {
		fprintf(stderr, "%s not implemented!\n", __func__);
	}
}
