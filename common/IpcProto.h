#pragma once

#include <cstdint>

enum IpcCmd {
	IPC_CMD_REDRAW,
};

#pragma pack(push, 1)
struct IpcPacket {
	int cmd;
	int size;
};
#pragma pack(pop)
