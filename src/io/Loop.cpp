#include "src/io/Loop.h"
#include "src/utils/time.h"

#include <cstring>
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <poll.h>
#include <format>
#include <csignal>
#include <cassert>

static volatile sig_atomic_t sigint_called = 0;

std::shared_ptr<Loop> Loop::instance() {
	static auto instance = std::make_shared<Loop>();
	return instance;
}

/**
 * EventLoop
 * */
Loop::Loop() {
	int fds[2];
	if (pipe2(fds, O_CLOEXEC | O_NONBLOCK) < 0)
		throw std::runtime_error(std::format("pipe2(): {}", strerror(errno)));

	m_waker_r = fds[0];
	m_waker_w = fds[1];

	watchFd(m_waker_r, WATCH_READ, [](IoWatcherEv ev, int fd) {
		static char buf[4];
		switch (ev) {
			case EV_CAN_READ:
				while (read(fd, buf, sizeof(buf)) < 0 && errno == EINTR);
			break;

			case EV_HUP:
			case EV_ERROR:
				throw std::runtime_error("Loop waker poll error.");
			break;

			case EV_CAN_WRITE:
				// none
			break;
		}
	});
}

void Loop::run() {
	sigint_called = 0;

	// SIGINT hook
	struct sigaction sa = {};
	sa.sa_sigaction = +[](int signal, siginfo_t *info, void *context) {
		sigint_called = 1;
	};
	if (sigaction(SIGINT, &sa, NULL) == -1)
		throw std::runtime_error(std::format("sigaction(): {}", strerror(errno)));

	while (!m_need_stop) {
		updateFds();

		if (sigint_called)
			break;

		int timeout = m_next_run - getCurrentTimestamp();
		int ret = ::poll(m_fds.data(), m_fds.size(), std::max(0, timeout));
		if (ret < 0 && errno != EINTR)
			throw std::runtime_error(std::format("poll(): {}", strerror(errno)));

		if (ret >= 0) {
			for (auto &pfd: m_fds) {
				auto w = m_curr_watchers[pfd.fd];

				if ((pfd.revents & POLLOUT) && (w->flags & WATCH_WRITE))
					w->callback(EV_CAN_WRITE, pfd.fd);

				if ((pfd.revents & POLLIN) && (w->flags & WATCH_READ))
					w->callback(EV_CAN_READ, pfd.fd);

				if ((pfd.revents & POLLHUP))
					w->callback(EV_HUP, pfd.fd);

				if ((pfd.revents & POLLERR))
					w->callback(EV_ERROR, pfd.fd);
			}
		}

		runTimeouts();
	}

	// Remove SIGINT hook
	sa.sa_sigaction = (void (*)(int, siginfo_t *, void *)) SIG_DFL;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		throw std::runtime_error(std::format("sigaction(): {}", strerror(errno)));
}

Loop::~Loop() {
	close(m_waker_w);
	close(m_waker_r);
}

void Loop::stop() {
	m_need_stop = true;
	wake();
}

void Loop::wake() {
	if (m_waker_w != -1) {
		while (write(m_waker_w, "w", 1) < 0 && errno == EINTR);
	}
}

/**
 * Timers
 * */
int Loop::addTimer(const TimerCallback &callback, int timeout_ms, bool loop) {
	m_mutex.lock();

	int id = m_global_timer_id++;

	auto new_timer = std::make_shared<Timer>();
	new_timer->id = id;
	new_timer->interval = timeout_ms;
	new_timer->callback = callback;
	new_timer->expires = getCurrentTimestamp() + timeout_ms;
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

void Loop::addTimerToQueue(std::shared_ptr<Timer> new_timer) {
	removeTimerFromQueue(new_timer);

	bool inserted = false;
	for (auto it = m_list.begin(); it != m_list.end(); it++) {
		if (it->get()->expires - new_timer->expires > 0) {
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

	if (next_timer->expires - getCurrentTimestamp() <= 0) {
		if (!(next_timer->flags & TIMER_CANCEL))
			next_timer->callback();

		if ((next_timer->flags & TIMER_LOOP) && !(next_timer->flags & TIMER_CANCEL)) {
			m_mutex.lock();
			next_timer->expires = getCurrentTimestamp() + next_timer->interval;
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
		m_next_run = next_timer->expires;
	} else {
		m_next_run = getCurrentTimestamp() + 60000;
	}
}

/**
 * IO watcher
 * */
void Loop::updateFds() {
	if (!m_watchers_changed)
		return;

	m_mutex.lock();
	m_fds.clear();
	m_curr_watchers.clear();

	for (auto it: m_watchers) {
		m_fds.resize(m_fds.size() + 1);
		auto *pfd = &m_fds.back();
		pfd->fd = it.second->fd;
		pfd->events = 0;
		pfd->revents = 0;

		if ((it.second->flags & WATCH_READ))
			pfd->events |= POLLIN;
		if ((it.second->flags & WATCH_WRITE))
			pfd->events |= POLLOUT;

		m_curr_watchers[it.second->fd] = it.second;
	}

	m_watchers_changed = false;
	m_mutex.unlock();
}

void Loop::setWatchFdEvents(int fd, uint32_t flags) {
	m_mutex.lock();
	auto w = m_watchers.at(fd);
	bool is_changed = w->flags != flags;
	if (is_changed)
		m_watchers_changed = true;
	w->flags = flags;
	m_mutex.unlock();

	if (is_changed)
		wake();
}

void Loop::watchFd(int fd, uint32_t flags, const IoWatcherCallback &callback) {
	m_mutex.lock();
	if (m_watchers.find(fd) != m_watchers.end()) {
		m_mutex.unlock();
		std::runtime_error(std::format("File descriptor {} already in watch mode!", fd));
	}
	m_watchers[fd] = std::make_shared<IoWatcherItem>(fd, flags, callback);
	m_watchers_changed = true;
	m_mutex.unlock();
	wake();
}

void Loop::unwatchFd(int fd) {
	m_mutex.lock();
	if (m_watchers.find(fd) == m_watchers.end()) {
		m_mutex.unlock();
		std::runtime_error(std::format("File descriptor {} is NOT in watch mode!", fd));
	}
	m_watchers.erase(fd);
	m_watchers_changed = true;
	m_mutex.unlock();

	if (fd != m_waker_r && fd != m_waker_w)
		wake();
}
