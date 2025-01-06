#pragma once

#include <string>
#include <swilib/image.h>
#include "src/gui/Bitmap.h"

IMGHDR *IMG_LoadAny(const std::string &path, bool only_rgb888 = false);
Bitmap::Type IMG_GetBitmapType(int bpnum);
