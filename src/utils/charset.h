#pragma once

#include <cstddef>
#include <cstdint>

// CP1251 -> UTF8
size_t cp1251_to_utf8(const char *input, size_t input_len, char *buffer, size_t buffer_size);

// CP1251 -> UTF16
size_t cp1251_to_utf16(const char *input, size_t input_len, uint16_t *buffer, size_t buffer_size);

// UTF16 -> UTF8
size_t utf16_to_utf8(const uint16_t *input, size_t input_len, char *buffer, size_t buffer_size);

// UTF8 -> UTF16
size_t utf8_to_utf16(const char *input, size_t input_len, uint16_t *buffer, size_t buffer_size);

// CP1252 -> UTF8
size_t cp1252_to_utf8(const char *input, size_t input_len, char *buffer, size_t buffer_size);

// CP1252 -> UTF16
size_t cp1252_to_utf16(const char *input, size_t input_len, uint16_t *buffer, size_t buffer_size);
