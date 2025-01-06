#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <spdlog/spdlog.h>

#include <swilib/system.h>
#include <swilib/gbs.h>

#include "src/swi/gbs.h"
#include "src/io/Loop.h"
#include "src/swi/gbs/MOPIProcessGroup.h"
#include "src/utils/time.h"

#define GBS_TMR_CREATED 0x74696d72 // "timr"
#define GBS_TMR_DELETED 0x756e6466 // "undf"

typedef void (*GbsTimerCallback)(GBSTMR *tmr);

struct GbsTimerData {
	int cepid = 0;
	int msg = 0;
	GbsTimerCallback callback;
	GBSTMR *tmr = nullptr;
	int timer_id = -1;
};

void GBS_RunInContext(int cepid, std::function<void()> callback) {
	auto pgroup = MOPIProcessGroup::getProcessGroup(cepid);
	if (!pgroup)
		throw std::runtime_error(std::format("CEPID {:04X} process group not found!", cepid));
	pgroup->runInContext(cepid, callback);
}

static void _onGbsTimerFired(GbsTimerData *tmr_data) {
	auto callback = tmr_data->callback;
	int cepid = tmr_data->cepid;
	int msg = tmr_data->msg;
	GBSTMR *tmr = tmr_data->tmr;

	delete tmr_data;
	tmr->param0 = 0;

	MOPI_MSG *mopi_msg;
	if (callback) {
		// Run callback (GBS_StartTimerProc)
		auto *payload = MOPI_CreateGTimerMessage(&mopi_msg, cepid, msg, 0, MOPI_GTIMER_TYPE_CALLBACK);
		payload->callback = callback;
		payload->tmr = tmr;
		MOPI_PostMessage(&mopi_msg);
	} else {
		// Send GBS message (GBS_StartTimer)
		auto *payload = MOPI_CreateGTimerMessage(&mopi_msg, cepid, msg, 0, MOPI_GTIMER_TYPE_MSG);
		payload->callback = nullptr;
		payload->tmr = tmr;
		MOPI_PostMessage(&mopi_msg);
	}
}

void GBS_StartTimerProc(GBSTMR *tmr, long ticks, GBSTMR_CALLBACK callback) {
	assert(tmr != nullptr);
	assert(GBS_GetCurCepid() != -1);

	GBS_DelTimer(tmr);

	GbsTimerData *priv = new GbsTimerData;
	priv->callback = callback;
	priv->cepid = GBS_GetCurCepid();
	priv->tmr = tmr;
	priv->msg = 0;
	priv->timer_id = Loop::instance()->setTimeout([priv]() {
		_onGbsTimerFired(priv);
	}, ticksToMs(ticks));

	tmr->param0 = reinterpret_cast<int>(priv);
	tmr->param1 = GBS_TMR_CREATED;

	assert(xTimerStart(priv->handle, 0));
}

void GBS_StartTimer(GBSTMR *tmr, int ticks, int msg_id, int unk, int cepid) {
	assert(tmr != nullptr);

	GBS_DelTimer(tmr);

	GbsTimerData *priv = new GbsTimerData;
	priv->callback = nullptr;
	priv->cepid = cepid;
	priv->tmr = tmr;
	priv->msg = msg_id;
	priv->timer_id = Loop::instance()->setTimeout([priv]() {
		_onGbsTimerFired(priv);
	}, ticksToMs(ticks));

	tmr->param0 = reinterpret_cast<int>(priv);
	tmr->param1 = GBS_TMR_CREATED;

	assert(xTimerStart(priv->handle, 0));
}

void GBS_StopTimer(GBSTMR *timer) {
	if (IsTimerProc(timer)) {
		auto *priv = reinterpret_cast<GbsTimerData *>(timer->param0);
		if (priv->timer_id) {
			Loop::instance()->removeTimer(priv->timer_id);
			priv->timer_id = -1;
		}
	}
}

void GBS_DelTimer(GBSTMR *timer) {
	if (IsTimerProc(timer)) {
		auto *priv = reinterpret_cast<GbsTimerData *>(timer->param0);
		if (priv->timer_id != -1)
			Loop::instance()->removeTimer(priv->timer_id);
		delete priv;
	}
	timer->param0 = 0;
	timer->param1 = GBS_TMR_DELETED;
}

int IsTimerProc(GBSTMR *timer) {
	return timer->param1 == GBS_TMR_CREATED && timer->param0 != 0;
}

void GBS_SendMessage(int cepid, int msgid, ...) {
	va_list va;
	va_start(va, msgid);
	int submess = va_arg(va, int);
	void *data0 = va_arg(va, void *);
	void *data1 = va_arg(va, void *);
	va_end(va);

	MOPI_MSG *msg;
	int cepid_from = GBS_GetCurCepid();
	auto *payload = MOPI_CreateGbsMessage(&msg, cepid, cepid_from, msgid & 0xFFFF, msgid >> 0x1C);
	payload->submess = submess;
	payload->data0 = data0;
	payload->data1 = data1;
	MOPI_PostMessage(&msg);
}

int GBS_ReciveMessage(GBS_MSG *msg) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void GBS_AcceptMessage(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

void GBS_ClearMessage(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

void GBS_PendMessage(const GBS_MSG *msg) {
	spdlog::debug("{}: not implemented!", __func__);
}

void CreateGBSproc(int cepid, const char *name, GBS_PROC_CALLBACK callback, int prio, int unk) {
	auto pgroup = MOPIProcessGroup::getProcessGroup(cepid);
	if (!pgroup)
		throw std::runtime_error(std::format("CEPID {:04X} process group not found!", cepid));
	pgroup->createProcessor(cepid, name, callback, prio);
}

int GBS_GetCurCepid(void) {
	return MOPIProcessGroup::cepid();
}

int GBS_RecActDstMessage(GBS_MSG *msg) {
	int cepid = GBS_GetCurCepid();
	auto pgroup = MOPIProcessGroup::getProcessGroup(cepid);
	if (pgroup) {
		auto *mopi_msg = pgroup->popMessageFromQueue(cepid);
		if (mopi_msg) {
			MOPI_ConvertToGBS(msg, mopi_msg);
			return 1;
		}
	}
	return 0;
}

void KillGBSproc(int cepid) {
	auto pgroup = MOPIProcessGroup::getProcessGroup(cepid);
	if (!pgroup)
		throw std::runtime_error(std::format("CEPID {:04X} process group not found!", cepid));
	pgroup->destroyProcessor(cepid);
}

int GBS_WaitForMsg(const int *msg_list, int msgs_num, GBS_MSG *msgs, int timeout_ticks) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void GBS_DestroyMessage(GBS_MSG *msg) {
	spdlog::debug("{}: not implemented!", __func__);
}

void* GetGBSProcAddress(short cepid) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void CreateICL(PGROUP *pgroup) {
	MOPIProcessGroup::createProcessGroup(pgroup->id << 8, pgroup->name, pgroup->prio);
}

void *GBS_CreateProcessGroupResource(int mask, int unk_zero, PGROUP *pgroup, int size) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *GBS_GetCurrentTask() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *MemMBPAlloc(void *pool, int size) {
	return malloc(size);
}

void MemMBPFree(void *pool, void *ptr) {
	free(ptr);
}

void *RamMopiResourcePool() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *MOPI_CreateMessage(MOPI_MSG **msg, short cepid, int msgid, int size) {
	*msg = (MOPI_MSG *) malloc((size + sizeof(MOPI_MSG)));
	(*msg)->next = nullptr;
	(*msg)->onDestroy = nullptr;
	(*msg)->flags = 1;
	(*msg)->msgid = msgid;
	(*msg)->size = size;
	(*msg)->cepid_from = GBS_GetCurCepid();
	(*msg)->cepid_to = cepid;
	(*msg)->field6_0x12 = 0;
	return (size != 0) ? (*msg + 1) : nullptr;
}

MOPI_GBS_MSG *MOPI_CreateGbsMessage(MOPI_MSG **msg, int cepid, int cepid_from, short msgid, int flags) {
	auto *payload = (MOPI_GBS_MSG *) MOPI_CreateMessage(msg, cepid, msgid, sizeof(MOPI_GBS_MSG));
	(*msg)->cepid_from = cepid_from;
	(*msg)->flags = flags;
	return payload;
}

MOPI_GTIMER_MSG *MOPI_CreateGTimerMessage(MOPI_MSG **msg, short cepid, short msgid, int prio, int type) {
	auto *payload = (MOPI_GTIMER_MSG *) MOPI_CreateMessage(msg, cepid, msgid, sizeof(MOPI_GTIMER_MSG));
	(*msg)->cepid_from = -1;
	switch (type) {
		case MOPI_GTIMER_TYPE_MSG:
			(*msg)->flags = MOPI_MSG_TYPE_GTIMER_MSG | type | (prio << 8);
		break;

		case MOPI_GTIMER_TYPE_CALLBACK:
			(*msg)->flags = MOPI_MSG_TYPE_GTIMER_CALLBACK | type | ((prio | 0x10) << 8);
		break;

		default:
			throw std::runtime_error(std::format("Unknown gtimer type: {}", type));
		break;
	}
	return payload;
}

void MOPI_ConvertToGBS(GBS_MSG *gbs_msg, MOPI_MSG *mopi_msg) {
	gbs_msg->pid_from = mopi_msg->cepid_from;
	gbs_msg->submess = 0;
	gbs_msg->msg = mopi_msg->msgid;
	gbs_msg->data0 = nullptr;
	gbs_msg->data1 = nullptr;

	if (mopi_msg->size <= (int) sizeof(MOPI_GBS_MSG)) {
		if (MOPI_GetMessageType(&mopi_msg) == MOPI_MSG_TYPE_GTIMER_CALLBACK) {
			// ????
			memcpy(&gbs_msg->data1, mopi_msg + 1, 4);
		} else {
			// Short GBS message
			memcpy(&gbs_msg->submess, mopi_msg + 1, mopi_msg->size);
		}
	} else {
		// Long GBS message
		gbs_msg->msg = mopi_msg->msgid | 0x10000;
		gbs_msg->data1 = mopi_msg;
	}
}


void MOPI_FreeMessage(MOPI_MSG **msg) {
	if (msg == NULL || *msg == NULL)
		return;
	if (MOPI_GetMessageType(msg) != MOPI_MSG_TYPE_UNK) {
		if ((*msg)->onDestroy != NULL)
			(*msg)->onDestroy(msg);
		free(*msg);
		*msg = NULL;
	}
}

void *MOPI_GetPayload(MOPI_MSG **msg) {
	void *ptr = reinterpret_cast<uint8_t *>(*msg) + sizeof(MOPI_MSG);
	return (*msg)->size != 0 ? ptr : nullptr;
}

int MOPI_PostMessage(MOPI_MSG **msg) {
	MOPIProcessGroup::sendMessage(msg);
	return 0;
}
