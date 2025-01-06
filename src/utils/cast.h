#pragma once

#include <concepts>

template<std::signed_integral T>
constexpr auto toUnsigned(T const value) {
	return static_cast<std::make_unsigned_t<T>>(value);
}

template<std::unsigned_integral T>
constexpr auto toSigned(T const value) {
	return static_cast<std::make_signed_t<T>>(value);
}
