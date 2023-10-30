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

WSHDR *SWI_AllocWS(uint16_t len) {
	return SWI_CreateWS(SWI_malloc, SWI_free, len);
}

WSHDR *SWI_CreateLocalWS(WSHDR *ws, uint16_t *wsbody, uint16_t maxlen) {
	assert(ws != nullptr && wsbody != nullptr && maxlen >= 1);
	
	ws->ws_malloc = nullptr;
	ws->ws_mfree = nullptr;
	ws->isbody_allocated = 0;
	ws->maxlen = maxlen;
	ws->wsbody = wsbody;
	ws->unk1 = 0; // wtf
	memset(ws->wsbody, 0, ws->maxlen * sizeof(uint16_t));
	return ws;
}

WSHDR *SWI_CreateWS(void *(*ws_malloc)(size_t), void (*ws_mfree)(void *), uint16_t len) {
	assert(ws_malloc != nullptr && ws_mfree != nullptr);
	
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
	assert(ws != nullptr && ws->ws_mfree != nullptr);
	
	auto ws_free = ws->ws_mfree;
	if (ws->isbody_allocated)
		ws_free(ws->wsbody);
	ws_free(ws);
}

int SWI_wstrlen(WSHDR *ws) {
	assert(ws != nullptr);
	return ws->wsbody[0];
}

int SWI_wstrcmp(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t len = std::min(ws1->body->len, ws2->body->len);
	for (uint32_t i = 0; i < len; i++) {
		if (ws1->body->data[i] != ws2->body->data[i])
			return ws1->body->data[i] - ws2->body->data[i];
	}
	
	if (ws1->body->len == ws2->body->len)
		return 0;
	return ws1->body->len > ws2->body->len ? -1 : 1;
}

void SWI_wsRemoveChars(WSHDR *ws, uint16_t from, uint16_t to) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
}

uint16_t SWI_wstrchr(WSHDR *ws, uint16_t start_pos, uint16_t ch) {
	assert(ws != nullptr);
	
	for (size_t i = start_pos; i < ws->body->len; i++) {
		if (ws->body->data[i] == ch)
			return i;
	}
	return 0xFFFF;
}

uint16_t SWI_wstrrchr(WSHDR *ws, uint16_t max_pos, uint16_t ch) {
	assert(ws != nullptr && max_pos <= ws->body->len - 1);
	
	for (int i = max_pos; i >= 0; i--) {
		if (ws->body->data[i] == ch)
			return i;
	}
	return 0xFFFF;
}

void SWI_wsAppendChar(WSHDR *ws, uint16_t ch) {
	assert(ws != nullptr);
	assert(ws->body->len < ws->maxlen - 1);
	
	ws->body->data[ws->body->len++] = ch;
}

int SWI_wsInsertChar(WSHDR *ws, uint16_t ch, uint16_t pos) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

WSHDR *SWI_wstrcpy(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	return SWI_wstrncpy(ws1, ws2, ws2->body->len);
}

WSHDR *SWI_wstrncpy(WSHDR *ws1, WSHDR *ws2, uint16_t n) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t copy_n = std::min(n, ws2->body->len);
	assert(ws1->maxlen - 1 >= copy_n);
	
	ws1->body->len = copy_n;
	memcpy(ws1->body->data, ws2->body->data, copy_n * sizeof(uint16_t));
	return ws1;
}

void SWI_wstrcpybypos(WSHDR *ws1, WSHDR *ws2, uint16_t from, uint16_t n) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t copy_n = std::min(n, ws2->body->len);
	assert(from < ws1->body->len);
	assert(ws1->maxlen - 1 - from >= copy_n);
	
	ws1->body->len = copy_n;
	memcpy(&ws1->body->data[from], ws2->body->data, copy_n * sizeof(uint16_t));
}

WSHDR *SWI_wstrcat(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	return SWI_wstrncat(ws1, ws2, ws2->body->len);
}

WSHDR *SWI_wstrncat(WSHDR *ws1, WSHDR *ws2, uint16_t n) {
	size_t copy_n = std::min(n, ws2->body->len);
	
	assert(ws1 != nullptr && ws2 != nullptr);
	assert(ws1->maxlen - ws1->body->len - 1 >= copy_n);
	
	for (size_t i = 0; i < copy_n; i++)
		ws1->body->data[ws1->body->len++] = ws2->body->data[i];
	return ws1;
}

void SWI_CutWSTR(WSHDR *ws, uint16_t len) {
	assert(ws != nullptr && len <= ws->body->len);
	ws->body->len = len;
}

int SWI_wsprintf(WSHDR *param1, const char *format,...) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_str_2ws(WSHDR *ws, const char *buffer, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && ws->maxlen - 1 >= max_size);
	ws->body->len = cp1251_to_utf16(buffer, strlen(buffer), ws->body->data, max_size);
	return ws->body->len;
}

int SWI_ws_2utf8(WSHDR *ws, char *buffer, int *result_length, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && max_size > 0);
	
	size_t new_len = utf16_to_utf8(ws->body->data, ws->body->len, buffer, max_size - 1);
	buffer[new_len] = 0;
	
	if (result_length)
		*result_length = new_len;
	return new_len;
}

int SWI_utf8_2ws(WSHDR *ws, const char *buffer, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && ws->maxlen - 1 >= max_size);
	ws->body->len = utf8_to_utf16(buffer, strlen(buffer), ws->body->data, max_size);
	return ws->body->len;
}
