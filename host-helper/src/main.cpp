#include <cstdio>
#include <QtWidgets>

#include <sys/types.h>
#include <sys/shm.h>

#include "AppWindow.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	if (argc != 5) {
		printf("usage: %s <mem_id> <socket> <width> <height>\n", argv[0]);
		return -1;
	}
	
	int mem_id = atoi(argv[1]);
	const char *socket = argv[2];
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	
	auto mem = reinterpret_cast<uint8_t *>(shmat(mem_id, NULL, 0));
	if (mem ==  (void *) -1) {
		perror("shmat");
		return -1;
	}
    
	AppWindow win(width, height);
	win.setScreenBuffer(mem);
	win.setSocketPath(socket);
	win.connectToServer();
	win.show();
	
    return app.exec();
}
