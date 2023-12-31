#pragma once

#include <cstdio>
#include <cstdint>

class Bitmap {
	public:
		enum Type {
			TYPE_WB		= 0,
			TYPE_RGB332	= 1,
			TYPE_RGB565	= 2,
			TYPE_RGB8888	= 3
		};
		
		static inline uint32_t RGB565toRGB8888(uint32_t color) {
			uint32_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
			uint32_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
			uint32_t b = (((color & 0x1F) * 527) + 23) >> 6;
			return 0xFF000000 | (r << 16) | (g << 8) | b;
		}
		
		static inline uint32_t RGB8888toRGB565(uint32_t color) {
			uint32_t r = (color >> 16) & 0xFF;
			uint32_t g = (color >> 8) & 0xFF;
			uint32_t b = color & 0xFF;
			return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
		}
		
		static inline uint32_t getBitmapPixelRGB888(int x, int y, int w, int h, uint8_t *bitmap) {
			uint32_t *pixels = reinterpret_cast<uint32_t *>(bitmap);
			return pixels[y * w + x];
		}
		
		static inline uint32_t getBitmapPixelRGB565(int x, int y, int w, int h, uint8_t *bitmap) {
			uint16_t *pixels = reinterpret_cast<uint16_t *>(bitmap);
			uint32_t color = pixels[y * w + x];
			uint32_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
			uint32_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
			uint32_t b = (((color & 0x1F) * 527) + 23) >> 6;
			return 0xFF000000 | (r << 16) | (g << 8) | b;
		}
		
		static inline uint32_t getBitmapPixelRGB332(int x, int y, int w, int h, uint8_t *bitmap) {
			uint16_t *pixels = reinterpret_cast<uint16_t *>(bitmap);
			uint32_t color = pixels[y * w + x];
			return RGB565toRGB8888(pixels[y * w + x]);
		}
		
		static inline uint32_t getBitmapPixelWB(int x, int y, int w, int h, uint8_t *pixels) {
			uint32_t row = (w + 7) / 8;
			uint32_t current_bit = y * (row * 8) + x;
			uint32_t current_byte = current_bit / 8;
			uint32_t current_shift = 7 - (current_bit - current_byte * 8);
			return ((pixels[current_byte] >> current_shift) & 1) ? 0xFFFFFFFF : 0xFF000000;
		}
		
		static inline uint32_t getBitmapPixel(Type type, int x, int y, int w, int h, uint8_t *bitmap) {
			switch (type) {
				case TYPE_WB:		return getBitmapPixelWB(x, y, w, h, bitmap);
				case TYPE_RGB332:	return getBitmapPixelRGB332(x, y, w, h, bitmap);
				case TYPE_RGB565:	return getBitmapPixelRGB565(x, y, w, h, bitmap);
				case TYPE_RGB8888:	return getBitmapPixelRGB888(x, y, w, h, bitmap);
			}
			return 0;
		}
};
