#pragma once
#include <cstdint>
#include <format>
#include <functional>
#include <stdexcept>
#include <swilib/gbs.h>

#define MOPI_MSG_FLAGS_SHIFT	(0)
#define MOPI_MSG_FLAGS_MASK		(0xFF)

#define MOPI_MSG_TYPE_SHIFT		(0)
#define MOPI_MSG_TYPE_MASK		(0x7F)

#define MOPI_MSG_PRIO_SHIFT		(8)
#define MOPI_MSG_PRIO_MASK		(0xFF00)

#define GBS_MSG_FROM_MOPI		0x10000

enum {
	MOPI_MSG_FLAG_GTIMER				= 1 << 6,
	MOPI_MSG_FLAG_POSTPONED				= 1 << 7,
};

enum {
	MOPI_GTIMER_TYPE_MSG		= 1,
	MOPI_GTIMER_TYPE_CALLBACK	= 2,
};

enum {
	MOPI_MSG_TYPE_DEFAULT			= 1,
	MOPI_MSG_TYPE_UNK				= 3,
	MOPI_MSG_TYPE_GTIMER_MSG		= MOPI_MSG_FLAG_GTIMER | 1,
	MOPI_MSG_TYPE_GTIMER_CALLBACK	= MOPI_MSG_FLAG_GTIMER | 2,
	MOPI_MSG_TYPE_EXEC				= 0x77, // only for simulator
};

struct MOPI_GBS_MSG {
	int submess;
	void *data0;
	void *data1;
};

struct MOPI_GTIMER_MSG {
	GBSTMR *tmr;
	GBSTMR_CALLBACK callback;
	short unk;
	short unk2;
};

/**
 * MOPI Message.
 * */
struct MOPI_MSG {
	MOPI_MSG *next;
	void (*onDestroy)(MOPI_MSG **msg);	/*< Destructor */
	int flags;
	short cepid_to;		/**< Receiver cepid */
	short cepid_from;	/**< Sender cepid */
	short msgid;		/**< Message ID */
	short field6_0x12;
	short size;			/**< Payload size */
	short field8_0x16;
};

/**
 * MOPI Processor.
 * */
struct MOPI_PROC {
	void *task_config;
	short cepid;
	uint8_t field2_0x6;
	uint8_t field3_0x7;
	char *name;
	uint32_t unk1;
	void *onCreate;
	void *onMessage;
	void *onDestroy;
};

void *MOPI_CreateMessage(MOPI_MSG **msg, short cepid, int msgid, int size);

template<typename T>
T *MOPI_CreateMessage(MOPI_MSG **msg, short cepid, int msgid) {
	return reinterpret_cast<T *>(MOPI_CreateMessage(msg, cepid, msgid, sizeof(T)));
}

MOPI_GTIMER_MSG *MOPI_CreateGTimerMessage(MOPI_MSG **msg, short cepid_to, short msgid, int prio, int type);
MOPI_GBS_MSG *MOPI_CreateGbsMessage(MOPI_MSG **msg, int cepid, int cepid_from, short msgid, int flags);
void MOPI_FreeMessage(MOPI_MSG **msg);
void *MOPI_GetPayload(MOPI_MSG **msg);

template<typename T>
T *MOPI_GetPayload(MOPI_MSG **msg) {
	if (!msg || !*msg)
		throw std::runtime_error(std::format("MOPI_GetPayload<{}>: message is null", typeid(T).name()));
	if ((*msg)->size != sizeof(T))
		throw std::runtime_error(std::format("MOPI_GetPayload<{}>: msg.size={} != sizeof({})", typeid(T).name(), (*msg)->size, typeid(T).name()));
	return reinterpret_cast<T *>(MOPI_GetPayload(msg));
}

int MOPI_PostMessage(MOPI_MSG **msg);
void MOPI_ConvertToGBS(GBS_MSG *gbs_msg, MOPI_MSG *mopi_msg);

void GBS_RunInContext(int cepid, std::function<void()> callback);

static inline uint32_t MOPI_GetMessageType(MOPI_MSG **msg) {
	return ((*msg)->flags & MOPI_MSG_TYPE_MASK) >> MOPI_MSG_TYPE_SHIFT;
}

static inline uint32_t MOPI_GetMessageFlags(MOPI_MSG **msg) {
	return ((*msg)->flags & MOPI_MSG_FLAGS_MASK) >> MOPI_MSG_FLAGS_SHIFT;
}

static inline uint32_t MOPI_GetMessagePriority(MOPI_MSG **msg) {
	return ((*msg)->flags & MOPI_MSG_PRIO_MASK) >> MOPI_MSG_PRIO_SHIFT;
}
