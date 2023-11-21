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
		
		static inline uint32_t getBitmapPixelRGB888(int x, int y, int w, int h, uint8_t *bitmap) {
			uint32_t *pixels = reinterpret_cast<uint32_t *>(bitmap);
			return pixels[y * w + x];
		}
		
		static inline uint32_t getBitmapPixelRGB565(int x, int y, int w, int h, uint8_t *bitmap) {
			uint16_t *pixels = reinterpret_cast<uint16_t *>(bitmap);
			uint32_t color = pixels[y * w + x];
			uint32_t b = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
			uint32_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
			uint32_t r = (((color & 0x1F) * 527) + 23) >> 6;
			return r << 16 | g << 8 | b;
		}
		
		static inline uint32_t getBitmapPixelRGB332(int x, int y, int w, int h, uint8_t *bitmap) {
			uint16_t *pixels = reinterpret_cast<uint16_t *>(bitmap);
			uint32_t color = pixels[y * w + x];
			uint32_t r = (((color >> 5) & 0x7) * 0xFF) / 0x7;
			uint32_t g = (((color >> 2) & 0x7) * 0xFF) / 0x7;
			uint32_t b = ((color & 0x3) * 0xFF) / 0x3;
			return r << 16 | g << 8 | b;
		}
		
		static inline uint32_t getBitmapPixelWB(int x, int y, int w, int h, uint8_t *pixels) {
			uint32_t index = y * w + x;
			uint32_t byte_index = (index + 7) / 8;
			uint32_t bit_index = 7 - (index % 8);
			return ((pixels[index] >> byte_index) & 1) ? 0xFFFFFFFF : 0xFF000000;
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
