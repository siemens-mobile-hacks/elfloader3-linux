#include "src/main.h"
#include "src/IPC.h"
#include "src/io/Loop.h"
#include "src/utils/string.h"
#include "src/utils/time.h"

#include <csignal>
#include <cstring>
#include <format>
#include <poll.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <spdlog/spdlog.h>

IPC *IPC::instance() {
	static IPC ipc;
	return &ipc;
}

void IPC::start() {
	m_socket_path = tempDir() + "/helper.socket";
	m_server_fd = openUnixSock(m_socket_path.c_str());
	m_screen_buffer = createSharedMemory(&m_screen_buffer_id);

	runDetachedProcess(m_helper_path, {
		std::to_string(m_screen_buffer_id),
		m_socket_path,
		std::to_string(m_window_width),
		std::to_string(m_window_height)
	});

	spdlog::debug("Waiting for elfloader-helper...");

	m_client_fd = waitForClient(m_server_fd, 10000);
	if (m_client_fd < 0)
		throw std::runtime_error("Timeout! Helper process is not running!");

	spdlog::debug("Connection established.");

	Loop::instance()->watchFd(m_client_fd, Loop::WATCH_READ, [this](Loop::IoWatcherEv ev, int fd) {
		uint8_t buffer[4096];

		switch (ev) {
			case Loop::EV_CAN_READ:
			{
				int readed = read(m_client_fd, buffer, sizeof(buffer));
				if (readed >= 0) {
					m_rx_buffer.insert(m_rx_buffer.end(), buffer, buffer + readed);
					parseCommand();
				} else if (errno != EINTR) {
					throw std::runtime_error(std::format("IPC read(): {}", strerror(errno)));
				}
			}
			break;

			case Loop::EV_CAN_WRITE:
			{
				m_tx_mutex.lock();
				int written = write(m_client_fd, &m_tx_buffer[0], m_tx_buffer.size());
				if (written >= 0) {
					m_tx_buffer.erase(m_tx_buffer.begin(), m_tx_buffer.begin() + written);
					if (!m_tx_buffer.size())
						Loop::instance()->setWatchFdEvents(m_client_fd, Loop::WATCH_READ);
				} else if (errno != EINTR) {
					m_tx_mutex.unlock();
					throw std::runtime_error(std::format("IPC write(): {}", strerror(errno)));
				}
				m_tx_mutex.unlock();
			}
			break;

			case Loop::EV_HUP:
			case Loop::EV_ERROR:
				Loop::instance()->unwatchFd(fd);
				spdlog::debug("Window closed...");
				Loop::instance()->stop();
				// kill(getpid(), SIGINT);
			break;
		}
	});
}

void IPC::parseCommand() {
	while (m_rx_buffer.size() >= sizeof(IpcPacket)) {
		IpcPacket *pkt = reinterpret_cast<IpcPacket *>(&m_rx_buffer[0]);

		if (m_rx_buffer.size() < pkt->size)
			break;

		handleCommand(pkt);
		m_rx_buffer.erase(m_rx_buffer.begin(), m_rx_buffer.begin() + pkt->size);
	}
}

void IPC::handleCommand(IpcPacket *pkt) {
	if (m_handlers.find(pkt->cmd) != m_handlers.end())
		m_handlers[pkt->cmd](pkt);
}

uint8_t *IPC::createSharedMemory(int *mem_id) {
	*mem_id = shmget(IPC_PRIVATE, m_window_height * m_window_width * 4, IPC_CREAT | 0600);
	if (*mem_id < 0)
		throw std::runtime_error(std::format("shmget(): {}", strerror(errno)));
	auto mem = reinterpret_cast<uint8_t *>(shmat(*mem_id, NULL, 0));
	if (mem == (uint8_t *) -1)
		throw std::runtime_error(std::format("shmat(): {}", strerror(errno)));
	return mem;
}

void IPC::send(IpcPacket *pkt) {
	m_tx_mutex.lock();
	uint8_t *data = reinterpret_cast<uint8_t *>(pkt);
	m_tx_buffer.insert(m_tx_buffer.end(), data, data + pkt->size);
	Loop::instance()->setWatchFdEvents(m_client_fd, Loop::WATCH_READ | Loop::WATCH_WRITE);
	m_tx_mutex.unlock();
}

int IPC::waitForClient(int sock, int timeout) {
	struct pollfd pfd[1] = {
		{.fd = sock, .events = POLLIN},
	};

	uint64_t now = getCurrentTimestamp();

	while (1) {
		int ret;
		do {
			ret = poll(pfd, 1, 1000);
		} while (ret < 0 && errno == EINTR);

		if (ret < 0) {
			spdlog::error("poll(): {}", strerror(errno));
			return -1;
		}

		if ((pfd[0].revents & (POLLERR | POLLHUP))) {
			spdlog::error("poll(): {}", strerror(errno));
			return -1;
		}

		if ((pfd[0].revents & POLLIN)) {
			int new_client = accept4(sock, NULL, NULL, SOCK_NONBLOCK);
			if (new_client < 0) {
				spdlog::error("accept4(): {}", strerror(errno));
				return -1;
			}
			return new_client;
		}

		int elapsed = getCurrentTimestamp() - now;
		if (elapsed >= timeout)
			break;
	}

	return -1;
}

int IPC::openUnixSock(const char *name) {
	int sock;
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error(std::format("socket(): {}", strerror(errno)));

	struct sockaddr_un sock_un;
	memset(&sock_un, 0, sizeof(struct sockaddr_un));

	sock_un.sun_family = AF_UNIX;
	strcpy(sock_un.sun_path, name);

	unlink(name);

	int socket_length = strlen(sock_un.sun_path) + sizeof(sock_un.sun_family);
	if (bind(sock, (struct sockaddr *) &sock_un, socket_length) < 0) {
		close(sock);
		throw std::runtime_error(std::format("bind(): {}", strerror(errno)));
	}

	if (listen(sock, 1) < 0) {
		close(sock);
		throw std::runtime_error(std::format("listen(): {}", strerror(errno)));
	}

	return sock;
}

void IPC::runDetachedProcess(const std::string &path, const std::vector<std::string> &argv) {
	spdlog::debug("[exec] {} {}", path, strJoin(" ", argv));
	pid_t pid = fork();
	if (pid < 0) {
		throw std::runtime_error(std::format("fork(): {}", strerror(errno)));
	} else if (pid == 0) {
		if (setsid() < 0)
			throw std::runtime_error(std::format("setsid(): {}", strerror(errno)));

		std::vector<char *> args;
		args.push_back(const_cast<char *>(path.c_str()));
		for (const auto &arg :argv)
			args.push_back(const_cast<char *>(arg.c_str()));
		args.push_back(nullptr);

		if (execvp(path.c_str(), args.data()) == -1)
			throw std::runtime_error(std::format("execvp(): {}", strerror(errno)));

		exit(0);
	}
}
