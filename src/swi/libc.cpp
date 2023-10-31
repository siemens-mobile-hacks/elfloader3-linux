#include "../swi.h"

#include <stdarg.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

char *SWI_strpbrk(const char *s1, const char *s2) {
	return strpbrk(const_cast<char *>(s1), s2);
}

size_t SWI_strcspn(const char *s1, const char *s2) {
	return strcspn(s1, s2);
}

char *SWI_strncat(char *dest, const char *substr, size_t maxSubLen) {
	return strncat(dest, substr, maxSubLen);
}

int SWI_strncmp(const char *s1, const char *s2, size_t n) {
	return strncmp(s1, s2, n);
}

char *SWI_strncpy(char *dest, const char *source, size_t maxlen) {
	return strncpy(dest, source, maxlen);
}

char *SWI_strrchr(const char *s, int c) {
	return strrchr(const_cast<char *>(s), c);
}

char *SWI_strstr(const char *s1, const char *s2) {
	return strstr(const_cast<char *>(s1), s2);
}

long SWI_strtol(const char *nptr, char **endptr, int base) {
	return strtol(nptr, endptr, base);
}

unsigned long SWI_strtoul(const char *nptr, char **endptr, int base) {
	return strtoul(nptr, endptr, base);
}

int SWI_memcmp(const void *m1, const void *m2, size_t n) {
	return memcmp(m1, m2, n);
}

void SWI_zeromem(void *dest, int n) {
	memset(dest, 0, n);
}

void *SWI_memcpy(void *dest, const void *source,size_t cnt) {
	return memcpy(dest, source, cnt);
}

void *SWI_memmove(void *dest, const void *source, size_t cnt) {
	return memmove(dest, source, cnt);
}

int SWI_setjmp(_jmp_buf env) {
	fprintf(stderr, "SWI_setjmp wtf\n");
	abort();
	return -1;
}

int SWI_snprintf(char *buffer, int n, const char *format, ...) {
	va_list arg;
	int ret;
	va_start(arg, format);
	ret = vsnprintf(buffer, n, format, arg);
	va_end(arg);
	return ret;
}

void *SWI_memset(void *s, int c, size_t n) {
	return memset(s, c, n);
}

void *SWI_calloc(size_t nelem, size_t elsize) {
	return calloc(nelem, elsize);
}

int SWI_strcmpi(const char *s1, const char *s2) {
	return strcasecmp(s1, s2);
}

int SWI_StrToInt(char *s, char **endp) {
	return strtol(s, endp, 10);
}

void *SWI_malloc(size_t size) {
	return malloc(size);
}

void SWI_free(void *param1) {
	return free(param1);
}

int SWI_sprintf(char *buffer, const char *format, ...) {
	va_list arg;
	int ret;
	va_start(arg, format);
	ret = vsprintf(buffer, format, arg);
	va_end(arg);
	return ret;
}

char *SWI_strcat(char *param1, const char *param2) {
	return strcat(param1, param2);
}

char *SWI_strchr(const char *param1, int param2) {
	return strchr(const_cast<char *>(param1), param2);
}

int SWI_strcmp(const char *param1, const char *param2) {
	return strcmp(param1, param2);
}

char *SWI_strcpy(char *dest, const char *source) {
	return strcpy(dest, source);
}

uint32_t SWI_strlen(const char *param1) {
	return strlen(param1);
}
