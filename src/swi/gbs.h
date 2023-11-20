#pragma once

#include "swi.h"
#include <thread>
#include <mutex>
#include <semaphore>
#include <queue>
#include <thread>

class GbsProcess {
	protected:
		GbsProcCallback m_handler = nullptr;
		std::string m_name;
		int m_id = 0;
		std::thread *m_thread = nullptr;
		std::binary_semaphore m_sem{0};
		std::mutex m_mutex;
		std::queue<GBS_MSG *> m_queue;
		bool m_stop = false;
	
	public:
		GbsProcess(int id, const std::string &name, GbsProcCallback handler);
		~GbsProcess();
		bool popMessage(GBS_MSG *msg);
		void sendMessage(GBS_MSG *msg);
		std::thread::id threadId();
		void run();
		void kill();
};
