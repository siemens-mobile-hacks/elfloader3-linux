#include "../swi.h"
#include "../charset.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cassert>
#include <codecvt>
#include <iconv.h>

int SWI_wstrcmp(WSHDR *ws1 ,WSHDR *ws2) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

void SWI_wstrcpybypos(WSHDR *dest, WSHDR *src, int from, int len) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

void SWI_wsRemoveChars(WSHDR *ws, int from, int to) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

short SWI_wstrchr(WSHDR *ws, uint32_t start_pos, uint32_t wchar) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

short SWI_wstrrchr(WSHDR *ws, uint32_t max_pos, uint32_t wchar) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

void SWI_wsAppendChar(WSHDR *ws, int wchar) {
	WSHDR wslocal = {};
	uint16_t wsbody[2];
	SWI_CreateLocalWS(&wslocal, wsbody, 2);
	wsbody[0] = wchar;
	SWI_wstrcat(ws, &wslocal);
}

int SWI_wsInsertChar(WSHDR *ws,int wchar,int pos) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

WSHDR *SWI_wstrcpy(WSHDR *ws1, WSHDR *ws2) {
	return SWI_wstrncpy(ws1, ws2, SWI_wstrlen(ws2));
}

WSHDR *SWI_wstrncpy(WSHDR *ws1, WSHDR *ws2, int n) {
	uint16_t *s = ws1->wsbody;
	uint16_t *s2 = ws2->wsbody;
	
	if (n >= ws2->maxlen)
		n = ws2->maxlen - 1;
	if (n >= ws1->maxlen)
		n = ws1->maxlen - 1;
	
	while (n > 0 && *s2 != 0) {
		*s++ = *s2++;
		--n;
	}
	while (n > 0) {
		*s++ = 0;
		--n;
	}
	return ws1;
}

WSHDR *SWI_wstrcat(WSHDR *s1, WSHDR *s2) {
	WSHDR wslocal = {};
	size_t len1 = SWI_wstrlen(s1);
	SWI_CreateLocalWS(&wslocal, &s1->wsbody[len1], s1->maxlen - len1);
	SWI_wstrcpy(&wslocal, s2);
	return s1;
}

WSHDR *SWI_wstrncat(WSHDR *s1, WSHDR *s2, int n) {
	size_t len1 = SWI_wstrlen(s1);
	size_t len2 = SWI_wstrlen(s2);
	if (len2 < n) {
		WSHDR wslocal = {};
		SWI_CreateLocalWS(&wslocal, &s1->wsbody[len1], s1->maxlen - len1);
		SWI_wstrcpy(&wslocal, s2);
	} else {
		WSHDR wslocal = {};
		SWI_CreateLocalWS(&wslocal, &s1->wsbody[len1], s1->maxlen - len1);
		SWI_wstrncpy(&s1[len1], s2, n);
		s1->wsbody[len1 + n] = 0;
	}
	return s1;
}

int SWI_wstrlen(WSHDR *ws) {
	uint16_t *ptr = ws->wsbody;
	int len = 0;
	while (*ptr) {
		*ptr++;
		len++;
	}
	return len;
}

int SWI_wsprintf(WSHDR * param1,const char *format,...) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

WSHDR *SWI_AllocWS(int len) {
	return SWI_CreateWS(SWI_malloc, SWI_free, len);
}

void SWI_CutWSTR(WSHDR *ws, int len) {
	if (len >= ws->maxlen)
		len = ws->maxlen - 1;
	ws->wsbody[len] = 0;
}

WSHDR *SWI_CreateLocalWS(WSHDR *ws, uint16_t *wsbody, int maxlen) {
	ws->ws_malloc = nullptr;
	ws->ws_mfree = nullptr;
	ws->isbody_allocated = 0;
	ws->maxlen = maxlen;
	ws->wsbody = wsbody;
	ws->unk1 = 0; // wtf
	memset(ws->wsbody, 0, ws->maxlen * sizeof(uint16_t));
	return ws;
}

WSHDR *SWI_CreateWS(void *(*ws_malloc)(size_t), void (*ws_mfree)(void *), int len) {
	WSHDR *ws = reinterpret_cast<WSHDR *>(ws_malloc(sizeof(WSHDR)));
	ws->ws_malloc = ws_malloc;
	ws->ws_mfree = ws_mfree;
	ws->isbody_allocated = 1;
	ws->maxlen = len + 1;
	ws->wsbody = reinterpret_cast<uint16_t *>(ws_malloc(ws->maxlen * sizeof(uint16_t)));
	ws->unk1 = 0; // wtf
	memset(ws->wsbody, 0, ws->maxlen * sizeof(uint16_t));
	return ws;
}

void SWI_FreeWS(WSHDR *ws) {
	if (ws) {
		auto ws_free = ws->ws_mfree;
		if (ws->isbody_allocated)
			ws_free(ws->wsbody);
		ws_free(ws);
	}
}

int SWI_str_2ws(WSHDR *ws, const char *buffer, uint32_t max_size) {
	if (max_size > ws->maxlen)
		max_size = ws->maxlen;
	return cp1251_to_utf16(buffer, ws->wsbody, max_size);
}

int SWI_ws_2utf8(WSHDR *from, char *buffer, int *result_length, uint32_t max_size) {
	std::vector<uint16_t> tmp;
	tmp.assign(from->wsbody, from->wsbody + SWI_wstrlen(from));
	size_t new_len = utf16_to_utf8(tmp, buffer, max_size);
	if (result_length)
		*result_length = new_len;
	return new_len;
}

int SWI_utf8_2ws(WSHDR *ws, const char *buffer, uint32_t max_size) {
	if (max_size > ws->maxlen)
		max_size = ws->maxlen;
	return utf8_to_utf16(buffer, ws->wsbody, max_size);
}
