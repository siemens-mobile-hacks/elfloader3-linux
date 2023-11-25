#include "AppWindow.h"

#include <cstdio>

AppWindow::AppWindow(int w, int h, QWindow *parent) : QWindow(parent), m_backingStore(new QBackingStore(this)) {
	m_width = w;
	m_height = h;
	
	QRect screen_geometry = QGuiApplication::primaryScreen()->geometry();
	int x = (screen_geometry.width() - m_width) / 2;
	int y = (screen_geometry.height() - m_height) / 2;
	setGeometry(x, y, m_width, m_height);
	
	setMinimumSize(QSize(width(), height()));
	setMaximumSize(QSize(width(), height()));
	
	m_socket = new QLocalSocket(this);
	m_stream = new QDataStream(m_socket);
	
	connect(m_socket, &QIODevice::readyRead, [this]() {
		char buffer[4096];
		auto readed = m_stream->readRawData(buffer, sizeof(buffer));
		if (readed > 0) {
			m_rx_buffer.insert(m_rx_buffer.end(), buffer, buffer + readed);
			parseRxBuffer();
		}
	});
}

void AppWindow::parseRxBuffer() {
	while (m_rx_buffer.size() >= sizeof(IpcPacket)) {
		IpcPacket *pkt = reinterpret_cast<IpcPacket *>(&m_rx_buffer[0]);
		int pkt_size = sizeof(IpcPacket) + pkt->size;
		
		if (m_rx_buffer.size() < pkt_size)
			break;
		
		handleIpcCommand(pkt, &m_rx_buffer[sizeof(IpcPacket)]);
		m_rx_buffer.erase(m_rx_buffer.begin(), m_rx_buffer.begin() + pkt_size);
	}
}

void AppWindow::handleIpcCommand(IpcPacket *pkt, uint8_t *payload) {
	switch (pkt->cmd) {
		case IPC_CMD_REDRAW:
			renderNow();
		break;
		
		default:
			printf("CMD: %d\n", pkt->cmd);
		break;
	}
}

void AppWindow::connectToServer() {
	m_socket->connectToServer(m_socket_path);
	if (!m_socket->waitForConnected(5000))
		throw std::runtime_error("Can't connect to ELF simulator server!");
	printf("Connected to %s\n", m_socket_path.toLocal8Bit().constData());
}

bool AppWindow::event(QEvent *event) {
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
	
	QImage image(m_screen_buffer, m_width, m_height, QImage::Format_RGB16);
	painter.drawImage(rect, image);
	
	painter.end();
	
	m_backingStore->endPaint();
	m_backingStore->flush(rect);
}
