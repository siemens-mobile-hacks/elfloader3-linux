#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>

#include "gui/Bitmap.h"
#include "gui/Painter.h"

#define NEWSGOLD 1
#define ELKA 1

#ifdef ELKA
#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	320
#else
#define SCREEN_WIDTH	132
#define SCREEN_HEIGHT	176
#endif

#define SWI_TRACE 0

typedef void *(*malloc_func_t)(size_t);
typedef void (*mfree_func_t)(void *);

/*
 * Internals
 * */
extern void *switab_functions[4096];

void loader_init_switab();
void loader_swi_stub(int swi);

/*
 * System
 * */
int SYS_GetFreeRamAvail();
uint16_t *SYS_RamCap();

/*
 * libc
 * */
char *SWI_strpbrk(const char *s1, const char *s2);
size_t SWI_strcspn(const char *s1, const char *s2);
char *SWI_strncat(char *dest, const char *substr, size_t maxSubLen);
int SWI_strncmp(const char *s1, const char *s2, size_t n);
char *SWI_strncpy(char *dest, const char *source, size_t maxlen);
char *SWI_strrchr(const char *s, int c);
char *SWI_strstr(const char *s1, const char *s2);
long SWI_strtol(const char *nptr, char **endptr, int base);
unsigned long SWI_strtoul(const char *nptr, char **endptr, int base);
int SWI_memcmp(const void *m1, const void *m2, size_t n);
void SWI_zeromem(void *dest, int n);
void *SWI_memcpy(void *dest, const void *source, size_t cnt);
void *SWI_memmove(void *dest, const void *source, size_t cnt);
int SWI_setjmp(int *env);
int SWI_snprintf(char * param1, int n, const char *format, ...);
void *SWI_memset(void *s, int c, size_t n);
int SWI_strcmpi(const char *s1, const char *s2);
int SWI_StrToInt(char *s, char **endp);
int SWI_sprintf(char *buffer, const char *format, ...);
char *SWI_strcat(char *param1, const char *param2);
char *SWI_strchr(const char *param1, int param2);
int SWI_strcmp(const char *param1, const char *param2);
char *SWI_strcpy(char *dest, const char *source);
uint32_t SWI_strlen(const char *param1);

/*
 * Filesystem
 * */

// Permissions
#define P_WRITE 0x100
#define P_READ 0x80

// Access
#define A_ReadOnly 0
#define A_WriteOnly 1
#define A_ReadWrite 2
#define A_NoShare 4
#define A_Append 8
#define A_Exclusive 0x10
#define A_MMCStream 0x20
#define A_Create 0x100
#define A_Truncate 0x200
#define A_FailCreateOnExist 0x400
#define A_FailOnReopen 0x800

#define A_TXT 0x4000
#define A_BIN 0x8000

// Attributes
#define FA_READONLY 0x0001
#define FA_HIDDEN 0x0002
#define FA_SYSTEM 0x0004
#define FA_DIRECTORY 0x0010
#define FA_ARCHIVE 0x0020

#pragma pack(1)
typedef struct {
#ifdef NEWSGOLD
  void *priv;
  char unk0[40 - sizeof(void *)];
  uint32_t file_size;
  short file_attr;
  uint32_t create_date_time;
  char unk1[2];
  char folder_name[128];
  char unk2;
  char file_name[128];
  char unk3[11];
  short wsfolder_name[128+1];
  short wsfile_name[128+1];
#else
  void *priv;
  char unk1[14 - sizeof(void *)];
  short file_attr;
  uint32_t create_date_time;
  uint32_t file_size;
  char unk2[16];
  char folder_name[128];
  char unk3;
  char file_name[271];
#endif
} DIR_ENTRY;
#pragma pack()

typedef struct {
  char unk1;
  char unk2;
  char unk3;
  char unk4;
  uint32_t size;
  char unk5[28];
  int file_attr;
} FSTATS;

int FS_open(const char *path, uint32_t f, uint32_t m, uint32_t *errp);
int FS_read(int fd, void *buff, int count, uint32_t *errp);
int FS_write(int fd, void const *buff, int count, uint32_t *errp);
int FS_close(int fd, uint32_t *errp);
int FS_flush(int fd, uint32_t *errp);
long FS_lseek(int fd, uint32_t offset, uint32_t origin, uint32_t *errp, uint32_t *errp2);
int FS_mkdir(const char *pathname, uint32_t *errp);
int FS_GetFileAttrib(const char *cFileName, uint8_t *cAttribute, uint32_t *errp);
int FS_SetFileAttrib(const char *cFileName, uint8_t cAttribute, uint32_t *errp);
int FS_setfilesize(int FileHandler, uint32_t iNewFileSize, uint32_t *errp);
int FS_FindFirstFile(DIR_ENTRY *de, const char *mask, uint32_t *errp);
int FS_FindNextFile(DIR_ENTRY *de ,uint32_t *errp);
int FS_FindClose(DIR_ENTRY *de, uint32_t *errp);
int FS_fmove(const char *src, const char *dst, uint32_t *errp);
int FS_rmdir(const char *path, uint32_t *errp);
int FS_unlink(const char *path, uint32_t *errp);
int FS_truncate(int fd, int length, uint32_t *errp);
int FS_isdir(const char *path, uint32_t *errp);
int FS_GetFileStats(const char *path, FSTATS *st, uint32_t *errp);

/*
 * Date & Time
 * */
typedef struct {
	uint32_t year;
	uint8_t month;
	uint8_t day;
} TDate;

typedef struct {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint32_t millisec;
} TTime;

typedef struct {
  int8_t yearNormBudd; //1 - norm, 2 - buddhist
  int8_t dateFormat;
  int8_t timeFormat; // 0-24h, 1-12h
  int8_t timeZone; // internal representation
  int8_t isAutoTime1;
  int8_t isAutoTime2;
} TDateTimeSettings;

void GetDateTime(TDate *param1, TTime *param2);
char GetWeek(TDate *param1);
int GetTimeZoneShift(TDate *param1, TTime *param2, int timeZone);
TDateTimeSettings *RamDateTimeSettings();

/*
 * Wide String
 * */
typedef struct {
	uint16_t len;
	uint16_t data[];
} WSBODY;

typedef struct {
	union {
		WSBODY *body;
		uint16_t *wsbody;
	};
	void *(*ws_malloc)(size_t);
	void (*ws_mfree)(void *);
	int isbody_allocated;
	uint16_t maxlen;
	uint16_t unk1;
} WSHDR;

WSHDR *wstrcpy(WSHDR *wshdr_d, WSHDR *wshdr_s);
WSHDR *wstrncpy(WSHDR *param1, WSHDR * param2, uint16_t param3);
WSHDR *wstrcat(WSHDR *wshdr_d, WSHDR *wshdr_s);
WSHDR *wstrncat(WSHDR *wshdr_d, WSHDR *wshdr_s, uint16_t n);
int wstrlen(WSHDR *wshdr);
int wsprintf(WSHDR* param1, const char *format,...);
void wstrcatprintf(WSHDR *param1, const char *format,...);
WSHDR *AllocWS(uint16_t len);
void CutWSTR(WSHDR *wshdr, uint16_t len);
WSHDR *CreateLocalWS(WSHDR *wshdr, uint16_t *wsbody, uint16_t len);
WSHDR *CreateWS(void *(*ws_malloc)(size_t), void (*ws_mfree)(void *), uint16_t len);
void FreeWS(WSHDR *wshdr);
int str_2ws(WSHDR *ws, const char *str, uint16_t size);
void ws_2str(WSHDR *ws, char *str, uint16_t size);
int wstrcmp(WSHDR *ws1, WSHDR *ws2);
void wstrcpybypos(WSHDR *dest, WSHDR *src, uint16_t from, uint16_t len);
void wsRemoveChars(WSHDR *ws, uint16_t from, uint16_t to);
int ws_2utf8( WSHDR *from, char *to, int *result_length, uint16_t max_len);
int utf8_2ws(WSHDR *ws, const char *utf8_str, uint16_t maxLen);
uint16_t wstrchr(WSHDR *ws, uint16_t start_pos, uint16_t wchar);
uint16_t wstrrchr(WSHDR *ws, uint16_t max_pos, uint16_t wchar);
void wsAppendChar(WSHDR *ws, uint16_t wchar);
int wsInsertChar(WSHDR *ws, uint16_t wchar, uint16_t pos);
std::string ws2string(WSHDR *ws); // internal

/*
 * Lock
 * */
typedef struct {
	void *p;
	int i;
} MUTEX;

void LockSched();
void UnlockSched();
void MutexCreate(MUTEX *mtx);
void MutexDestroy(MUTEX *mtx);
void MutexLock(MUTEX *mtx);
void MutexLockEx(MUTEX *mtx, int flag);
void MutexUnlock(MUTEX *mtx);

/*
 * GBS
 * */

#ifdef NEWSGOLD

#define MSG_PLAYFILE_REPORT 0x70BC
#define MSG_CSM_DESTROYED 0x6400
#define MSG_CSM_DESTROY_NOTIFY 0x6402
#define MSG_GUI_DESTROYED 0x640E
#define MSG_IDLETMR_EXPIRED 0x6401

#define MMI_CEPID 0x4209
#define HELPER_CEPID 0x440A

#else

#define MSG_CSM_DESTROYED 0x93
#define MSG_CSM_DESTROY_NOTIFY 0x92
#define MSG_GUI_DESTROYED 0x98
#define MSG_IDLETMR_EXPIRED 0x95

#define MMI_CEPID 0x4209
#define HELPER_CEPID 0x4407

#endif

typedef struct {
#ifdef NEWSGOLD
	short pid_from;
	short unk;
	int msg;
#else
	short pid_from;
	short msg;
#endif
	int submess;
	void *data0;
	void *data1;
} GBS_MSG;

#define GBS_TMR_CREATED 0x74696d72 // "timr"
#define GBS_TMR_DELETED 0x756e6466 // "undf"

typedef struct {
	int param0;
	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	int param6;
} GBSTMR;

typedef void (*GbsProcCallback)(void);
typedef void (*GbsTimerCallback)(GBSTMR *);

void GBS_StartTimerProc(GBSTMR *tmr, long ms, GbsTimerCallback callback);
void GBS_StartTimer(GBSTMR *tmr, int ms, int msg, int unk, int cepid);
void GBS_StopTimer(GBSTMR *tmr);
int GBS_IsTimerProcessing(GBSTMR * param1);
void GBS_RunInContext(int cepid, std::function<void()> callback);
void GBS_CreateProc(int cepid, const char *name, GbsProcCallback on_msg, int prio, int unk_zero);
int GBS_GetCurCepid(void);
void GBS_KillProc(int cepid);
void GBS_PendMessage(GBS_MSG * param1);
void GBS_SendMessage(int cepid_to, int msg, int submess, void *data0, void *data1);
int GBS_ReciveMessage(GBS_MSG * param1);
void GBS_AcceptMessage(void);
void GBS_ClearMessage(void);
int GBS_RecActDstMessage(GBS_MSG * param1);
int GBS_WaitForMsg(const int *msg_list, int msgs_num, GBS_MSG* param3, int timeout);
void GBS_DelTimer(GBSTMR * param1);

/*
 * LinkedList
 * */
typedef struct {
	void *first;
	void *last;
	void (*data_mfree)(void *);
} LLQ;

typedef struct {
	void *next;
	void *prev;
} LLIST;

// Non-SWI methods
void linked_list_init(LLQ *q);
void linked_list_push(LLQ *head, void *item);
void linked_list_remove(LLQ *head, void *item_ptr);

/*
 * CSM
 * */
typedef struct CSM_DESC CSM_DESC;
typedef struct CSM_RAM CSM_RAM;

enum {
	CSM_STATE_OPEN		= 0,
	CSM_STATE_CLOSED	= -3,
};

struct CSM_RAM {
	CSM_RAM *next;
	CSM_RAM *prev;
	const CSM_DESC *desc;
	int id;
	int state;
	int unk1;
	int unk2;
	LLQ gui_ll;
};

struct CSM_DESC {
	int (*onMessage)(CSM_RAM *, GBS_MSG *);
	void (*onCreate)(CSM_RAM *);
#ifdef NEWSGOLD
	int zero1;
	int zero2;
	int zero3;
	int zero4;
#endif
	void (*onClose)(CSM_RAM *);
	int datasize;
	int statesize;
	const int *states;
};

#ifdef NEWSGOLD
typedef struct {
	int id;
	CSM_RAM *current_msg_processing_csm;
	LLQ csm;
	LLQ cmd;
	LLQ csm_background;
} CSMQ;
#else
typedef struct {
	int id;
	CSM_RAM *current_msg_processing_csm;
	LLQ csm;
	LLQ cmd;
	int unk;
	int unk1;
	void *methods;
	int unk2;
	void *tmr;
	short cepid;
} CSMQ;
#endif

typedef struct {
	int under_idle_id;
	int idle_id;
	CSMQ *csm_q;
} CSMROOT;

void GBS_Init();
CSMROOT *CSM_root();
int CSM_Create(const CSM_DESC *desc, const void *default_value, int param3);
void CSM_Close(CSM_RAM *csm);
void CSM_GarbageCollector();
CSM_RAM *FindCSMbyID(int id);
void DoIDLE();
CSM_RAM *FindCSM(CSMQ *q, int id);
void CloseCSM(int id);

// Non-SWI methods
void CSM_Init();
CSM_RAM *CSM_Current();

/*
 * Images
 * */
enum {
	IMGHDR_TYPE_WB		= 1,
	IMGHDR_TYPE_RGB332	= 5,
	IMGHDR_TYPE_RGB565	= 8,
	IMGHDR_TYPE_RGB8888	= 10
};

typedef struct {
#ifdef ELKA
	uint16_t w;
	uint16_t h;
	int bpnum; //For BW=1, 8bit=5, 16bit=8, 0x80 - packed
#else
	uint8_t w;
	uint8_t h;
	uint16_t bpnum; //For BW=1, 8bit=5, 16bit=8, 0x80 - packed
#endif
	uint8_t *bitmap;
} IMGHDR;

enum {
	EIMGHDR_TRANSPARENT_8BIT		= 0xC0,
	EIMGHDR_TRANSPARENT_16BIT		= 0xE000
};

enum {
	EIMGHDR_TYPE_RGB565		= 8,
	EIMGHDR_TYPE_RGB8888	= 10
};

typedef struct{
	int bpnum; // 16bit = 8, 32bit = 0x0A, 0x80 - packed
	int w;
	int h;
	int zero;     // 0x00000000
	int tp_pixel; // Пиксель прозрачности 16 бит 0xE000, 8 бит 0xC0
	char *bitmap;
} EIMGHDR;

IMGHDR *IMG_LoadAny(const std::string &path, bool only_rgb888 = true);
IMGHDR *IMG_CreateIMGHDRFromPngFile(const char *fname, int type);
int IMG_CalcBitmapSize(short w,short h, char typy);
Bitmap::Type IMG_GetBitmapType(int bpnum);

/*
 * GUI
 * */
enum {
	CSM_GUI_STATE_CLOSED		= 0,
	CSM_GUI_STATE_UNFOCUSED		= 1,
	CSM_GUI_STATE_FOCUSED		= 2,
};

// UTF16 formating
#define UTF16_UNDERLINE			0xE001 // TEXT_UNDERLINE
#define UTF16_NO_UNDERLINE		0xE002

#define UTF16_BG_INVERTION		0xE003 // TEXT_INVERT
#define UTF16_BG_INVERTION2		0xE005 // TEXT_INVERT2
#define UTF16_NO_INVERTION		0xE004

#define UTF16_TEXT_COLOR_RGBA	0xE006 // Example: { UTF16_TEXT_COLOR_RGBA, 0xRRGG, 0xBBAA }
#define UTF16_BG_COLOR_RGBA		0xE007 // Example: { UTF16_BG_COLOR_RGBA, 0xRRGG, 0xBBAA }

#define UTF16_TEXT_COLOR		0xE008 // Example: { UTF16_TEXT_COLOR, PC_FOREGROUND }
#define UTF16_BG_COLOR			0xE009 // Example: { UTF16_BG_COLOR, PC_BACKGROUND }

#define UTF16_FONT_RESET		0xE010 // Reset font to default

#define UTF16_FONT_SMALL		0xE012 // FONT_SMALL
#define UTF16_FONT_SMALL_BOLD	0xE013 // FONT_SMALL_BOLD

#define UTF16_FONT_MEDIUM		0xE014 // FONT_MEDIUM
#define UTF16_FONT_MEDIUM_BOLD	0xE015 // FONT_MEDIUM_BOLD

#define UTF16_FONT_LARGE_BOLD	0xE017 // FONT_LARGE_BOLD
#define UTF16_FONT_LARGE		0xE018 // FONT_LARGE

#define UTF16_FONT_UNK0			0xE011 // FONT_SMALL
#define UTF16_FONT_UNK1			0xE016 // FONT_LARGE_BOLD
#define UTF16_FONT_UNK2			0xE019 // FONT_SMALL
#define UTF16_FONT_UNK3			0xE01A // FONT_SMALL_BOLD
#define UTF16_FONT_UNK4			0xE01B // FONT_SMALL

// Font sizes
#ifdef NEWSGOLD
	#ifdef ELKA
		#define FONT_LARGE 0
		#define FONT_LARGE_BOLD 1
		#define FONT_LARGE_ITALIC 2
		#define FONT_LARGE_ITALIC_BOLD 3
		#define FONT_MEDIUM 4
		#define FONT_MEDIUM_BOLD 5
		#define FONT_MEDIUM_ITALIC 6
		#define FONT_MEDIUM_ITALIC_BOLD 7
		#define FONT_NUMERIC_SMALL 8
		#define FONT_NUMERIC_SMALL_BOLD 9
		#define FONT_NUMERIC_XSMALL 8
		#define FONT_SMALL 8
		#define FONT_SMALL_BOLD 9
		#define FONT_SMALL_ITALIC 10
		#define FONT_SMALL_ITALIC_BOLD 11
		#define FONT_NUMERIC_LARGE 0
		#define FONT_NUMERIC_MEDIUM 4
	#else
		#define FONT_LARGE 0
		#define FONT_LARGE_BOLD 1
		#define FONT_LARGE_ITALIC 2
		#define FONT_LARGE_ITALIC_BOLD 3
		#define FONT_MEDIUM 4
		#define FONT_MEDIUM_BOLD 5
		#define FONT_MEDIUM_ITALIC 6
		#define FONT_MEDIUM_ITALIC_BOLD 7
		#define FONT_NUMERIC_SMALL 8
		#define FONT_NUMERIC_SMALL_BOLD 9
		#define FONT_NUMERIC_XSMALL 10
		#define FONT_SMALL 11
		#define FONT_SMALL_BOLD 12
		#define FONT_SMALL_ITALIC 13
		#define FONT_SMALL_ITALIC_BOLD 14
		#define FONT_NUMERIC_LARGE 15
		#define FONT_NUMERIC_MEDIUM 16
	#endif
#else
	#define FONT_LARGE 0
	#define FONT_LARGE_BOLD 1
	#define FONT_LARGE_ITALIC 0
	#define FONT_LARGE_ITALIC_BOLD 1
	#define FONT_MEDIUM 2
	#define FONT_MEDIUM_BOLD 3
	#define FONT_MEDIUM_ITALIC 2
	#define FONT_MEDIUM_ITALIC_BOLD 3
	#define FONT_NUMERIC_SMALL 4
	#define FONT_NUMERIC_SMALL_BOLD 5
	#define FONT_NUMERIC_XSMALL 6
	#define FONT_SMALL 7
	#define FONT_SMALL_BOLD 8
	#define FONT_SMALL_ITALIC 7
	#define FONT_SMALL_ITALIC_BOLD 8
	#define FONT_NUMERIC_LARGE 9
	#define FONT_NUMERIC_MEDIUM 10
#endif

// DrawString flags
#define TEXT_ALIGNLEFT 1
#define TEXT_ALIGNMIDDLE 2
#define TEXT_ALIGNRIGHT 4
#define TEXT_UNDERLINE 8
#define TEXT_INVERT 16
#define TEXT_OUTLINE 32
#define TEXT_PASSWORD 64
#define TEXT_NOFORMAT 128
#define TEXT_INVERT2 256

// DrawArc flags
#define ARC_DRAW_OUTLINE 1

// DrawRectangle flags
#define RECT_DOT_OUTLINE 1
#define RECT_FILL_WITH_PEN 2
#define RECT_DRAW_INVERT 4

// DrawLine flags
#define LINE_DOTTED 1
#define LINE_DOTTED2 2

typedef struct {
	short x;
	short y;
	short x2;
	short y2;
} RECT;

enum {
	DRWOBJ_ELLIPSE_SECTION_UPPER_RIGHT = 1,
	DRWOBJ_ELLIPSE_SECTION_UPPER_LEFT,
	DRWOBJ_ELLIPSE_SECTION_LOWER_LEFT,
	DRWOBJ_ELLIPSE_SECTION_LOWER_RIGHT,
};

enum {
	DRWOBJ_TYPE_RECT						= 0x00,
	DRWOBJ_TYPE_TEXT						= 0x01,
	DRWOBJ_TYPE_SCROLLING_TEXT				= 0x03,
	DRWOBJ_TYPE_RECT_EX						= 0x04,
	DRWOBJ_TYPE_IMG							= 0x05,
	DRWOBJ_TYPE_PIXEL						= 0x0B,
	DRWOBJ_TYPE_LINE						= 0x0F,
	DRWOBJ_TYPE_STROKE_ELLIPSE_SECTION		= 0x10,
	DRWOBJ_TYPE_FILLED_ELLIPSE_SECTION		= 0x11,
	DRWOBJ_TYPE_PIE							= 0x13,
	DRWOBJ_TYPE_ARC							= 0x12,
	DRWOBJ_TYPE_TRIANGLE					= 0x15,
	DRWOBJ_TYPE_EIMG						= 0x17,
};

typedef struct {
	uint8_t type;
	uint8_t rect_flags;
	uint8_t unk2;
	char color1[4];
	char color2[4];
	uint8_t unk3;
	RECT rect;
	
	union {
		char dummy[0x10];
		
		struct {
			WSHDR *value;
			uint8_t font;
			uint8_t offset_x;
			uint8_t offset_y;
			uint8_t unk5;
			uint16_t flags;
			uint8_t xdisp;
		} text;
		
		struct {
			IMGHDR *value;
			uint8_t font;
			uint8_t offset_x;
			uint8_t offset_y;
			uint8_t unk5;
			uint16_t flags;
		} img;
		
		struct {
			void *value;
			int offset_x;
			int offset_y;
			uint16_t flags;
		} eimg;
		
		struct {
			int16_t x;
			int16_t y;
			int16_t x2;
			int16_t y2;
			uint16_t flags;
		} line;
		
		struct {
			int16_t x1;
			int16_t y1;
			int16_t x2;
			int16_t y2;
			int16_t x3;
			int16_t y3;
			uint16_t flags;
		} triangle;
		
		struct {
			int16_t x;
			int16_t y;
			int16_t w;
			int16_t h;
			int16_t start;
			int16_t end;
			uint16_t flags;
		} arc;
		
		struct {
			int16_t x;
			int16_t y;
			int16_t radius_x;
			int16_t radius_y;
			uint16_t flags;
		} ellipse_section;
		
		struct {
			uint8_t fill_mode;
			uint8_t fill_value;
		} rectangle;
	};
} DRWOBJ;

static_assert(sizeof(DRWOBJ) == 0x24);

enum {
	LCDLAYER_DEPTH_TYPE_16BIT = 4,
	LCDLAYER_DEPTH_TYPE_32BIT = 5
};

typedef struct {
	int w;
	int h;
	int unk0;
	int unk1;
	RECT rc;
	void *buffer;
	void *unk2;
	uint8_t depth;
	char unk3[117];
} LCDLAYER;

typedef struct GUI GUI;

typedef struct {
	char zero;
	char unk1;
	short keys;
	GBS_MSG *gbsmsg;
} GUI_MSG;

typedef struct {
	void (*onRedraw)(GUI *);
	void (*onCreate)(GUI *, malloc_func_t);
	void (*onClose)(GUI *, mfree_func_t);
	void (*onFocus)(GUI *, malloc_func_t, mfree_func_t);
	void (*onUnfocus)(GUI *, mfree_func_t);
	int (*onKey)(GUI *, GUI_MSG *);
	void *unk0;
	void (*onDestroy)(GUI *, mfree_func_t);
	void (*method8)();
	void (*method9)();
	void *unk1;
} GUI_METHODS;

struct GUI_RAM {
	GUI_RAM *next;
	GUI_RAM *prev;
	GUI *gui;
	int id;
};

#ifdef NEWSGOLD
struct GUI {
	RECT *canvas;
	GUI_METHODS *methods;
	void *definition;
	char state;
	char unk2;
	char unk3;
	char unk4;
	int color1;
	int color2;
	LLQ item_ll;
	int unk5;
	char unk6;
	char unk7;
	char unk8;
	char unk9;
	int unk10;
	int flag30;
};
#else
struct GUI {
	RECT *canvas;
	GUI_METHODS *methods;
	void *definition;
	char state;
	char unk2;
	char unk3;
	char unk4;
	int color1;
	int color2;
	LLQ item_ll;
	int unk5;
	char unk6;
	char unk7;
	char unk8;
	char unk9;
	int unk10;
	int flag30;
};
#endif

enum {
	DRAW_RECT_FLAG_DOTTED		= 0 << 1,
	DRAW_RECT_FLAG_NO_BORDER	= 0 << 2,
};

void GUI_Init();

void GUI_WalkRenderTree(const std::function<bool(GUI_RAM *)> &callback, bool reverse = false);
void GUI_SyncStates();

GUI *GUI_GetFocusedTop();
int GUI_GetTopID();
bool GUI_IsOnTop(int id);

void GUI_DoFocus(int id);
void GUI_DoUnFocus(int id);

Painter *GUI_GetPainter();

GUI_RAM *GUI_GetById(int id);
GUI_RAM *GUI_GetPrev(int id);
GUI_RAM *GUI_GetNext(int id);

int GUI_Create_ID(GUI *gui, int id);
int GUI_Create(GUI *gui);
int GUI_Create_30or2(GUI *gui);
int GUI_CreateWithDummyCSM(GUI *gui, int flag);
int GUI_CreateWithDummyCSM_30or2(GUI *gui, int flag);

void GUI_Close(int id);
void GUI_GarbageCollector();

void GUI_GeneralFuncF1(int cmd);
void GUI_GeneralFuncF0(int cmd);
void GUI_GeneralFunc_flag1(int id, int cmd);
void GUI_GeneralFunc_flag0(int id, int cmd);

void GUI_DirectRedrawGUI();
void GUI_DirectRedrawGUI_ID(int id);
void GUI_PendedRedrawGUI();
void GUI_REDRAW();
void GUI_IpcRedrawScreen();

void GUI_DrawString(WSHDR *ws, int x1, int y1, int x2, int y2, int font, int text_attribute, const char *pen, const char *brush);
void GUI_DrawScrollString(WSHDR *ws, int x1, int y1, int x2, int y2, int xdisp, int font, int text_attribute, const char *pen, const char *brush);
void GUI_DrawRoundedFrame(int x1, int y1, int x2, int y2, int x_round, int y_round, int flags, const char *pen, const char *brush);
void GUI_DrawLine(int x, int y, int x2, int y2, int flags, const char *pen);
void GUI_DrawRectangle(int x, int y, int x2, int y2, int flags, const char *pen, const char *brush);
void GUI_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush);
void GUI_DrawPixel(int x, int y, const char *color);
void GUI_DrawArc(int x1, int y1, int x2, int y2, int start, int end, int flags, char *pen, char *brush);
void GUI_DrawIMGHDR(int x, int y, IMGHDR *img, const char *pen, const char *brush, uint16_t offset_x, uint16_t offset_y, int w, int h, int flags);
void GUI_DrawImg(int x, int y, int picture);
void GUI_DrawCanvas(void *data, int x1, int y1, int x2, int y2, int flags);
void GUI_DrawImgBW(int x, int y, int picture, char *pen, char *brush);

IMGHDR* GUI_GetPITaddr(int num);
int GUI_GetImgHeight(int picture);
int GUI_GetImgWidth(int picture);

void GUI_DrawObject(DRWOBJ *drw);
void GUI_FreeDrawObject(DRWOBJ *drw);

DRWOBJ *GUI_DrawObjectSetColor(DRWOBJ *drw, const char *color1, const char *color2);
DRWOBJ *GUI_SetProp2Text(DRWOBJ *drw, RECT *rect, int rect_flag, WSHDR *wshdr, int font, int flags);
DRWOBJ *GUI_SetProp2ScrollingText(DRWOBJ *drw, RECT *rect, int rect_flag, WSHDR *wshdr, int xdisp, int font, int flags);
DRWOBJ *GUI_SetProp2ImageOrCanvas(DRWOBJ *drw, RECT *rect, int flags, IMGHDR *img, int offset_x, int offset_y);
DRWOBJ *GUI_SetProp2Image(DRWOBJ *drw, RECT *rect, int flags, IMGHDR *img);
DRWOBJ *GUI_SetProp2Triangle(DRWOBJ *drw, int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush);
DRWOBJ *GUI_SetProp2Rect(DRWOBJ *drw, RECT *rect, int flags);
DRWOBJ *GUI_SetProp2RectEx(DRWOBJ *drw, RECT *rect, int flags, int fill_mode, int fill_value);
DRWOBJ *GUI_SetProp2Arc(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int x2, int y2, int start, int end);
DRWOBJ *GUI_SetProp2Pie(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int x2, int y2, int start, int end);
DRWOBJ *GUI_SetProp2Line(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int x2, int y2);
DRWOBJ *GUI_SetProp2Pixel(DRWOBJ *drw, RECT *rect, int flags, int x, int y);
DRWOBJ *GUI_SetProp2EImage(DRWOBJ *drw, RECT *rect, int flags, EIMGHDR *img, int offset_x, int offset_y);
DRWOBJ *GUI_SetProp2StrokeEllipseSection(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int radius_x, int radius_y, int type);
DRWOBJ *GUI_SetProp2FilledEllipseSection(DRWOBJ *drw, RECT *rect, int flags, int x, int y, int radius_x, int radius_y, int type);
	
inline bool GUI_ColorIsTransparent(const char *color) {
	return !color || color[3] == 0;
}

char *GUI_GetPaletteAdrByColorIndex(int index);
void GUI_GetRGBcolor(int index, char *dest);
void GUI_GetRGBbyPaletteAdr(char *addr, char *dest);
void GUI_SetColor(int a, int r, int g, int b, char *dest);
uint32_t GUI_Color2Int(const char *color);

int GUI_ScreenW();
int GUI_ScreenH();
int GUI_HeaderH();
int GUI_SoftkeyH();

LCDLAYER *GUI_RamMainLCDLayer();
LCDLAYER **GUI_RamMMILCDLayer();

void GUI_SetDepthBuffer(uint8_t depth);
void GUI_SetDepthBufferOnLCDLAYER(LCDLAYER *layer, uint8_t depth);

void GUI_SetIDLETMR(int time_ms, int msg);
void GUI_RestartIDLETMR();
void GUI_DisableIDLETMR();

void GUI_DisableIconBar(int disable);
void GUI_AddIconToIconBar(int pic, short *num);

void GUI_StoreXYWHtoRECT(RECT *rect, int x, int y, int w, int h);
void GUI_StoreXYXYtoRECT(RECT *rect, int x, int y, int x2, int y2);

int GUI_GetFontYSIZE(int font_id);
int GUI_GetSymbolWidth(int ch, int font_id);
int GUI_GetStringWidth(WSHDR *ws, int font_id);

void *GUI_RamScreenBuffer();

void GUI_HandleKeyPress(GBS_MSG *msg);

/*
 * SettingsAE
 * */
int SettingsAE_Update_ws(WSHDR * param1, int set, char *entry, char *keyword);
int SettingsAE_Read_ws(WSHDR * param1, int set, char *entry, char *keyword);
int SettingsAE_SetFlag(int val,int set, char *entry, char *keyword);
int SettingsAE_GetFlag(int *res, int set, char *entry, char *keyword);
int SettingsAE_Update(int val, int set, char *entry, char *keyword);
int SettingsAE_Read(int *res, int set, char *entry, char *keyword);
void *SettingsAE_GetEntryList(int set);
int SettingsAE_RemoveEntry(int set, char *entry, int flag);

/*
 * MMI
 * */
#define KEY_DOWN        0x193
#define KEY_UP          0x194
#define LONG_PRESS      0x195

#ifdef NEWSGOLD
#define LEFT_SOFT       0x01
#define RIGHT_SOFT      0x04
#define GREEN_BUTTON    0x0B
#define RED_BUTTON      0x0C
#define VOL_UP_BUTTON   0x0D
#define VOL_DOWN_BUTTON 0x0E
#define UP_BUTTON       0x3B
#define DOWN_BUTTON     0x3C
#define LEFT_BUTTON     0x3D
#define RIGHT_BUTTON    0x3E
#define ENTER_BUTTON	0x1A
#ifdef ELKA
#define POC_BUTTON 0x15
#define MEDIA_BUTTON	0x11
#define EL_PLAY_BUTTON	0x1D
#else
#define INTERNET_BUTTON 0x11
#endif
#define PLAY_BUTTON	0x15
#define CAMERA_BUTTON	0x14
#else
#define LEFT_SOFT       0x01
#define RIGHT_SOFT      0x04
#define GREEN_BUTTON    0x0B
#define RED_BUTTON      0x0C
#ifdef X75
#define VOL_UP_BUTTON   0x14
#define VOL_DOWN_BUTTON 0x15
#else
#define VOL_UP_BUTTON   0x0D
#define VOL_DOWN_BUTTON 0x0E
#endif
#define UP_BUTTON       0x3B
#define DOWN_BUTTON     0x3C
#define LEFT_BUTTON     0x3D
#define RIGHT_BUTTON    0x3E
#define ENTER_BUTTON	0x1A
#define INTERNET_BUTTON     0x11
#endif
#define PTT_BUTTON	0x27

void MMI_Init();

/*
 * Obs
 * */
struct HObj;

typedef struct {
	int type;
	void *func;
} OBSevent;

HObj *Obs_CreateObject(int uid_in, int uid_out, int prio, int msg_callback, int emb4, int sync, int *errp);
int Obs_DestroyObject(HObj *hObj);
int Obs_SetInput_File(HObj *hObj, int unk_zero, WSHDR *path);
int Obs_GetInputImageSize(HObj *hObj, short *w, short *h);
int Obs_SetOutputImageSize(HObj *hObj, short w, short h);
int Obs_Start(HObj *hObj);
int Obs_Output_GetPictstruct(HObj *hObj, IMGHDR **img);
int Obs_Graphics_SetClipping(HObj *hObj, short x, short y, short w, short h);
int Obs_SetRotation(HObj *hObj, int angle);
int Obs_GetInfo(HObj *hObj, int unk_0or1);
int Obs_SetScaling(HObj *hObj, int unk5);
int Obs_TranslateMessageGBS(GBS_MSG *msg, OBSevent *event_handler);
int Obs_Pause(HObj *hObj);
int Obs_Resume(HObj *hObj);
int Obs_Stop(HObj *hObj);
int Obs_Prepare(HObj *hObj);
int Obs_SetRenderOffset(HObj *hObj, short x, short y);
int Obs_SetPosition(HObj *hObj, int ms);
int Obs_Mam_SetPurpose(HObj *hObj, char purpose);
int Obs_Sound_SetVolumeEx(HObj *hObj, char vol, char delta);
int Obs_Sound_GetVolume(HObj *hObj, char *vol);
int Obs_Sound_SetPurpose(HObj *hObj, int purpose);

/*
 * Explorer
 * */
#ifndef NEWSGOLD
#define UID_PLAY_SOUND 0x30
#else
#define UID_PLAY_SOUND 0x34
#endif
#define UID_ZOOM_PIC 0x2d

#define UID_MP3  0x03
#define UID_M3U  0x04
#define UID_JAR  0x12
#define UID_JAD  0x13
#define UID_MID  0x18
#define UID_AMR  0x19
#define UID_IMY  0x1A
#define UID_SRT  0x1C
#define UID_AAC  0x1D
#define UID_WAV  0x1E
#define UID_JTS  0x20
#define UID_XMF  0x21
#define UID_M4A  0x22
#define UID_BMX  0x23
#define UID_WBMP 0x24
#define UID_BMP  0x25
#define UID_JPG  0x26
#define UID_PNG  0x27
#define UID_GIF  0x28
#define UID_SVG  0x2B
#define UID_3GP  0x32
#define UID_MP4  0x33
#define UID_SDP  0x3E
#define UID_PVX  0x3F
#define UID_SDT  0x40
#define UID_LDB  0x44
#define UID_TXT  0x57
#define UID_URL  0x58

int GetExtUidByFileName_ws(WSHDR *fn);

/*
 * Helper (Elfloader functions)
 * */
#ifdef NEWSGOLD
#define MSG_HELPER_TRANSLATOR 0xDEAE
#else
#define MSG_HELPER_TRANSLATOR 0x3F0
#endif

typedef struct PNGLIST PNGLIST;

struct PNGLIST {
	PNGLIST *next;
	char *pngname;
	IMGHDR *img;
	uint32_t hash;
};

typedef struct DYNPNGICONLIST DYNPNGICONLIST;

struct DYNPNGICONLIST {
	DYNPNGICONLIST *next;
	int icon;
	IMGHDR *img;
};

typedef struct {
	PNGLIST *pltop;
	uint8_t *bitmap;
	DYNPNGICONLIST *dyn_pltop;
} PNGTOP_DESC;

#define PNG_8 1
#define PNG_16 2
#define PNG_24 3

#define PNG_1 0xFF

void Helper_Init();
void SUBPROC(void *func, int p2 = 0, void *p1 = nullptr);
PNGTOP_DESC *Helper_PngTop();
void *Helper_LibTop();
