#pragma once

#include <cstdint>
#include <cstddef>

// libc
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
unsigned int SWI_strlen(const char *param1);
