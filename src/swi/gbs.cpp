#include "swi.h"
#include "swi/gbs.h"
#include "utils.h"
#include "log.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>
#include <uv.h>

struct GbsTimerData {
	int cepid;
	GbsTimerCallback callback;
	GBSTMR *tmr;
};

static std::map<int, GbsProcess *> gbs_processes;
static std::map<std::thread::id, int> thread2cepid;

static void _gbsTimerCallback(uv_timer_t *ut) {
	auto *tmr_data = reinterpret_cast<GbsTimerData *>(ut->data);
	GBSTMR *tmr = tmr_data->tmr;
	GBS_StopTimer(tmr); // free timer
	
	printf("_gbsTimerCallback\n");
}

void GBS_Init() {
	GBS_CreateProc(MMI_CEPID, "MMI", +[]() {
		GBS_MSG msg;
		LOGD("MMI PROC HANDLER, lol\n");
		if (GBS_RecActDstMessage(&msg)) {
			LOGD("got msg\n");
		}
	}, 0, 0);
	GBS_SendMessage(MMI_CEPID, 123, 321, nullptr, nullptr);
}

void GBS_StartTimerProc(GBSTMR *tmr, long ticks, GbsTimerCallback callback) {
	assert(GBS_GetCurCepid() != -1);
	
	// Create timer payload
	GbsTimerData *tmr_data = new GbsTimerData;
	tmr_data->callback = callback;
	tmr_data->cepid = GBS_GetCurCepid();
	tmr_data->tmr = tmr;
	
	// Create timer
	uv_timer_t *ut = new uv_timer_t;
	uv_timer_init(uv_default_loop(), ut);
	ut->data = reinterpret_cast<void *>(tmr_data);
	tmr->param0 = reinterpret_cast<int>(ut);
	
	// Start timer
	uv_timer_start(ut, _gbsTimerCallback, (ticks * 1000) / 216, 0);
}

void GBS_StartTimer(GBSTMR *tmr, int ms, int msg, int unk, int cepid) {
//	GbsTimerInternal *timer_internal = new GbsTimerInternal();
//	timer_internal->timer = new uv_timer_t;
//	timer_internal->callback = callback;
//	timer_internal->cepid = cepid;
	
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_StopTimer(GBSTMR *tmr) {
	auto *ut = reinterpret_cast<uv_timer_t *>(tmr->param0);
	assert(ut != nullptr);
	
	// Delete timer peyload
	auto *tmr_data = reinterpret_cast<GbsTimerData *>(ut->data);
	delete tmr_data;
	
	// Detele timer
	uv_timer_stop(ut);
	delete ut;
	
	tmr->param0 = 0;
}

int GBS_IsTimerRunning(GBSTMR *tmr) {
	auto *ut = reinterpret_cast<uv_timer_t *>(tmr->param0);
	return ut != nullptr;
}

void GBS_DelTimer(GBSTMR *tmr) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_CreateProc(int cepid, const char *name, GbsProcCallback msg_handler, int prio, int unk_zero) {
	assert(gbs_processes.find(cepid) == gbs_processes.end());
	gbs_processes[cepid] = new GbsProcess(cepid, name, msg_handler);
	thread2cepid[gbs_processes[cepid]->threadId()] = cepid;
}

int GBS_GetCurCepid(void) {
	auto tid = std::this_thread::get_id();
	if (thread2cepid.find(tid) != thread2cepid.end())
		return thread2cepid[tid];
	return -1;
}

void GBS_KillProc(int cepid) {
	assert(gbs_processes.find(cepid) != gbs_processes.end());
	gbs_processes[cepid]->kill();
	thread2cepid.erase(gbs_processes[cepid]->threadId());
	gbs_processes.erase(cepid);
}

void GBS_RunInContext(int cepid, std::function<void()> callback) {
	assert(gbs_processes.find(cepid) != gbs_processes.end());
	gbs_processes[cepid]->runInContext(callback);
}

void GBS_PendMessage(GBS_MSG * param1) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_SendMessage(int cepid, int msg, int submess, void *data0, void *data1) {
	assert(gbs_processes.find(cepid) != gbs_processes.end());
	
	GBS_MSG *gbs_msg = (GBS_MSG *) malloc(sizeof(GBS_MSG));
	#if NEWSGOLD
	gbs_msg->unk = 0xA000;
	#endif
	gbs_msg->pid_from = GBS_GetCurCepid();
	gbs_msg->msg = msg;
	gbs_msg->submess = submess;
	gbs_msg->data0 = data0;
	gbs_msg->data1 = data1;
	gbs_processes[cepid]->sendMessage(gbs_msg);
}

int GBS_ReciveMessage(GBS_MSG * param1) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

void GBS_AcceptMessage(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_ClearMessage(void) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

int GBS_RecActDstMessage(GBS_MSG *msg) {
	int cepid = GBS_GetCurCepid();
	assert(cepid != -1);
	assert(gbs_processes.find(cepid) != gbs_processes.end());
	return gbs_processes[cepid]->popMessage(msg) ? 1 : 0;
}

int GBS_WaitForMsg(const int *msg_list, int msgs_num, GBS_MSG *param3, int timeout) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

GbsProcess::GbsProcess(int id, const std::string &name, GbsProcCallback handler) {
	m_id = id;
	m_name = name;
	m_handler = handler;
	m_thread = new std::thread(&GbsProcess::run, this);
}

bool GbsProcess::popMessage(GBS_MSG *msg) {
	m_mutex.lock();
	GBS_MSG *queue_msg = m_queue.front();
	if (queue_msg) {
		*msg = *queue_msg;
		free(queue_msg);
		m_queue.pop();
	}
	m_mutex.unlock();
	return queue_msg != nullptr;
}

void GbsProcess::sendMessage(GBS_MSG *msg) {
	m_mutex.lock();
	m_queue.push(msg);
	m_sem.release();
	m_mutex.unlock();
}

void GbsProcess::runInContext(std::function<void()> callback) {
	m_mutex.lock();
	m_run_queue.push(callback);
	m_sem.release();
	m_mutex.unlock();
}

void GbsProcess::run() {
	while (!m_stop) {
		m_sem.acquire();
		
		while (m_run_queue.size() > 0) {
			std::function<void()> func;
			m_mutex.lock();
			if (m_run_queue.size() > 0) {
				func = m_run_queue.front();
				m_run_queue.pop();
			}
			m_mutex.unlock();
			
			if (func)
				func();
		}
		
		m_handler();
	}
	m_mutex.lock();
	delete this;
}

std::thread::id GbsProcess::threadId() {
	return m_thread->get_id();
}

void GbsProcess::kill() {
	m_mutex.lock();
	m_stop = true;
	m_sem.release();
	m_mutex.unlock();
}

GbsProcess::~GbsProcess() {
	while (m_queue.size() > 0) {
		free(m_queue.front());
		m_queue.pop();
	}
	
	while (m_run_queue.size() > 0)
		m_run_queue.pop();
	
	m_thread->join();
	delete m_thread;
	m_mutex.unlock();
}
