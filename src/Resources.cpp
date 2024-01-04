#include "Resources.h"

#include <string>
#include <cassert>
#include <regex>
#include <filesystem>
#include <iostream>

#include "SieFs.h"
#include "utils.h"

static const int E1XX_PICS[] = {
#if defined(ELKA)
	1357, 1355, 1354, 1349, 1351, 1358, 335, 321, 332, 312, 313, 310, 311, 305,
	319, 320, 321, 323, 331, 333, 334, 1, 1338, 1340, 50, 43, 49, 42, 46, 1, 55,
	40, 47, 53, 36, 35, 39, 37, 38, 1344, 34, 184, 1365, 1366, 1367, 1368, 1361,
	1362, 1363, 1364, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378,
	1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391,
	1392, 1405, 1406, 1407, 1408, 1397, 1398, 1399, 1400, 1393, 1394, 1395, 1396,
	1401, 1402, 1403, 1404, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417,
	1418, 1419, 1420, 1, 1, 1, 1, 867, 1, 1, 426, 1, 302, 1, 184, 1, 1315, 599,
	449, 487, 487, 1247, 1255, 1, 1241, 1246, 1257, 1250, 1249, 1242, 1243, 1245,
	1248, 1251, 1252, 1253, 1254, 1256, 1455, 1, 1454, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1448, 1, 1, 1105, 1102, 1, 1, 1, 1, 1, 1, 277, 1, 1, 150, 150, 149,
	149, 77, 78, 83, 79, 81, 82, 80, 84, 74, 75, 86, 87, 88, 85, 1480, 1481, 1482,
	1483, 1484, 1485, 449, 599, 227, 223, 222, 187, 214, 228, 212, 213, 1163, 1489,
	419, 1466, 1465, 44, 45, 1172
#elif defined(NEWSGOLD)
	1401, 1399, 1398, 1393, 1395, 1402, 374, 360, 371, 351, 352, 349, 350, 344,
	358, 359, 360, 362, 370, 372, 373, 1, 1382, 1384, 68, 61, 67, 60, 64, 1, 73,
	58, 65, 71, 54, 53, 57, 55, 56, 1388, 52, 204, 1409, 1410, 1411, 1412, 1405,
	1406, 1407, 1408, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422,
	1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435,
	1436, 1449, 1450, 1451, 1452, 1441, 1442, 1443, 1444, 1437, 1438, 1439, 1440,
	1445, 1446, 1447, 1448, 1453, 1454, 1455, 1456, 1457, 1458, 1459, 1460, 1461,
	1462, 1463, 1464, 1, 1, 1, 1, 837, 1, 1, 475, 1, 342, 1, 204, 1, 1355, 590,
	498, 534, 534, 1289, 1297, 1, 1283, 1288, 1299, 1292, 1291, 1284, 1285, 1287,
	1290, 1293, 1294, 1295, 1296, 1298, 1499, 1, 1498, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1492, 1, 1, 1135, 1132, 1, 1, 1, 1, 1, 1, 317, 1, 1, 170, 170, 169,
	169, 97, 98, 103, 99, 101, 102, 100, 104, 94, 95, 106, 107, 108, 105, 1524, 1525,
	1526, 1527, 1528, 1529, 498, 590, 265, 259, 258, 207, 246, 266, 244, 245, 1205,
	1533, 468, 1510, 1509, 62, 63, 1214
#endif
};

static Resources *m_instance = nullptr;

Resources::Resources(const std::string &root) {
	m_root = root;
}

void Resources::init(const std::string &root) {
	assert(m_instance == nullptr);
	m_instance = new Resources(root);
	m_instance->load();
}

Resources *Resources::instance() {
	assert(m_instance != nullptr);
	return m_instance;
}

void Resources::load() {
	loadColorTheme();
	loadPictures();
	loadFonts();
}

int Resources::getPicIdByUnicode(uint32_t num) {
	if ((num & 0xFF00) == 0xE100) {
		int index = num & 0xFF;
		if (index < COUNT_OF(E1XX_PICS))
			return E1XX_PICS[index];
	}
	return -1;
}

IMGHDR *Resources::getPicByUnicode(uint32_t num) {
	int id = getPicIdByUnicode(num);
	if (id != -1)
		return getPicture(id);
	return nullptr;
}

IMGHDR *Resources::getFontChar(int font_id, int16_t ch, bool allow_bitmaps) {
	Font *font = m_fonts[font_id];
	assert(font != nullptr);
	
	if ((ch & 0xFF00) == 0xE000 || ch == 0x0A || ch == 0x0D)
		return &font->empty_char;
	
	if (allow_bitmaps) {
		if ((ch & 0xFF00) == 0xE100) { // Bitmaps
			return getPicByUnicode(ch) ?: &font->chars.at(0xFFFF);
		} else if ((ch & 0xFF00) == 0xE200) { // Dyn images
			return &font->chars.at(0xFFFF);
		}
	}
	
	auto it = font->chars.find(ch);
	if (it != font->chars.end())
		return &it->second;
	
	return &font->chars.at(0xFFFF);
}

void Resources::loadFonts() {
	std::smatch m;
	std::regex font_re("^(\\d+)\\.font$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	std::string path = m_root + "/res/" + getPlatformName();
	
	for (const auto &entry : std::filesystem::directory_iterator(path)) {
		std::string fname = entry.path().filename();
		std::string path = entry.path();
		if (std::regex_match(fname, m, font_re)) {
			int id = strToInt(m[1].str(), 10, 0);
			Font *font = new Font;
			if (loadFont(font, path)) {
				m_fonts[id] = font;
			} else {
				delete font;
			}
		}
	}
}

bool Resources::loadFont(Font *font, const std::string &path) {
	std::string font_data = readFile(path);
	
	BinaryBufferReader r(font_data);
	
	uint8_t magic[8];
	if (!r.read(magic, 8) || memcmp(magic, "SIEFONT\0", 8) != 0)
		return false;
	
	uint16_t segments_n;
	if (!r.readUInt16(&segments_n))
		return false;
	
	uint8_t max_height = 0;
	uint8_t max_width = 0;
	
	for (int seg = 0; seg < segments_n; seg++) {
		uint16_t chars_n;
		uint16_t unicode_start;
		uint16_t unicode_end;
		
		if (!r.readUInt16(&chars_n))
			return false;
		if (!r.readUInt16(&unicode_start))
			return false;
		if (!r.readUInt16(&unicode_end))
			return false;
		
		for (int ch_n = 0; ch_n < chars_n; ch_n++) {
			uint8_t w;
			uint8_t h;
			
			if (!r.readUInt8(&w))
				return false;
			if (!r.readUInt8(&h))
				return false;
			
			int codepoint = unicode_start + ch_n;
			int bitmap_size = ((w + 7) / 8) * h;
			
			IMGHDR *ch = &font->chars[codepoint];
			ch->bpnum = IMGHDR_TYPE_WB;
			ch->bitmap = new uint8_t[bitmap_size];
			ch->w = w;
			ch->h = h;
			
			max_height = std::max(max_height, h);
			max_width = std::max(max_width, w);
			
			if (!r.read(ch->bitmap, bitmap_size))
				return false;
		}
	}
	
	font->empty_char.w = 0;
	font->empty_char.h = max_height;
	font->empty_char.bitmap = nullptr;
	font->empty_char.bpnum = IMGHDR_TYPE_WB;
	
	font->h = max_height;
	font->w = max_width;
	
	return true;
}

void Resources::loadPictures() {
	std::smatch m;
	std::regex pic_re("^(\\d+)\\.png$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	std::string path = SieFs::sie2path("0:\\ZBin\\img");
	
	for (const auto &entry: std::filesystem::directory_iterator(path)) {
		std::string fname = entry.path().filename();
		std::string path = entry.path();
		if (std::regex_match(fname, m, pic_re)) {
			int pit_index = strToInt(m[1].str(), 10, 0);
			IMGHDR *img = IMG_LoadAny(path, false);
			if (img)
				m_pit_table[pit_index] = img;
		}
	}
}

IMGHDR *Resources::getPicture(int num) {
	auto it = m_pit_table.find(num);
	if (it == m_pit_table.end()) {
		std::string path = m_root + "/res/" + getPlatformName() + "/PIT/" + strprintf("%04d.png", num);
		m_pit_table[num] = IMG_LoadAny(path, false);
		it = m_pit_table.find(num);
	}
	return it->second;
}

void Resources::loadColorTheme() {
	int color_index = 0;
	std::smatch m;
	std::regex color_re("^\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	
	std::string col = readFile(SieFs::sie2path("2:\\Default\\ColorControls.col"));
	
	bool color_data_started = false;
	for (auto line: strSplit("\n", col)) {
		line = trim(line);
		if (!color_data_started) {
			color_data_started = strStartsWith(line, "Data");
		} else if (line.size() > 0) {
			assert(color_index < MAX_EXT_PALETTE_COLORS);
			assert(std::regex_match(line, m, color_re));
			
			m_ext_pallete[color_index][0] = strToInt(m[1].str(), 10, 0);
			m_ext_pallete[color_index][1] = strToInt(m[2].str(), 10, 0);
			m_ext_pallete[color_index][2] = strToInt(m[3].str(), 10, 0);
			m_ext_pallete[color_index][3] = strToInt(m[4].str(), 10, 0);
			color_index++;
		}
	}
}

char *Resources::getColorPtr(int index) {
	if (index >= 0 && index < MAX_INT_PALETTE_COLORS)
		return m_int_pallete[index];
	
	if (index >= 0x64 && index < (0x64 + MAX_EXT_PALETTE_COLORS))
		return m_ext_pallete[index - 0x64];
	
	return m_int_pallete[MAX_INT_PALETTE_COLORS - 1];
}
