#pragma once

#include <QtGui>
#include <QScopedPointer>
#include <QLocalSocket>

#include <vector>
#include <mutex>

#include <IpcProto.h>

class AppWindow: public QWindow {
	Q_OBJECT
	
	protected:
		int m_width;
		int m_height;
		QLocalSocket *m_socket = nullptr;
		QDataStream *m_stream = nullptr;
		uint8_t *m_screen_buffer = nullptr;
		QString m_socket_path;
		std::vector<uint8_t> m_rx_buffer;
	
	public:
		explicit AppWindow(int w, int h, QWindow *parent = nullptr);
		
		void parseRxBuffer();
		void handleIpcCommand(IpcPacket *pkt, uint8_t *payload);
		
		void setScreenBuffer(uint8_t *buffer) {
			m_screen_buffer = buffer;
		}
		
		void setSocketPath(QString socket) {
			m_socket_path = socket;
		}
		
		void connectToServer();
	public slots:
		void renderLater();
		void renderNow();
	
	protected:
		bool event(QEvent *event) override;
	
		void resizeEvent(QResizeEvent *event) override;
		void exposeEvent(QExposeEvent *event) override;
		
	private:
		QScopedPointer<QBackingStore> m_backingStore;
};
