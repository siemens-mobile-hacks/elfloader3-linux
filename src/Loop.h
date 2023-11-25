#pragma once

#include <mutex>
#include <map>
#include <any>
#include <list>
#include <memory>
#include <thread>
#include <stdexcept>
#include <functional>
#include <optional>

#include "log.h"
#include "utils.h"

class Loop {
	public:
		enum IoWatcherType {
			WATCH_READ		= 1 << 0,
			WATCH_WRITE		= 1 << 1,
		};
		
		enum IoWatcherEv {
			EV_CAN_READ		= 1 << 0,
			EV_CAN_WRITE	= 1 << 1,
			EV_ERROR		= 1 << 2,
			EV_HUP			= 1 << 3,
		};
		
	protected:
		enum TimerFlags {
			TIMER_LOOP		= 1 << 0,
			TIMER_INSTALLED	= 1 << 1,
			TIMER_CANCEL	= 1 << 2
		};
		
		typedef std::function<void()> TimerCallback;
		
		struct Timer {
			std::list<std::shared_ptr<Timer>>::iterator it;
			int id = 0;
			TimerCallback callback;
			uint8_t flags = 0;
			int interval = 0;
			int64_t time = 0;
		};
		
		typedef std::function<void(IoWatcherEv, int)> WatcherCallback;
		
		struct IoWatcher {
			int fd;
			uint32_t flags;
			WatcherCallback callback;
		};
		
		uint64_t m_next_run = 0;
		
		int m_waker_r = -1;
		int m_waker_w = -1;
		
		int fds_count = 0;
		struct pollfd *fds = nullptr;
		std::map<int, IoWatcher> m_watchers;
		std::map<int, IoWatcher> m_curr_watchers;
		bool m_watchers_changed = true;
		
		std::list<std::shared_ptr<Timer>> m_list;
		std::map<int, std::shared_ptr<Timer>> m_timers;
		std::mutex m_mutex;
		
		bool m_need_stop = false;
		int m_global_timer_id = 0;
	protected:
		static Loop *m_instance;
		
		void addTimerToQueue(std::shared_ptr<Timer> new_timer);
		void removeTimerFromQueue(std::shared_ptr<Timer> timer);
		
		void runTimeouts();
		
		void wake();
		void updateFds();
	public:
		void init();
		void run();
		void stop();
		void destroy();
		
		void addFd(int fd, uint32_t flags, const WatcherCallback &callback);
		void setFdFlags(int fd, uint32_t flags);
		void removeFd(int fd);
		
		int addTimer(const TimerCallback &callback, int timeout_ms, bool loop);
		void removeTimer(int id);
		
		static inline void setInstance(Loop *loop) {
			m_instance = loop;
		}
		
		static inline Loop * instance() {
			return m_instance;
		}
};
