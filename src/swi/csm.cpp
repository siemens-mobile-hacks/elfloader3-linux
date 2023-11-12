#include "swi.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>

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

int CreateCSM(const CSM_DESC *desc, const void *default_value, int param3) {
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
	
	csm->desc->onCreate(csm);
	
	return id;
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
	fprintf(stderr, "%s not implemented!\n", __func__);
}
