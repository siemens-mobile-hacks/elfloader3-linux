#pragma once

#include <cstdint>
#include <functional>
#include <cassert>
#include <map>
#include <semaphore>
#include <string>
#include <IpcProto.h>

class IPC {
protected:
	int m_server_fd = -1;
	int m_client_fd = -1;
	std::mutex m_tx_mutex;
	std::vector<uint8_t> m_rx_buffer;
	std::vector<uint8_t> m_tx_buffer;
	std::binary_semaphore m_wait_for_client{0};
	std::function<void(bool)> m_start_callback;

	int m_screen_buffer_id = -1;
	uint8_t *m_screen_buffer = nullptr;
	std::string m_helper_path;
	std::string m_socket_path;
	int m_window_width = 132;
	int m_window_height = 176;
	std::map<IpcCmd, std::function<void(IpcPacket *pkt)>> m_handlers;

	uint8_t *createSharedMemory(int *mem_id);
	void send(IpcPacket *pkt);
	void parseCommand();
	void handleCommand(IpcPacket *pkt);
	void runDetachedProcess(const std::string &path, const std::vector<std::string> &argv);

	int openUnixSock(const char *name);
	int waitForClient(int dock, int timeout);
public:
	static IPC *instance();

	inline uint8_t *getScreenBuffer() {
		return m_screen_buffer;
	}

	inline void addHandler(IpcCmd cmd, std::function<void(IpcPacket *pkt)> callback) {
		m_handlers[cmd] = callback;
	}

	inline void sendRedraw() {
		IpcPacket packet = { IPC_CMD_REDRAW, sizeof(IpcPacket) };
		send(&packet);
	}

	inline void setHelperPath(const std::string &helper_path) {
		m_helper_path = helper_path;
	}

	inline void setWindowSize(int w, int h) {
		m_window_width = w;
		m_window_height = h;
	}

	void start();
};
