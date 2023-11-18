#include "swi.h"
#include "charset.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cassert>
#include <codecvt>
#include <iconv.h>

#include "ws_printf.h"

WSHDR *AllocWS(uint16_t len) {
	return CreateWS(malloc, free, len);
}

WSHDR *CreateLocalWS(WSHDR *ws, uint16_t *wsbody, uint16_t maxlen) {
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

WSHDR *CreateWS(void *(*ws_malloc)(size_t), void (*ws_mfree)(void *), uint16_t len) {
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

void FreeWS(WSHDR *ws) {
	assert(ws != nullptr && ws->ws_mfree != nullptr);
	
	auto ws_free = ws->ws_mfree;
	if (ws->isbody_allocated)
		ws_free(ws->wsbody);
	ws_free(ws);
}

int wstrlen(WSHDR *ws) {
	assert(ws != nullptr);
	return ws->wsbody[0];
}

int wstrcmp(WSHDR *ws1, WSHDR *ws2) {
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

uint16_t wstrchr(WSHDR *ws, uint16_t start_pos, uint16_t ch) {
	assert(ws != nullptr);
	
	for (size_t i = start_pos; i < ws->body->len; i++) {
		if (ws->body->data[i] == ch)
			return i;
	}
	return 0xFFFF;
}

uint16_t wstrrchr(WSHDR *ws, uint16_t max_pos, uint16_t ch) {
	assert(ws != nullptr && max_pos <= ws->body->len - 1);
	
	for (int i = max_pos; i >= 0; i--) {
		if (ws->body->data[i] == ch)
			return i;
	}
	return 0xFFFF;
}

void wsAppendChar(WSHDR *ws, uint16_t ch) {
	assert(ws != nullptr);
	assert(ws->body->len < ws->maxlen - 1);
	
	ws->body->data[ws->body->len++] = ch;
}

int wsInsertChar(WSHDR *ws, uint16_t ch, uint16_t pos) {
	assert(ws != nullptr && pos > 0);
	pos = pos - 1;
	
	assert(pos < ws->maxlen - 1);
	assert(ws->body->len + 1 <= ws->maxlen - 1);
	
	memmove(&ws->body->data[pos + 1], &ws->body->data[pos], (ws->maxlen - 1 - pos) * sizeof(uint16_t));
	ws->body->data[pos] = ch;
	ws->body->len++;
	
	return ws->body->len;
}

void wsRemoveChars(WSHDR *ws, uint16_t pos, uint16_t len) {
	assert(ws != nullptr && pos > 0);
	pos = pos - 1;
	
	assert(pos < ws->maxlen - 1);
	assert(pos + len <= ws->maxlen - 1);
	
	memmove(&ws->body->data[pos], &ws->body->data[pos + len], (ws->maxlen - 1 - (pos + len)) * sizeof(uint16_t));
	ws->body->len -= len;
}

WSHDR *wstrcpy(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	return wstrncpy(ws1, ws2, ws2->body->len);
}

WSHDR *wstrncpy(WSHDR *ws1, WSHDR *ws2, uint16_t n) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t copy_n = std::min(n, ws2->body->len);
	assert(ws1->maxlen - 1 >= copy_n);
	
	ws1->body->len = copy_n;
	memcpy(ws1->body->data, ws2->body->data, copy_n * sizeof(uint16_t));
	return ws1;
}

void wstrcpybypos(WSHDR *ws1, WSHDR *ws2, uint16_t from, uint16_t n) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t copy_n = std::min(n, ws2->body->len);
	assert(from < ws1->body->len);
	assert(ws1->maxlen - 1 - from >= copy_n);
	
	ws1->body->len = copy_n;
	memcpy(&ws1->body->data[from], ws2->body->data, copy_n * sizeof(uint16_t));
}

WSHDR *wstrcat(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	return wstrncat(ws1, ws2, ws2->body->len);
}

WSHDR *wstrncat(WSHDR *ws1, WSHDR *ws2, uint16_t n) {
	size_t copy_n = std::min(n, ws2->body->len);
	
	assert(ws1 != nullptr && ws2 != nullptr);
	assert(ws1->maxlen - ws1->body->len - 1 >= copy_n);
	
	for (size_t i = 0; i < copy_n; i++)
		ws1->body->data[ws1->body->len++] = ws2->body->data[i];
	return ws1;
}

void CutWSTR(WSHDR *ws, uint16_t len) {
	assert(ws != nullptr && len <= ws->body->len);
	ws->body->len = len;
}

int wsprintf(WSHDR *ws, const char *format,...) {
	assert(ws != nullptr && format != nullptr);
	
	size_t ws_maxlen = ws->maxlen - 1;
	if (!ws_maxlen)
		return 0;
	
	// Convert format from CP1252 to UTF8
	size_t format_len = strlen(format);
	char *format_utf8 = new char[format_len * 4 + 1];
	
	size_t format_utf8_len = cp1252_to_utf8(format, format_len, format_utf8, format_len * 4);
	format_utf8[format_utf8_len] = 0;
	
	// Temporary UTF-8 buffer for result
	char *buffer_utf8 = new char[ws_maxlen * 4 + 1];
	
	// Do format
	va_list arg;
	va_start(arg, format);
	size_t buffer_len_utf8 = _ws_vsnprintf(buffer_utf8, ws_maxlen * 4 + 1, format_utf8, arg);
	va_end(arg);
	
	// Convert result from UTF-8 to UTF-16
	ws->body->len = utf8_to_utf16(buffer_utf8, buffer_len_utf8, ws->body->data, ws_maxlen);
	
	// Clean temp buffers
	delete[] format_utf8;
	delete[] buffer_utf8;
	
	return ws->body->len;
}

void wstrcatprintf(WSHDR *ws, const char *format,...) {
	assert(ws != nullptr && format != nullptr);
	
	size_t ws_maxlen = ws->maxlen - 1 - ws->body->len;
	if (!ws_maxlen)
		return;
	
	// Convert format from CP1252 to UTF8
	size_t format_len = strlen(format);
	char *format_utf8 = new char[format_len * 4 + 1];
	
	size_t format_utf8_len = cp1252_to_utf8(format, format_len, format_utf8, format_len * 4);
	format_utf8[format_utf8_len] = 0;
	
	// Temporary UTF-8 buffer for result
	char *buffer_utf8 = new char[ws_maxlen * 4 + 1];
	
	// Do format
	va_list arg;
	va_start(arg, format);
	size_t buffer_len_utf8 = _ws_vsnprintf(buffer_utf8, ws_maxlen * 4 + 1, format_utf8, arg);
	va_end(arg);
	
	// Convert result from UTF-8 to UTF-16
	ws->body->len += utf8_to_utf16(buffer_utf8, buffer_len_utf8, &ws->body->data[ws->body->len], ws_maxlen);
	
	// Clean temp buffers
	delete[] format_utf8;
	delete[] buffer_utf8;
}

int str_2ws(WSHDR *ws, const char *buffer, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && ws->maxlen - 1 >= max_size);
	ws->body->len = cp1252_to_utf16(buffer, strlen(buffer), ws->body->data, max_size);
	return ws->body->len;
}

void ws_2str(WSHDR *ws, char *buffer, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && max_size > 0);
	size_t new_len = utf16_to_utf8(ws->body->data, ws->body->len, buffer, max_size);
	buffer[new_len] = 0;
}

int ws_2utf8(WSHDR *ws, char *buffer, int *result_length, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && max_size > 0);
	
	size_t new_len = utf16_to_utf8(ws->body->data, ws->body->len, buffer, max_size);
	buffer[new_len] = 0;
	
	if (result_length)
		*result_length = new_len;
	return new_len;
}

int utf8_2ws(WSHDR *ws, const char *buffer, uint16_t max_size) {
	assert(ws != nullptr && buffer != nullptr && ws->maxlen - 1 >= max_size);
	ws->body->len = utf8_to_utf16(buffer, strlen(buffer), ws->body->data, max_size);
	return ws->body->len;
}

std::string ws2string(WSHDR *ws) {
	std::string result;
	result.resize(ws->body->len * 4);
	int new_len = ws_2utf8(ws, &result[0], nullptr, result.size() + 1);
	result.resize(new_len);
	return result;
}
