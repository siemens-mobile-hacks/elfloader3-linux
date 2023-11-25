#include "Loop.h"

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <csignal>
#include <cassert>

Loop *Loop::m_instance = nullptr;

void Loop::init() {
	int fds[2];
	
	if (pipe2(fds, O_CLOEXEC | O_NONBLOCK) < 0)
		throw std::runtime_error("pipe2() error");
	
	m_waker_r = fds[0];
	m_waker_w = fds[1];
	
	addFd(m_waker_r, WATCH_READ, [](IoWatcherEv ev, int fd) {
		static char buf[4];
		
		switch (ev) {
			case EV_CAN_READ:
				while (read(fd, buf, sizeof(buf)) > 0 || errno == EINTR);
			break;
			
			case EV_HUP:
			case EV_ERROR:
				throw std::runtime_error("Loop waker poll error.");
			break;
		}
	});
}

void Loop::run() {
	while (!m_need_stop) {
		int timeout = m_next_run - getCurrentTimestamp();
		
		updateFds();
		
		int ret = ::poll(fds, fds_count, std::max(0, timeout));
		if (ret < 0 && errno != EINTR) {
			LOGE("poll errno = %d\n", errno);
			throw std::runtime_error("poll error");
		}
		
		for (int i = 0; i < fds_count; i++) {
			auto pfd = &fds[i];
			auto w = &m_curr_watchers[pfd->fd];
			
			if ((pfd->revents & POLLOUT))
				w->callback(EV_CAN_WRITE, pfd->fd);
			
			if ((pfd->revents & POLLIN))
				w->callback(EV_CAN_READ, pfd->fd);
			
			if ((pfd->revents & POLLHUP))
				w->callback(EV_HUP, pfd->fd);
			
			if ((pfd->revents & POLLERR))
				w->callback(EV_ERROR, pfd->fd);
		}
		
		runTimeouts();
	}
}

void Loop::updateFds() {
	if (!m_watchers_changed)
		return;
	
	m_mutex.lock();
	
	if (fds) {
		delete[] fds;
		fds = nullptr;
	}
	
	assert(m_watchers.size() > 0);
	
	fds = new struct pollfd[m_watchers.size()];
	fds_count = 0;
	
	for (auto &w: m_watchers) {
		if (!w.second.callback)
			continue;
		
		fds[fds_count].fd = w.second.fd;
		fds[fds_count].events = 0;
		fds[fds_count].revents = 0;
		
		if ((w.second.flags & WATCH_READ))
			fds[fds_count].events |= POLLIN;
		
		if ((w.second.flags & WATCH_WRITE))
			fds[fds_count].events |= POLLOUT;
		
		fds_count++;
	}
	
	m_curr_watchers = m_watchers;
	m_watchers_changed = false;
	
	m_mutex.unlock();
}

void Loop::destroy() {
	m_mutex.lock();
	close(m_waker_w);
	close(m_waker_r);
	
	m_list.clear();
	m_timers.clear();
	m_watchers.clear();
	m_mutex.unlock();
}

void Loop::stop() {
	m_need_stop = true;
	wake();
}

void Loop::runTimeouts() {
	if (m_need_stop)
		return;
	
	m_mutex.lock();
	auto next_timer = (m_list.size() > 0 ? m_list.front() : nullptr);
	m_mutex.unlock();
	
	if (!next_timer) {
		m_next_run = getCurrentTimestamp() + 60000;
		return;
	}
	
	if (next_timer->time - getCurrentTimestamp() <= 0) {
		if (!(next_timer->flags & TIMER_CANCEL))
			next_timer->callback();
		
		if ((next_timer->flags & TIMER_LOOP) && !(next_timer->flags & TIMER_CANCEL)) {
			m_mutex.lock();
			next_timer->time = getCurrentTimestamp() + next_timer->interval;
			addTimerToQueue(next_timer);
			m_mutex.unlock();
		} else {
			m_mutex.lock();
			removeTimerFromQueue(next_timer);
			m_timers.erase(next_timer->id);
			m_mutex.unlock();
		}
		
		m_mutex.lock();
		next_timer = (m_list.size() > 0 ? m_list.front() : nullptr);
		m_mutex.unlock();
	}
	
	if (next_timer) {
		m_next_run = next_timer->time;
	} else {
		m_next_run = getCurrentTimestamp() + 60000;
	}
}

void Loop::addTimerToQueue(std::shared_ptr<Timer> new_timer) {
	removeTimerFromQueue(new_timer);
	
	bool inserted = false;
	for (auto it = m_list.begin(); it != m_list.end(); it++) {
		if (it->get()->time - new_timer->time > 0) {
			new_timer->it = m_list.insert(it, new_timer);
			inserted = true;
			break;
		}
	}
	
	if (!inserted) {
		m_list.push_back(new_timer);
		new_timer->it = --m_list.end();
	}
	
	new_timer->flags |= TIMER_INSTALLED;
}

void Loop::removeTimerFromQueue(std::shared_ptr<Timer> timer) {
	if ((timer->flags & TIMER_INSTALLED)) {
		m_list.erase(timer->it);
		timer->flags &= ~TIMER_INSTALLED;
	}
}

void Loop::wake() {
	if (m_waker_w != -1) {
		while (write(m_waker_w, "w", 1) < 0 && errno == EINTR);
	}
}

int Loop::addTimer(const TimerCallback &callback, int timeout_ms, bool loop) {
	m_mutex.lock();
	
	int id = m_global_timer_id++;
	
	auto new_timer = std::make_shared<Timer>();
	new_timer->id = id;
	new_timer->interval = timeout_ms;
	new_timer->callback = callback;
	new_timer->time = getCurrentTimestamp() + timeout_ms;
	new_timer->flags = loop ? TIMER_LOOP : 0;
	
	m_timers[id] = new_timer;
	addTimerToQueue(new_timer);
	
	m_mutex.unlock();
	
	wake();
	
	return id;
}

void Loop::removeTimer(int id) {
	m_mutex.lock();
	auto it = m_timers.find(id);
	if (it != m_timers.end())
		it->second->flags |= TIMER_CANCEL;
	m_mutex.unlock();
}

void Loop::setFdFlags(int fd, uint32_t flags) {
	m_mutex.lock();
	IoWatcher *w = &m_watchers[fd];
	if (w->flags != flags)
		m_watchers_changed = true;
	w->flags = flags;
	m_mutex.unlock();
	
	if (m_watchers_changed)
		wake();
}

void Loop::addFd(int fd, uint32_t flags, const WatcherCallback &callback) {
	m_mutex.lock();
	m_watchers[fd] = { fd, flags, callback };
	m_watchers_changed = true;
	m_mutex.unlock();
	wake();
}

void Loop::removeFd(int fd) {
	m_mutex.lock();
	m_watchers.erase(fd);
	m_watchers_changed = true;
	m_mutex.unlock();
	wake();
}
