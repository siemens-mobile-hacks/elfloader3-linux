#include "IPC.h"
#include "Loop.h"
#include "utils.h"

#include <poll.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>

IPC *IPC::instance() {
	static IPC ipc;
	return &ipc;
}

IPC::IPC() {
	
}

void IPC::start() {
	int ret;
	
	m_socket_path = tmpnam(nullptr);
	
	m_server_fd = openUnixSock(m_socket_path.c_str());
	if (m_server_fd < 0)
		throw std::runtime_error(strprintf("openUnixSock(%s) failed", m_socket_path.c_str()));
	
	m_screen_buffer = createSharedMemory(&m_screen_buffer_id);
	if (!m_screen_buffer)
		throw std::runtime_error("Can't allocate shared memory for screen buffer.");
	
	m_thread = new std::thread([this]() {
		std::string helper_exe = strprintf("%s %d '%s' %d %d", m_helper_path.c_str(), m_screen_buffer_id,
			m_socket_path.c_str(), m_window_width, m_window_height);
		system(helper_exe.c_str());
	});
	
	printf("Waiting for elfloader-helper...\n");
	
	m_client_fd = waitForClient(m_server_fd, 30000);
	if (m_client_fd < 0) {
		close(m_server_fd);
		throw std::runtime_error("elfloader3-helper process not found!");
	}
	
	Loop::instance()->addFd(m_client_fd, Loop::WATCH_READ, [this](Loop::IoWatcherEv ev, int fd) {
		switch (ev) {
			case Loop::EV_CAN_READ:
				
			break;
			
			case Loop::EV_CAN_WRITE:
			{
				m_tx_mutex.lock();
				int written = write(m_client_fd, &m_tx_buffer[0], m_tx_buffer.size());
				if (written > 0) {
					m_tx_buffer.erase(m_tx_buffer.begin(), m_tx_buffer.begin() + written);
					if (!m_tx_buffer.size())
						Loop::instance()->setFdFlags(m_client_fd, Loop::WATCH_READ);
				} else {
					throw std::runtime_error(strprintf("IPC write(): %s", strerror(errno)));
				}
				m_tx_mutex.unlock();
			}
			break;
			
			case Loop::EV_HUP:
			case Loop::EV_ERROR:
				throw std::runtime_error("IPC poll error.");
			break;
		}
	});
}

void IPC::stop() {
	if (m_socket_path.size() > 0) {
		unlink(m_socket_path.c_str());
		m_socket_path = "";
	}
	
	if (m_thread) {
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
}

uint8_t *IPC::createSharedMemory(int *mem_id) {
	*mem_id = shmget(IPC_PRIVATE, m_window_height * m_window_width * 4, IPC_CREAT | 0600);
	if (*mem_id < 0) {
		perror("shmget()");
		return nullptr;
	}
	
	auto mem = reinterpret_cast<uint8_t *>(shmat(*mem_id, NULL, 0));
	if (mem == (uint8_t *) -1) {
		perror("shmat()");
		return nullptr;
	}
	
	return mem;
}

void IPC::send(uint8_t *data, int size) {
	m_tx_mutex.lock();
	m_tx_buffer.insert(m_tx_buffer.end(), data, data + size);
	Loop::instance()->setFdFlags(m_client_fd, Loop::WATCH_READ | Loop::WATCH_WRITE);
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
			perror("poll");
			return -1;
		}
		
		if ((pfd[0].revents & (POLLERR | POLLHUP))) {
			perror("poll");
			return -1;
		}
		
		if ((pfd[0].revents & POLLIN)) {
			int new_client = accept4(sock, NULL, NULL, SOCK_NONBLOCK);
			if (new_client < 0) {
				perror("accept4");
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
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	
	struct sockaddr_un sock_un;
	memset(&sock_un, 0, sizeof(struct sockaddr_un));
	
	sock_un.sun_family = AF_UNIX;
	strcpy(sock_un.sun_path, name);
	
	unlink(name);
	
	int socket_length = strlen(sock_un.sun_path) + sizeof(sock_un.sun_family);
	if (bind(sock, (struct sockaddr *) &sock_un, socket_length) < 0) {
		perror("bind");
		close(sock);
		return -1;
	}
	
	if (listen(sock, 1) < 0) {
		perror("listen");
		close(sock);
		return -1;
	}
	
	return sock;
}

IPC::~IPC() {
	
}
