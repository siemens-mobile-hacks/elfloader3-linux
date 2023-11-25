#pragma once

#include <cstdint>
#include <thread>
#include <functional>
#include <cassert>
#include <mutex>
#include <map>

#include <IpcProto.h>

class IPC {
	protected:
		IPC();
		~IPC();
		
		int m_screen_buffer_id;
		uint8_t *m_screen_buffer = nullptr;
		std::thread *m_thread = nullptr;
		std::string m_helper_path;
		std::string m_socket_path;
		int m_window_width = 132;
		int m_window_height = 176;
		std::vector<uint8_t> m_tx_buffer;
		std::vector<uint8_t> m_rx_buffer;
		std::mutex m_tx_mutex;
		std::map<IpcCmd, std::function<void(IpcPacket *pkt)>> m_handlers;
		
		int m_server_fd = -1;
		int m_client_fd = -1;
		
		uint8_t *createSharedMemory(int *mem_id);
		int openUnixSock(const char *name);
		int waitForClient(int sock, int timeout);
		void send(IpcPacket *pkt);
		void parseCommand();
		void handleCommand(IpcPacket *pkt);
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
		void stop();
};
