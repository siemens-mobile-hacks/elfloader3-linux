#include "swi.h"
#include "swi/gbs.h"
#include "utils.h"
#include "log.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>

static std::map<int, GbsProcess *> gbs_processes;
static std::map<std::thread::id, int> thread2cepid;

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

void GBS_StartTimerProc(void *htimer, long ms, GbsTimerCallback callback) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_StartTimer(GBSTMR *tmr, int ms, int msg, int unk, int cepid) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_StopTimer(GBSTMR *tmr) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

int GBS_IsTimerRunning(GBSTMR *tmr) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
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

void GBS_PendMessage(GBS_MSG * param1) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void GBS_SendMessage(int cepid, int msg, int submess, void *data0, void *data1) {
	assert(gbs_processes.find(cepid) != gbs_processes.end());
	
	GBS_MSG *gbs_msg = (GBS_MSG *) malloc(sizeof(GBS_MSG));
	gbs_msg->pid_from = 0;
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

void GbsProcess::run() {
	while (!m_stop) {
		m_sem.acquire();
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
	while (m_queue.size()) {
		free(m_queue.front());
		m_queue.pop();
	}
	
	m_thread->join();
	delete m_thread;
	m_mutex.unlock();
}
