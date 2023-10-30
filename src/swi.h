#pragma once

#include <cstdint>
#include <cstddef>

#define NEWSGOLD 1

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
void *SWI_malloc_adr(void);
void *SWI_mfree_adr(void);
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

#pragma pack(1)
typedef struct {
#ifdef NEWSGOLD
  char unk0[40];
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
  char unk1[14];
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
