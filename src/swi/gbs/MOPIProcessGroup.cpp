#include "MOPIProcessGroup.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <pthread.h>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <spdlog/spdlog.h>

#include "src/swi/gbs.h"
#include "src/utils/cast.h"

static std::unordered_map<int, std::shared_ptr<MOPIProcessGroup>> m_process_groups;
static std::unordered_map<std::thread::id, std::shared_ptr<MOPIProcessGroup>> m_process_groups_by_tasks;

short MOPIProcessGroup::cepid() {
	auto current_tid = std::this_thread::get_id();
	if (m_process_groups_by_tasks.find(current_tid) != m_process_groups_by_tasks.end()) {
		auto pgroup = m_process_groups_by_tasks.at(current_tid);
		return pgroup->m_current_cepid;
	}
	return 0xFFFF;
}

std::shared_ptr<MOPIProcessGroup> MOPIProcessGroup::createProcessGroup(short cepid, const std::string &name, uint32_t priority) {
	auto pgroup = std::make_shared<MOPIProcessGroup>(cepid, name, priority);
	m_process_groups[cepid] = pgroup;
	m_process_groups_by_tasks[pgroup->m_thread.get_id()] = pgroup;
	return pgroup;
}

void MOPIProcessGroup::destroyProcessGroup(short gid) {
	auto pgroup = getProcessGroup(gid);
	if (pgroup) {
		m_process_groups.erase(gid);
		m_process_groups_by_tasks.erase(pgroup->m_thread.get_id());
	}
}

std::shared_ptr<MOPIProcessGroup> MOPIProcessGroup::getProcessGroup(short cepid) {
	if (m_process_groups.find(cepid & 0xFF00) != m_process_groups.end())
		return m_process_groups.at(cepid & 0xFF00);
	return {};
}

void MOPIProcessGroup::cleanup() {
	std::vector<int> ids;
	for (auto &it: m_process_groups) {
		ids.push_back(it.first);
	}

	for (auto id: ids) {
		destroyProcessGroup(id);
	}
}

MOPIProcessGroup::MOPIProcessGroup(short gid, const std::string &name, uint32_t priority) {
	m_gid = gid;
	m_name = name;
	m_priority = priority;

	m_thread = std::thread([this]() {
		run();
	});

	int min_priority = sched_get_priority_min(SCHED_FIFO);
	int max_priority = sched_get_priority_max(SCHED_FIFO);
	struct sched_param thread_params = {};
	thread_params.sched_priority = min_priority + ((max_priority - min_priority) * (0xFF - priority) / 0xFF);
	int ret = pthread_setschedparam(m_thread.native_handle(), SCHED_FIFO, &thread_params);
	if (ret == -1) {
		spdlog::error("pthread_setschedparam(): {}", strerror(errno));
	}

	spdlog::debug("[MOPI] [{:04X}] create group ({})", toUnsigned(m_gid), m_name);
}

MOPIProcessGroup::~MOPIProcessGroup() {
	destroy();
}

void MOPIProcessGroup::addMessageToQueue(short cepid, MOPI_MSG *msg) {
	auto proc = getProcessor(cepid);
	proc->mutex.lock();
	MOPI_MSG *cursor = proc->queue.head;
	MOPI_MSG *prev = nullptr;

	if (!proc->queue.head) {
		// Initialize the queue with the new message
		proc->queue.head = msg;
		proc->queue.tail = msg;
	} else {
		// Traverse the queue to find the correct insertion point based on priority
		while (cursor) {
			if (MOPI_GetMessagePriority(&cursor) > MOPI_GetMessagePriority(&msg))
				break;
			prev = cursor;
			cursor = cursor->next;
		}

		if (cursor) {
			// Insert before the cursor
			msg->next = cursor;

			if (proc->queue.head == cursor)
				proc->queue.head = msg;

			if (prev)
				prev->next = msg;
		} else {
			// Insert at the end of the queue if no higher-priority message found
			proc->queue.tail->next = msg;
			proc->queue.tail = msg;
			msg->next = nullptr;

		}
	}

	proc->queue.count++;
	proc->mutex.unlock();
}

void MOPIProcessGroup::removeMessageFromQueue(MOPI_MSG *msg) {
	auto proc = getProcessor(msg->cepid_to);
	proc->mutex.lock();
	MOPI_MSG *cursor = proc->queue.head;
	MOPI_MSG *prev = nullptr;

	while (cursor) {
		if (cursor == msg)
			break;
		prev = cursor;
		cursor = cursor->next;
	}

	if (!cursor) {
		proc->mutex.unlock();
		return;
	}

	if (msg == proc->queue.head)
		proc->queue.head = msg->next;

	if (msg == proc->queue.tail)
		proc->queue.tail = prev;

	if (prev)
		prev->next = msg->next;

	msg->next = nullptr;
	proc->queue.count--;
	proc->mutex.unlock();
}

MOPI_MSG *MOPIProcessGroup::peekMessageFromQueue(short cepid, bool all_messages) {
	auto proc = getProcessor(cepid);
	proc->mutex.lock();
	MOPI_MSG *cursor = proc->queue.head;
	while (cursor) {
		bool should_skip = (
			MOPI_GetMessageType(&cursor) == MOPI_MSG_TYPE_EXEC ||
			MOPI_GetMessageType(&cursor) == MOPI_MSG_TYPE_GTIMER_CALLBACK ||
			MOPI_GetMessageType(&cursor) == MOPI_MSG_TYPE_UNK ||
			(MOPI_GetMessageFlags(&cursor) & MOPI_MSG_FLAG_POSTPONED) != 0
		);
		if (all_messages || !should_skip)
			break;
		cursor = cursor->next;
	}
	proc->mutex.unlock();
	return cursor;
}

MOPI_MSG *MOPIProcessGroup::popMessageFromQueue(short cepid, bool all_messages) {
	auto proc = getProcessor(cepid);
	proc->mutex.lock();
	auto *top = peekMessageFromQueue(cepid, all_messages);
	if (top)
		removeMessageFromQueue(top);
	proc->mutex.unlock();
	return top;
}

void MOPIProcessGroup::sendMessage(MOPI_MSG **msg) {
	assert(msg != nullptr && *msg != nullptr);
	int cepid_to = (*msg)->cepid_to;

	auto pgroup = getProcessGroup(cepid_to);
	if (!pgroup) {
		spdlog::debug("[MOPI] CEPID {:04X} process group not found, message from {:04X} ignored.", toUnsigned(cepid_to), toUnsigned((*msg)->cepid_from));
		MOPI_FreeMessage(msg);
		return;
	}

	auto proc = pgroup->getProcessor(cepid_to);
	if (!proc || proc->stop) {
		spdlog::debug("[MOPI] CEPID {:04X} not found, message from {:04X} ignored.", toUnsigned(cepid_to), toUnsigned((*msg)->cepid_from));
		MOPI_FreeMessage(msg);
		return;
	}

	proc->mutex.lock();
	pgroup->addMessageToQueue(cepid_to, *msg);
	proc->mutex.unlock();
	pgroup->m_sem.release();
}

std::shared_ptr<MOPIProcessGroup::EventProcessor> MOPIProcessGroup::getProcessor(short cepid) {
	m_mutex.lock();
	if (m_processors.find(cepid) != m_processors.end()) {
		auto proc = m_processors[cepid];
		m_mutex.unlock();
		return proc;
	}
	m_mutex.unlock();
	return {};
}

void MOPIProcessGroup::createProcessor(short cepid, const std::string &name, MessageHandler handler, uint32_t priority) {
	if ((cepid & 0xFF00) != m_gid)
		throw std::runtime_error(std::format("CEPID {:04X} is not in group {:04X}", toUnsigned(cepid), toUnsigned(m_gid)));

	spdlog::debug("[MOPI] [{:04X}] create processor ({})", toUnsigned(cepid), name);

	m_mutex.lock();

	if (m_destroying) {
		m_mutex.unlock();
		throw std::runtime_error(std::format("CEPID group {:04X} is not exists!", toUnsigned(m_gid)));
	}

	if (m_processors.find(cepid) != m_processors.end()) {
		m_mutex.unlock();
		throw std::runtime_error(std::format("CEPID {:04X} already exists!", toUnsigned(cepid)));
	}

	auto proc = std::make_shared<EventProcessor>();
	proc->id = cepid;
	proc->name = name;
	proc->handler = handler;
	proc->priority = priority;
	m_processors[cepid] = proc;

	m_sorted_processors.clear();
	for (auto it: m_processors)
		m_sorted_processors.push_back(it.first);

	std::sort(m_sorted_processors.begin(), m_sorted_processors.end(), [this](int a, int b) {
		return m_processors.at(a)->priority < m_processors.at(b)->priority;
	});

	m_mutex.unlock();
}

void MOPIProcessGroup::destroyProcessor(short cepid) {
	m_mutex.lock();
	auto proc = getProcessor(cepid);
	if (!proc) {
		m_mutex.unlock();
		throw std::runtime_error(std::format("CEPID {:04X} not exists!", toUnsigned(cepid)));
	}
	spdlog::debug("[MOPI] [{:04X}] destroy processor ({})", toUnsigned(proc->id), proc->name);
	proc->stop = true;
	m_to_delete.push(cepid);
	m_mutex.unlock();
	m_sem.release();
}

void MOPIProcessGroup::runInContext(short cepid, std::function<void()> callback) {
	MOPI_MSG *msg;
	auto *payload = MOPI_CreateMessage<MOPI_EXEC_MSG>(&msg, cepid, 0);
	payload->callback = new std::function<void()>(callback);
	msg->flags = MOPI_MSG_TYPE_EXEC;
	msg->onDestroy = +[](MOPI_MSG **msg) {
		auto payload = MOPI_GetPayload<MOPI_EXEC_MSG>(msg);
		delete payload->callback;
	};
	MOPI_PostMessage(&msg);
}

MOPI_MSG *MOPIProcessGroup::peekMessage() {
	for (auto cepid: m_sorted_processors) {
		auto *msg = peekMessageFromQueue(cepid, true);
		if (msg)
			return msg;
	}
	return nullptr;
}

void MOPIProcessGroup::run() {
	while (!m_stop || m_processors.size() > 0) {
		m_sem.acquire();

		MOPI_MSG *msg;
		while ((msg = peekMessage())) {
			auto proc = getProcessor(msg->cepid_to);
			if (!proc) {
				spdlog::debug("[MOPI] CEPID {:04X} not found, message from {:40X} ignored.", toUnsigned(msg->cepid_to), toUnsigned(msg->cepid_from));
				removeMessageFromQueue(msg);
				MOPI_FreeMessage(&msg);
				continue;
			}

			if (proc->stop) {
				removeMessageFromQueue(msg);
				MOPI_FreeMessage(&msg);
				continue;
			}

			m_current_cepid = msg->cepid_to;

			if (MOPI_GetMessageType(&msg) == MOPI_MSG_TYPE_EXEC) { // only for simulator
				auto *payload = MOPI_GetPayload<MOPI_EXEC_MSG>(&msg);
				(*payload->callback)();
				removeMessageFromQueue(msg);
				MOPI_FreeMessage(&msg);
			} else if ((MOPI_GetMessageFlags(&msg) & MOPI_MSG_FLAG_GTIMER)) {
				switch (MOPI_GetMessageType(&msg)) {
					case MOPI_MSG_TYPE_GTIMER_CALLBACK:
						proc->handler();
					break;

					case MOPI_MSG_TYPE_GTIMER_MSG:
					{
						auto *gtimer_msg = MOPI_GetPayload<MOPI_GTIMER_MSG>(&msg);
						gtimer_msg->callback(gtimer_msg->tmr);
						removeMessageFromQueue(msg);
						MOPI_FreeMessage(&msg);
					}
					break;

					default:
						spdlog::error("[MOPI] CEPID {:04X} received invalid gtimer message (%d) from {:04X}.",
							toUnsigned(msg->cepid_to), MOPI_GetMessageType(&msg), toUnsigned(msg->cepid_from));
					break;
				}
			} else {
				proc->handler();
			}

			m_current_cepid = -1;
		}

		m_mutex.lock();
		while (m_to_delete.size() > 0) {
			int cepid = m_to_delete.front();
			MOPI_MSG *msg;
			while ((msg = peekMessageFromQueue(cepid, true))) {
				removeMessageFromQueue(msg);
				MOPI_FreeMessage(&msg);
			}
			m_processors.erase(cepid);
			m_to_delete.pop();
		}
		m_mutex.unlock();
	}
}

void MOPIProcessGroup::destroy() {
	spdlog::debug("[MOPI] [{:04X}] destroy group ({})", toUnsigned(m_gid), m_name);

	// Mark this process group as dead
	m_mutex.lock();
	m_destroying = true;
	m_mutex.unlock();

	// Destroy all processors
	for (auto &it: m_processors)
		destroyProcessor(it.first);

	m_stop = true;

	if (m_thread.joinable()) {
		m_sem.release();
		m_thread.join();
	}
}
