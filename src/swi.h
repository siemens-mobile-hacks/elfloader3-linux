#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>

#define NEWSGOLD 1
// #define ELKA 1

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
int GetFreeRamAvail();

/*
 * libc
 * */
typedef int _jmp_buf[11];

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
int SWI_setjmp(_jmp_buf env);
int SWI_snprintf(char * param1, int n, const char *format, ...);
void *SWI_memset(void *s, int c, size_t n);
void *SWI_calloc(size_t nelem, size_t elsize);
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

int FS_open(const char *cFileName, uint32_t iFileFlags, uint32_t iFileMode, uint32_t *ErrorNumber);
int FS_read(int FileHandler, void *cBuffer, int iByteCount, uint32_t *ErrorNumber);
int FS_write(int FileHandler, void const * cBuffer, int iByteCount, uint32_t *ErrorNumber);
int FS_close(int FileHandler, uint32_t *ErrorNumber);
int FS_flush(int stream, uint32_t *ErrorNumber);
long FS_lseek(int FileHandler, uint32_t offset, uint32_t origin, uint32_t *ErrorNumber, uint32_t *ErrorNumber2);
int FS_mkdir(const char * cFileName, uint32_t *ErrorNumber);
int FS_GetFileAttrib(const char *cFileName, uint8_t *cAttribute, uint32_t *ErrorNumber);
int FS_SetFileAttrib(const char *cFileName, uint8_t cAttribute, uint32_t *ErrorNumber);
int FS_setfilesize(int FileHandler, uint32_t iNewFileSize, uint32_t *ErrorNumber);
int FS_FindFirstFile(DIR_ENTRY *DIRENTRY, const char *mask, uint32_t *ErrorNumber);
int FS_FindNextFile(DIR_ENTRY *DIRENTRY,uint32_t *ErrorNumber);
int FS_FindClose(DIR_ENTRY *DIRENTRY,uint32_t *ErrorNumber);
int FS_fmove(const char * SourceFileName, const char * DestFileName, uint32_t *ErrorNumber);
int FS_rmdir(const char * cDirectory, uint32_t *ErrorNumber);
int FS_truncate(int FileHandler, int length, int *errornumber);
int FS_isdir(const char * cDirectory, uint32_t *ErrorNumber);
int FS_GetFileStats(const char *cFileName, FSTATS * StatBuffer, uint32_t *errornumber);

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
int wstrcmp(WSHDR *ws1, WSHDR *ws2);
void wstrcpybypos(WSHDR *dest, WSHDR *src, uint16_t from, uint16_t len);
void wsRemoveChars(WSHDR *ws, uint16_t from, uint16_t to);
int ws_2utf8( WSHDR *from, char *to, int *result_length, uint16_t max_len);
int utf8_2ws(WSHDR *ws, const char *utf8_str, uint16_t maxLen);
uint16_t wstrchr(WSHDR *ws, uint16_t start_pos, uint16_t wchar);
uint16_t wstrrchr(WSHDR *ws, uint16_t max_pos, uint16_t wchar);
void wsAppendChar(WSHDR *ws, uint16_t wchar);
int wsInsertChar(WSHDR *ws, uint16_t wchar, uint16_t pos);

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
typedef struct {
#ifdef NEWSGOLD
  int pid_from;
  int msg;
#else
  short pid_from;
  short msg;
#endif
  int submess;
  void *data0;
  void *data1;
} GBS_MSG;

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
	
/*
 * CSM
 * */
typedef struct CSM_DESC CSM_DESC;
typedef struct CSM_RAM CSM_RAM;

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

CSMROOT *CSM_root();
int CreateCSM(const CSM_DESC *desc, const void *default_value, int param3);
CSM_RAM *FindCSMbyID(int id);
void DoIDLE();
CSM_RAM *FindCSM(CSMQ *q, int id);
void CloseCSM(int id);

// Non-SWI methods
void CSM_Init();
CSM_RAM *CSM_Current();

/*
 * GUI
 * */
enum {
	CSM_GUI_STATE_CLOSED		= 0,
	CSM_GUI_STATE_UNFOCUSED		= 1,
	CSM_GUI_STATE_FOCUSED		= 2,
};

typedef struct {
	short x;
	short y;
	short x2;
	short y2;
} RECT;

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
	void (*onKey)(GUI *, GUI_MSG *);
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

GUI_RAM *GUI_GetById(int id);
GUI_RAM *GUI_GetPrev(int id);
GUI_RAM *GUI_GetNext(int id);

int GUI_Create_ID(GUI *gui, int id);
int GUI_Create(GUI *gui);
int GUI_Create_30or2(GUI *gui);
int GUI_CreateWithDummyCSM(GUI *gui, int flag);
int GUI_CreateWithDummyCSM_30or2(GUI *gui, int flag);

void GeneralFuncF1(int cmd);
void GeneralFuncF0(int cmd);
void GeneralFunc_flag1(int id, int cmd);
void GeneralFunc_flag0(int id, int cmd);

void GUI_DirectRedrawGUI();
void GUI_DirectRedrawGUI_ID(int id);
void GUI_PendedRedrawGUI();

void GUI_DrawString(WSHDR *wshdr, int x1, int y1, int x2, int y2, int font, int text_attribute, const char *pen, const char *brush);
void GUI_DrawRoundedFrame(int x1, int y1, int x2, int y2, int x_round, int y_round, int flags, const char *pen, const char *brush);
void GUI_DrawLine(int x, int y, int x2, int y2, int flags, const char *pen);
void GUI_DrawRectangle(int x, int y, int x2, int y2, int flags, const char *pen, const char *brush);

void GUI_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int flags, char *pen, char *brush);
void GUI_DrawPixel(int x, int y, const char *color);
void GUI_DrawArc(int x1, int y1, int x2, int y2, int start, int end, int flags, char *pen, char *brush);

char *GUI_GetPaletteAdrByColorIndex(int index);
void GUI_GetRGBcolor(int index, char *dest);
void GUI_GetRGBbyPaletteAdr(char *addr, char *dest);
void GUI_SetColor(int a, int r, int g, int b, char *dest);
uint32_t GUI_Color2Int(const char *color);

int GUI_ScreenW();
int GUI_ScreenH();
int GUI_HeaderH();
int GUI_SoftkeyH();

void GUI_SetIDLETMR(int time_ms, int msg);
void GUI_RestartIDLETMR();
void GUI_DisableIDLETMR();

void GUI_DisableIconBar(int disable);
void GUI_AddIconToIconBar(int pic, short *num);
