#pragma once

#include <string>
#include <vector>
#include <cstdint>

// CP1251 -> UTF8
std::string cp1251_to_utf8(const std::string &input);
size_t cp1251_to_utf8(const std::string &input, char *buffer, size_t buffer_size);

// CP1251 -> UTF16
std::vector<uint16_t> cp1251_to_utf16(const std::string &input);
size_t cp1251_to_utf16(const std::string &input, uint16_t *buffer, size_t buffer_size);

// UTF16 -> UTF8
size_t utf16_to_utf8(const std::vector<uint16_t> &input, char *buffer, size_t buffer_size);
size_t utf8_to_utf16(const std::string &input, uint16_t *buffer, size_t buffer_size);
