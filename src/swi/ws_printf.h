#pragma once

#include <cstdarg>
#include <cstddef>

int _ws_vsnprintf(char* buffer, size_t count, const char *format, va_list va);
