#include <cstdint>
#include <swilib/wstring.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cassert>
#include <iconv.h>

#include "src/swi/wstring.h"
#include "src/swi/ws_printf.h"
#include "src/utils/charset.h"

WSHDR *AllocWS(int len) {
	return CreateWS(malloc, free, len);
}

WSHDR *CreateLocalWS(WSHDR *ws, uint16_t *wsbody, int maxlen) {
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

WSHDR *CreateWS(void *(*ws_malloc)(size_t), void (*ws_mfree)(void *), int len) {
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

uint16_t wstrlen(const WSHDR *ws) {
	assert(ws != nullptr);
	return ws->wsbody[0];
}

uint16_t wsCharAt(const WSHDR *str, short pos) {
	assert(str != nullptr);
	assert(pos >= 0 && pos < wstrlen(str));
	return wsbody(str)->data[pos];
}

uint16_t *wstrcopy(uint16_t *destination, const uint16_t *source) {
	assert(source != nullptr);
	assert(destination != nullptr);

	uint16_t length = *source;
	if (source < destination) {
		uint16_t *destination_cursor = destination + length + 1;
		const uint16_t *source_cursor = source + (destination_cursor - destination);
		while (source_cursor != source) {
			source_cursor--;
			destination_cursor--;
			*destination_cursor = *source_cursor;
		}
	} else {
		uint16_t *destination_cursor = destination;
		uint16_t *destination_end = destination + length + 1;
		while (destination_cursor != destination_end) {
			*destination_cursor = *source;
			destination_cursor++;
			source++;
		}
	}
	return destination;
}

int wstrncmp(const WSHDR *ws1, const WSHDR *ws2, size_t n) {
	assert(ws1 != nullptr && ws2 != nullptr);

	size_t len1 = wstrlen(ws1);
	size_t len2 = wstrlen(ws2);

	size_t len = std::min({len1, len2, n});
	for (uint32_t i = 0; i < len; i++) {
		if (wsbody(ws1)->data[i] != wsbody(ws2)->data[i])
			return wsbody(ws1)->data[i] - wsbody(ws2)->data[i];
	}

	if (len == n || len1 == len2)
		return 0;

	return len1 < len2 ? -1 : 1;
}

int wstrcmp(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t len = std::min(wstrlen(ws1), wstrlen(ws2));
	for (uint32_t i = 0; i < len; i++) {
		if (wsbody(ws1)->data[i] != wsbody(ws2)->data[i])
			return wsbody(ws1)->data[i] - wsbody(ws2)->data[i];
	}
	
	if (wstrlen(ws1) == wstrlen(ws2))
		return 0;
	return wstrlen(ws1) > wstrlen(ws2) ? -1 : 1;
}

int16_t wstrchr(WSHDR *ws, uint32_t start_pos, uint32_t ch) {
	assert(ws != nullptr);
	
	for (int i = start_pos; i < wstrlen(ws); i++) {
		if (wsbody(ws)->data[i] == ch)
			return i;
	}
	return -1;
}

int16_t wstrrchr(WSHDR *ws, uint32_t max_pos, uint32_t ch) {
	assert(ws != nullptr && max_pos <= wstrlen(ws) - 1u);
	
	for (int i = max_pos; i >= 0; i--) {
		if (wsbody(ws)->data[i] == ch)
			return i;
	}
	return 0xFFFF;
}

void wsAppendChar(WSHDR *ws, int ch) {
	assert(ws != nullptr);
	assert(wstrlen(ws) < ws->maxlen - 1);
	auto body = wsbody(ws);
	body->data[body->len++] = ch;
}

int wsInsertChar(WSHDR *ws, int ch, int pos) {
	assert(ws != nullptr && pos > 0);
	pos = pos - 1;
	
	assert(pos < ws->maxlen - 1);
	assert(wstrlen(ws) + 1 <= ws->maxlen - 1);
	
	memmove(&wsbody(ws)->data[pos + 1], &wsbody(ws)->data[pos], (ws->maxlen - 1 - pos) * sizeof(uint16_t));

	auto body = wsbody(ws);
	body->data[pos] = ch;
	body->len++;
	
	return body->len;
}

void wsRemoveChars(WSHDR *ws, int pos, int len) {
	assert(ws != nullptr && pos > 0);
	pos = pos - 1;
	
	assert(pos < ws->maxlen - 1);
	// assert(pos + len <= ws->maxlen - 1);
	
	if (pos + len > ws->maxlen - 1)
		len = ws->maxlen - 1 - pos;
	
	memmove(&wsbody(ws)->data[pos], &wsbody(ws)->data[pos + len], (ws->maxlen - 1 - (pos + len)) * sizeof(uint16_t));
	wsbody(ws)->len -= len;
}

WSHDR *wstrcpy(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	return wstrncpy(ws1, ws2, wstrlen(ws2));
}

WSHDR *wstrncpy(WSHDR *ws1, WSHDR *ws2, int n) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t copy_n = std::min(n, (int) wstrlen(ws2));
	assert(ws1->maxlen - 1u >= copy_n);
	
	wsbody(ws1)->len = copy_n;
	memcpy(wsbody(ws1)->data, wsbody(ws2)->data, copy_n * sizeof(uint16_t));
	return ws1;
}

void wstrcpybypos(WSHDR *ws1, WSHDR *ws2, int from, int n) {
	assert(ws1 != nullptr && ws2 != nullptr);
	
	size_t copy_n = std::min(n, (int) wstrlen(ws2));
	assert(from < wstrlen(ws1));
	assert(ws1->maxlen - 1u - from >= copy_n);
	
	wsbody(ws1)->len = copy_n;
	memcpy(&wsbody(ws1)->data[from], wsbody(ws2)->data, copy_n * sizeof(uint16_t));
}

WSHDR *wstrcat(WSHDR *ws1, WSHDR *ws2) {
	assert(ws1 != nullptr && ws2 != nullptr);
	return wstrncat(ws1, ws2, wstrlen(ws2));
}

WSHDR *wstrncat(WSHDR *ws1, WSHDR *ws2, int n) {
	size_t copy_n = std::min(n, (int) wstrlen(ws2));
	
	assert(ws1 != nullptr && ws2 != nullptr);
	assert(ws1->maxlen - wstrlen(ws1) - 1u >= copy_n);
	
	for (size_t i = 0; i < copy_n; i++)
		wsbody(ws1)->data[wsbody(ws1)->len++] = wsbody(ws2)->data[i];
	return ws1;
}

void CutWSTR(WSHDR *ws, int len) {
	assert(ws != nullptr && len <= wstrlen(ws));
	wsbody(ws)->len = len;
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
	wsbody(ws)->len = utf8_to_utf16(buffer_utf8, buffer_len_utf8, wsbody(ws)->data, ws_maxlen);
	
	// Clean temp buffers
	delete[] format_utf8;
	delete[] buffer_utf8;
	
	return wstrlen(ws);
}

void wstrcatprintf(WSHDR *ws, const char *format,...) {
	assert(ws != nullptr && format != nullptr);
	
	size_t ws_maxlen = ws->maxlen - 1 - wstrlen(ws);
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
	wsbody(ws)->len += utf8_to_utf16(buffer_utf8, buffer_len_utf8, &wsbody(ws)->data[wstrlen(ws)], ws_maxlen);
	
	// Clean temp buffers
	delete[] format_utf8;
	delete[] buffer_utf8;
}

int str_2ws(WSHDR *ws, const char *buffer, uint32_t max_size) {
	assert(ws != nullptr && buffer != nullptr && ws->maxlen - 1u >= max_size);
	wsbody(ws)->len = cp1252_to_utf16(buffer, strlen(buffer), wsbody(ws)->data, max_size);
	return wstrlen(ws);
}

void ws_2str(const WSHDR *ws, char *buffer, uint32_t max_size) {
	assert(ws != nullptr && buffer != nullptr && max_size > 0);
	size_t new_len = utf16_to_utf8(wsbody(ws)->data, wstrlen(ws), buffer, max_size);
	buffer[new_len] = 0;
}

int ws_2utf8(const WSHDR *ws, char *buffer, int *result_length, uint32_t max_size) {
	assert(ws != nullptr && buffer != nullptr && max_size > 0);
	
	size_t new_len = utf16_to_utf8(wsbody(ws)->data, wstrlen(ws), buffer, max_size);
	buffer[new_len] = 0;
	
	if (result_length)
		*result_length = new_len;
	return new_len;
}

int utf8_2ws(WSHDR *ws, const char *buffer, uint32_t max_size) {
	assert(ws != nullptr && buffer != nullptr && ws->maxlen - 1u >= max_size);
	wsbody(ws)->len = utf8_to_utf16(buffer, strlen(buffer), wsbody(ws)->data, max_size);
	return wstrlen(ws);
}

std::string ws2string(const WSHDR *ws) {
	std::string result;
	result.resize(wstrlen(ws) * 4);
	int new_len = ws_2utf8(ws, &result[0], nullptr, result.size() + 1);
	result.resize(new_len);
	return result;
}
