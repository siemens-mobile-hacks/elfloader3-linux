#include "AppWindow.h"

#include <cstdio>

AppWindow::AppWindow(int w, int h, QWindow *parent) : QWindow(parent), m_backingStore(new QBackingStore(this)), m_socket(this) {
	m_width = w;
	m_height = h;

	QRect screen_geometry = QGuiApplication::primaryScreen()->geometry();
	int x = (screen_geometry.width() - m_width) / 2;
	int y = (screen_geometry.height() - m_height) / 2;
	setGeometry(x, y, m_width, m_height);

	setMinimumSize(QSize(width(), height()));
	setMaximumSize(QSize(width(), height()));

	connect(&m_socket, &QIODevice::readyRead, [this]() {
		char buffer[4096];
		auto readed = m_socket.read(buffer, sizeof(buffer));
		if (readed > 0) {
			m_rx_buffer.insert(m_rx_buffer.end(), buffer, buffer + readed);
			parseRxBuffer();
		}
	});

	connect(&m_socket, &QLocalSocket::errorOccurred, [this]() {
		QCoreApplication::quit();
	});
}

void AppWindow::parseRxBuffer() {
	while (m_rx_buffer.size() >= sizeof(IpcPacket)) {
		IpcPacket *pkt = reinterpret_cast<IpcPacket *>(&m_rx_buffer[0]);

		if (m_rx_buffer.size() < pkt->size)
			break;

		handleIpcCommand(pkt);
		m_rx_buffer.erase(m_rx_buffer.begin(), m_rx_buffer.begin() + pkt->size);
	}
}

void AppWindow::handleIpcCommand(IpcPacket *pkt) {
	switch (pkt->cmd) {
		case IPC_CMD_REDRAW:
			qDebug("renderNow");
			renderNow();
		break;

		default:
			qDebug("UNKNOWN CMD: %d\n", pkt->cmd);
		break;
	}
}

void AppWindow::sendIpcCommand(IpcPacket *pkt) {
	const char *data = reinterpret_cast<const char *>(pkt);
	m_socket.write(data, pkt->size);
	m_socket.flush();
}

void AppWindow::connectToServer() {
	m_socket.connectToServer(m_socket_path);
	if (!m_socket.waitForConnected(5000))
		throw std::runtime_error("Can't connect to ELF simulator server!");
}

bool AppWindow::event(QEvent *event) {
	if (event->type() == QEvent::KeyRelease || event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		IpcPacketKeyEvent pkt = {};
		pkt.header = { IPC_CMD_KEY_EVENT, sizeof(IpcPacketKeyEvent) };
		pkt.keycode = toIpcKeyCode(keyEvent);
		pkt.modifiers = toIpcKeyModifier(keyEvent);

		const char *text = keyEvent->text().toLocal8Bit().data();
		strncpy(pkt.text, text, sizeof(pkt.text));

		if (event->type() == QEvent::KeyPress) {
			// qDebug("key release %d%s", keyEvent->key(), keyEvent->isAutoRepeat() ? " [auto]" : "");
			pkt.state = keyEvent->isAutoRepeat() ? IPC_KEY_REPEAT : IPC_KEY_PRESS;
		} else {
			// qDebug("key press %d%s", keyEvent->key(), keyEvent->isAutoRepeat() ? " [auto]" : "");
			pkt.state = IPC_KEY_RELEASE;
		}

		sendIpcCommand(&pkt.header);

		return true;
	}

	if (event->type() == QEvent::UpdateRequest) {
		renderNow();
		return true;
	}
	return QWindow::event(event);
}

void AppWindow::renderLater() {
	requestUpdate();
}

void AppWindow::resizeEvent(QResizeEvent *resizeEvent) {
	m_backingStore->resize(resizeEvent->size());
}

void AppWindow::exposeEvent(QExposeEvent *) {
	if (isExposed())
		renderNow();
}

void AppWindow::renderNow() {
	if (!isExposed())
		return;

	QRect rect(0, 0, width(), height());
	m_backingStore->beginPaint(rect);

	QPaintDevice *device = m_backingStore->paintDevice();
	QPainter painter(device);

	QImage image(m_screen_buffer, m_width, m_height, QImage::Format_ARGB32);
	painter.drawImage(rect, image);

	painter.end();

	m_backingStore->endPaint();
	m_backingStore->flush(rect);
}
