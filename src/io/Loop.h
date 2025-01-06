#pragma once

#include <mutex>
#include <map>
#include <list>
#include <memory>
#include <functional>
#include <sys/poll.h>

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
	typedef std::function<void(IoWatcherEv, int)> IoWatcherCallback;

	struct Timer {
		std::list<std::shared_ptr<Timer>>::iterator it;
		int id = 0;
		TimerCallback callback;
		uint8_t flags = 0;
		int interval = 0;
		int64_t expires = 0;
	};

	struct IoWatcherItem {
		int fd;
		uint32_t flags;
		IoWatcherCallback callback;
	};

	uint64_t m_next_run = 0;

	int m_waker_r = -1;
	int m_waker_w = -1;

	std::vector<pollfd> m_fds;
	std::map<int, std::shared_ptr<IoWatcherItem>> m_watchers;
	std::map<int, std::shared_ptr<IoWatcherItem>> m_curr_watchers;
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

	int addTimer(const TimerCallback &callback, int timeout_ms, bool loop);
public:
	Loop();
	~Loop();

	void run();
	void stop();

	void watchFd(int fd, uint32_t flags, const IoWatcherCallback &callback);
	void setWatchFdEvents(int fd, uint32_t flags);
	void unwatchFd(int fd);

	inline int setTimeout(const TimerCallback &callback, int timeout_ms = 0) {
		return addTimer(callback, timeout_ms, false);
	}

	inline int setInterval(const TimerCallback &callback, int timeout_ms) {
		return addTimer(callback, timeout_ms, true);
	}

	void removeTimer(int id);

	static std::shared_ptr<Loop> instance();
};
