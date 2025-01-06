#pragma once

#include <cstdio>
#include <cstdint>

class Bitmap {
public:
	enum Type {
		TYPE_WB,
		TYPE_BGR233,
		TYPE_BGRA4444,
		TYPE_BGR565,
		TYPE_BGRA8888, // alpha 0-255
		TYPE_BGRA8888S, // alpha 0-100 (Siemens variant)
	};

	static inline uint32_t getBitmapPixel(Type type, int x, int y, int w, int h, const uint8_t *bitmap) {
		switch (type) {
			case TYPE_WB:				return getBitmapPixelWB(x, y, w, h, bitmap);
			case TYPE_BGR233:			return getBitmapPixelBGR233(x, y, w, h, bitmap);
			case TYPE_BGRA4444:			return getBitmapPixelBGRA4444(x, y, w, h, bitmap);
			case TYPE_BGR565:			return getBitmapPixelBGR565(x, y, w, h, bitmap);
			case TYPE_BGRA8888:			return getBitmapPixelBGRA8888(x, y, w, h, bitmap);
			case TYPE_BGRA8888S:		return getBitmapPixelBGRA8888S(x, y, w, h, bitmap);
		}
		return 0;
	}

	static inline void setBitmapPixel(Type type, int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		switch (type) {
			case TYPE_WB:				return setBitmapPixelWB(x, y, w, h, bitmap, color);
			case TYPE_BGR233:			return setBitmapPixelBGR233(x, y, w, h, bitmap, color);
			case TYPE_BGRA4444:			return setBitmapPixelBGRA4444(x, y, w, h, bitmap, color);
			case TYPE_BGR565:			return setBitmapPixelBGR565(x, y, w, h, bitmap, color);
			case TYPE_BGRA8888:			return setBitmapPixelBGRA8888(x, y, w, h, bitmap, color);
			case TYPE_BGRA8888S:		return setBitmapPixelBGRA8888S(x, y, w, h, bitmap, color);
		}
	}

	/**
	 * WB
	 * */
	static inline uint32_t WBtoBGRA8888(uint8_t color) {
		return color ? 0xFFFFFFFF : 0xFF000000;
	}

	static inline uint8_t BGRA8888toWB(uint32_t color) {
		uint8_t r = (color >> 24) & 0xFF;
		uint8_t g = (color >> 16) & 0xFF;
		uint8_t b = (color >> 8) & 0xFF;
		double gray = 0.299 * r + 0.587 * g + 0.114 * b;
		return (gray >= 128.0) ? 1 : 0;
	}

	static inline uint32_t getBitmapPixelWB(int x, int y, int w, int h, const uint8_t *pixels) {
		uint32_t row = (w + 7) / 8;
		uint32_t current_bit = y * (row * 8) + x;
		uint32_t current_byte = current_bit / 8;
		uint32_t current_shift = 7 - (current_bit - current_byte * 8);
		return WBtoBGRA8888((pixels[current_byte] >> current_shift) & 1);
	}

	static inline void setBitmapPixelWB(int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		uint32_t row = (w + 7) / 8;
		uint32_t current_bit = y * (row * 8) + x;
		uint32_t current_byte = current_bit / 8;
		uint32_t current_shift = 7 - (current_bit - current_byte * 8);
		if (BGRA8888toWB(color)) {
			bitmap[current_byte] |= (1 << current_shift);
		} else {
			bitmap[current_byte] &= ~(1 << current_shift);
		}
	}

	/**
	 * BGRA4444
	 * */
	static inline uint32_t BGRA4444toBGRA8888(uint16_t color) {
		uint32_t a = ((color >> 12) & 0xF) * 0xFF / 0xF;
		uint32_t r = ((color >> 8) & 0xF) * 0xFF / 0xF;
		uint32_t g = ((color >> 4) & 0xF) * 0xFF / 0xF;
		uint32_t b = (color & 0xF) * 0xFF / 0xF;
		return (a << 24) | (r << 16) | (g << 8) | b;
	}

	static inline uint16_t BGRA8888toBGRA4444(uint32_t color) {
		uint8_t a = ((color >> 24) & 0xFF) >> 4;
		uint8_t r = ((color >> 16) & 0xFF) >> 4;
		uint8_t g = ((color >> 8) & 0xFF) >> 4;
		uint8_t b = (color & 0xFF) >> 4;
		return (a << 12) | (r << 8) | (g << 4) | b;
	}

	static inline uint32_t getBitmapPixelBGRA4444(int x, int y, int w, int h, const uint8_t *bitmap) {
		const uint16_t *pixels = reinterpret_cast<const uint16_t *>(bitmap);
		return BGRA4444toBGRA8888(pixels[y * w + x]);
	}

	static inline void setBitmapPixelBGRA4444(int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		uint16_t *pixels = reinterpret_cast<uint16_t *>(bitmap);
		pixels[y * w + x] = BGRA8888toBGRA4444(color);
	}

	/**
	 * BGR565
	 * */
	static inline uint32_t BGR565toBGRA8888(uint16_t color) {
		uint32_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
		uint32_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
		uint32_t b = (((color & 0x1F) * 527) + 23) >> 6;
		return 0xFF000000 | (r << 16) | (g << 8) | b;
	}

	static inline uint16_t BGRA8888toBGR565(uint32_t color) {
		uint32_t r = (color >> 16) & 0xFF;
		uint32_t g = (color >> 8) & 0xFF;
		uint32_t b = color & 0xFF;
		return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
	}

	static inline uint32_t getBitmapPixelBGR565(int x, int y, int w, int h, const uint8_t *bitmap) {
		const uint16_t *pixels = reinterpret_cast<const uint16_t *>(bitmap);
		return BGR565toBGRA8888(pixels[y * w + x]);
	}

	static inline void setBitmapPixelBGR565(int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		uint16_t *pixels = reinterpret_cast<uint16_t *>(bitmap);
		pixels[y * w + x] = BGRA8888toBGR565(color);
	}

	/**
	 * BGRA8888 (alpha 0-255)
	 * */
	static inline uint32_t getBitmapPixelBGRA8888(int x, int y, int w, int h, const uint8_t *bitmap) {
		const uint32_t *pixels = reinterpret_cast<const uint32_t *>(bitmap);
		return pixels[y * w + x];
	}

	static inline void setBitmapPixelBGRA8888(int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		uint32_t *pixels = reinterpret_cast<uint32_t *>(bitmap);
		pixels[y * w + x] = color;
	}

	/**
	 * BGRA8888S (alpha 0-100)
	 * */
	static inline uint32_t BGRA8888StoBGRA8888(uint8_t color) {
		uint32_t new_alpha = ((color >> 24) & 0xFF) * 100 / 0xFF;
		return (color & ~0xFF000000) | (new_alpha << 24);
	}

	static inline uint8_t BGRA8888toBGRA8888S(uint32_t color) {
		uint32_t new_alpha = ((color >> 24) & 0xFF) * 0xFF / 100;
		return (color & ~0xFF000000) | (new_alpha << 24);
	}

	static inline uint32_t getBitmapPixelBGRA8888S(int x, int y, int w, int h, const uint8_t *bitmap) {
		const uint32_t *pixels = reinterpret_cast<const uint32_t *>(bitmap);
		return BGRA8888StoBGRA8888(pixels[y * w + x]);
	}

	static inline void setBitmapPixelBGRA8888S(int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		uint32_t *pixels = reinterpret_cast<uint32_t *>(bitmap);
		pixels[y * w + x] = BGRA8888toBGRA8888S(color);
	}

	/**
	 * BGR233
	 * */
	static inline uint32_t BGR233toBGRA8888(uint8_t color) {
		uint8_t r = ((color >> 5) & 0x7) * 0xFF / 0x7;
		uint8_t g = ((color >> 2) & 0x7) * 0xFF / 0x7;
		uint8_t b = (color & 0x3) * 0xFF / 0x3;
		return 0xFF000000 | (r << 16) | (g << 8) | b;
	}

	static inline uint8_t BGRA8888toBGR233(uint32_t color) {
		uint8_t r = ((color >> 16) & 0xFF) >> 5;
		uint8_t g = ((color >> 8) & 0xFF) >> 5;
		uint8_t b = (color & 0xFF) >> 6;
		return (r << 5) | (g << 2) | b;
	}

	static inline uint32_t getBitmapPixelBGR233(int x, int y, int w, int h, const uint8_t *bitmap) {
		const uint8_t *pixels = reinterpret_cast<const uint8_t *>(bitmap);
		return BGR233toBGRA8888(pixels[y * w + x]);
	}

	static inline void setBitmapPixelBGR233(int x, int y, int w, int h, uint8_t *bitmap, uint32_t color) {
		uint8_t *pixels = reinterpret_cast<uint8_t *>(bitmap);
		pixels[y * w + x] = BGRA8888toBGR233(color);
	}
};
