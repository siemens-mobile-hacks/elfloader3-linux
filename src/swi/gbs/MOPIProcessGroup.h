#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <functional>
#include <swilib/gbs.h>

#include "src/swi/gbs.h"

struct MOPI_EXEC_MSG {
	std::function<void()> *callback;
};

class MOPIProcessGroup {
public:
	typedef void (*MessageHandler)();

protected:
	struct MessagesQueue {
		MOPI_MSG *head = nullptr;
		MOPI_MSG *tail = nullptr;
		int count = 0;
	};

	struct EventProcessor {
		short id;
		std::string name;
		MessageHandler handler = nullptr;
		MessagesQueue queue;
		std::recursive_mutex mutex;
		uint32_t priority;
		bool stop = false;
	};

	std::string m_name;
	short m_gid = 0;
	uint32_t m_priority = 0;
	short m_current_cepid = -1;
	std::thread m_thread;
	std::recursive_mutex m_mutex;
	std::binary_semaphore m_sem{0};

	std::queue<short> m_to_delete;

	std::unordered_map<short, std::shared_ptr<EventProcessor>> m_processors;
	std::vector<short> m_sorted_processors;

	bool m_destroying = false;
	bool m_stop = false;

	std::shared_ptr<EventProcessor> getProcessor(short cepid);

	MOPI_MSG *peekMessage();
	void addMessageToQueue(short cepid, MOPI_MSG *msg);
	void removeMessageFromQueue(MOPI_MSG *msg);
	MOPI_MSG *peekMessageFromQueue(short cepid, bool all_messages = false);

public:
	MOPIProcessGroup(short gid, const std::string &name, uint32_t priority);
	~MOPIProcessGroup();

	static std::shared_ptr<MOPIProcessGroup> createProcessGroup(short gid, const std::string &name, uint32_t priority);
	static void destroyProcessGroup(short gid);
	static void cleanup();
	static std::shared_ptr<MOPIProcessGroup> getProcessGroup(short gid);
	MOPI_MSG *popMessageFromQueue(short cepid, bool all_messages = false);
	static void sendMessage(MOPI_MSG **msg);
	static short cepid();

	void createProcessor(short cepid, const std::string &name, MessageHandler handler, uint32_t priority);
	void destroyProcessor(short cepid);
	void runInContext(short cepid, std::function<void()> callback);

	void run();
	void destroy();
};
