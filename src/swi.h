#pragma once

#include <cstdint>
#include <cstddef>

#define NEWSGOLD 1
#define SWI_STRACE 0

/*
 * Internals
 * */
extern void *switab_functions[4096];

void loader_init_switab();
void loader_swi_stub(int swi);

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
void *SWI_malloc(size_t size);
void SWI_free(void *param1);
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

int SWI_open(const char *cFileName, uint32_t iFileFlags, uint32_t iFileMode, uint32_t *ErrorNumber);
int SWI_read(int FileHandler, void *cBuffer, int iByteCount, uint32_t *ErrorNumber);
int SWI_write(int FileHandler, void const * cBuffer, int iByteCount, uint32_t *ErrorNumber);
int SWI_close(int FileHandler, uint32_t *ErrorNumber);
int SWI_flush(int stream, uint32_t *ErrorNumber);
long SWI_lseek(int FileHandler, uint32_t offset, uint32_t origin, uint32_t *ErrorNumber, uint32_t *ErrorNumber2);
int SWI_mkdir(const char * cFileName, uint32_t *ErrorNumber);
int SWI_GetFileAttrib(const char *cFileName, uint8_t *cAttribute, uint32_t *ErrorNumber);
int SWI_SetFileAttrib(const char *cFileName, uint8_t cAttribute, uint32_t *ErrorNumber);
int SWI_setfilesize(int FileHandler, uint32_t iNewFileSize, uint32_t *ErrorNumber);
int SWI_FindFirstFile(DIR_ENTRY *DIRENTRY, const char *mask, uint32_t *ErrorNumber);
int SWI_FindNextFile(DIR_ENTRY *DIRENTRY,uint32_t *ErrorNumber);
int SWI_FindClose(DIR_ENTRY *DIRENTRY,uint32_t *ErrorNumber);
int SWI_fmove(const char * SourceFileName, const char * DestFileName, uint32_t *ErrorNumber);
int SWI_rmdir(const char * cDirectory, uint32_t *ErrorNumber);
int SWI_truncate(int FileHandler, int length, int *errornumber);
int SWI_isdir(const char * cDirectory, uint32_t *ErrorNumber);
int SWI_GetFileStats(const char *cFileName, FSTATS * StatBuffer, uint32_t *errornumber);

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

void SWI_GetDateTime(TDate *param1, TTime *param2);
char SWI_GetWeek(TDate *param1);
int SWI_GetTimeZoneShift(TDate *param1, TTime *param2, int timeZone);
TDateTimeSettings *SWI_RamDateTimeSettings(void);

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

WSHDR *SWI_wstrcpy(WSHDR *wshdr_d, WSHDR *wshdr_s);
WSHDR *SWI_wstrncpy(WSHDR *param1, WSHDR * param2, uint16_t param3);
WSHDR *SWI_wstrcat(WSHDR *wshdr_d, WSHDR *wshdr_s);
WSHDR *SWI_wstrncat(WSHDR *wshdr_d, WSHDR *wshdr_s, uint16_t n);
int SWI_wstrlen(WSHDR *wshdr);
int SWI_wsprintf(WSHDR* param1, const char *format,...);
void SWI_wstrcatprintf(WSHDR *param1, const char *format,...);
WSHDR *SWI_AllocWS(uint16_t len);
void SWI_CutWSTR(WSHDR *wshdr, uint16_t len);
WSHDR *SWI_CreateLocalWS(WSHDR *wshdr, uint16_t *wsbody, uint16_t len);
WSHDR *SWI_CreateWS(void *(*ws_malloc)(size_t), void (*ws_mfree)(void *), uint16_t len);
void SWI_FreeWS(WSHDR *wshdr);
int SWI_str_2ws(WSHDR *ws, const char *str, uint16_t size);
int SWI_wstrcmp(WSHDR *ws1, WSHDR *ws2);
void SWI_wstrcpybypos(WSHDR *dest, WSHDR *src, uint16_t from, uint16_t len);
void SWI_wsRemoveChars(WSHDR *ws, uint16_t from, uint16_t to);
int SWI_ws_2utf8( WSHDR *from, char *to, int *result_length, uint16_t max_len);
int SWI_utf8_2ws(WSHDR *ws, const char *utf8_str, uint16_t maxLen);
uint16_t SWI_wstrchr(WSHDR *ws, uint16_t start_pos, uint16_t wchar);
uint16_t SWI_wstrrchr(WSHDR *ws, uint16_t max_pos, uint16_t wchar);
void SWI_wsAppendChar(WSHDR *ws, uint16_t wchar);
int SWI_wsInsertChar(WSHDR *ws, uint16_t wchar, uint16_t pos);
