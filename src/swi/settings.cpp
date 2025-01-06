#include <cstdint>
#include <map>
#include <regex>
#include <cassert>
#include <spdlog/spdlog.h>
#include <swilib/settings.h>

#include "src/FFS.h"
#include "src/utils/fs.h"
#include "src/utils/string.h"

struct SettingsAE {
	int id;
	std::map<std::string, std::string> values;
};

static const char *SET_TO_PATH[] = {
	"1:\\Default\\fak.pd",
	"1:\\Default\\mymenu.pd",
	"1:\\Default\\vibralight.pd",
	"1:\\Default\\userprofiles.pd",
	"1:\\Default\\setup.pd",
	"1:\\Default\\developer_menu.pd",
	"1:\\Default\\apidc_setup.pd",
	"1:\\Default\\mediaplayer.pd",
	"1:\\Default\\soundrecorder.pd",
	"1:\\Default\\frdb.pd",
	"1:\\Default\\avatar.pd",
	"1:\\Default\\radio.pd",
	"1:\\Default\\orangehomescreen.pd",
};

static constexpr int MAX_SETTINGS_ID = (sizeof(SET_TO_PATH) / sizeof(SET_TO_PATH[0])) - 1;

static std::string SettingsAE_GetPath(int id) {
	assert(id >= 0 && id <= MAX_SETTINGS_ID);
	assert(SET_TO_PATH[id] != nullptr);
	return SET_TO_PATH[id];
}

static std::string SettingsAE_Key(char *entry, char *keyword) {
	if (entry && keyword) {
		return std::string(entry) + std::string(".") + std::string(keyword);
	} else if (entry) {
		return entry;
	} else if (keyword) {
		return keyword;
	}
	spdlog::error("{}: undefined entry and keyword\n", __func__);
	abort();
}

static std::map<std::string, std::string> SettingsAE_Load(int id) {
	std::string file_path = FFS::dos2unix(SettingsAE_GetPath(id));

	std::smatch m;
	std::regex color_re("^\\d+:T:(.*?)=(.*?)$", std::regex_constants::ECMAScript | std::regex_constants::icase);

	std::map<std::string, std::string> values;

	for (auto line: strSplit("\n", readFile(file_path))) {
		line = trim(line);

		if (!line.size())
			continue;

		assert(std::regex_match(line, m, color_re));
		values[m[1].str()] = m[2].str();
	}

	return values;
}

static std::tuple<bool, std::string> SettingsAE_GetValue(int id, char *entry, char *keyword) {
	auto values = SettingsAE_Load(id);
	auto key = SettingsAE_Key(entry, keyword);
	if (values.find(key) != values.end())
		return { true, values[key] };
	return { false, "" };
}

void Ringtones_SetState(uint8_t state) {
	spdlog::debug("{}: not implemented!", __func__);
}

int Registry_GetHMIKeyID(char *key) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Registry_SetResourcePath(int hmi_key_id, int prio, const WSHDR *path) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Registry_DeleteResourcePath(int hmi_key_id, int prio) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Registry_GetResourcePathFromLevel(int hmi_key_id, int start_prio, int *prio, WSHDR *path) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void getCurrentProfileName(WSHDR *name) {
	spdlog::debug("{}: not implemented!", __func__);
}

char GetProfile() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char SetProfile(char profile_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *RamProfileNum() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int pdcache_getval(char *value, int maxlen, int set_id, const char *entry, const char *keyword) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int pdcache_setval(const char *value, int set_id, const char *entry, const char *keyword) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SetDateTime(const TDate *date, const TTime *time) {
	spdlog::debug("{}: not implemented!", __func__);
}

void getProfileNameByN(WSHDR *name, int profile_id, int unk) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GetProfileVolumeSetting(int profile_id, int type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SaveMaxIllumination(int level) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetIlluminationoffTimeout(int timeout) {
	spdlog::debug("{}: not implemented!", __func__);
}

int SettingsAE_Update_ws(WSHDR *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SettingsAE_Read_ws(WSHDR *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	auto [status, value] = SettingsAE_GetValue(id, entry, keyword);
	if (status)
		utf8_2ws(rvalue, value.c_str(), rvalue->maxlen - 1);
	return status;
}

int SettingsAE_SetFlag(int value, int set_id, char *entry, char *keyword) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SettingsAE_GetFlag(int *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	auto [status, value] = SettingsAE_GetValue(id, entry, keyword);
	if (status)
		*rvalue = (value == "1" ? 1 : 0);
	return status;
}

int SettingsAE_Update(int value, int set_id, char *entry, char *keyword) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SettingsAE_Read(int *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	auto [status, value] = SettingsAE_GetValue(id, entry, keyword);
	if (status)
		*rvalue = strToInt(value);
	return status;
}

char **SettingsAE_GetEntryList(int set_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int SettingsAE_RemoveEntry(int set_id, char *entry, int flag) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SettingsAE_RegisterListener(SettingsAE_ListenerProc proc, int set_id, const char *entry, const char *keyword) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SettingsAE_DeregisterListener(SettingsAE_ListenerProc proc, int set_id, const char *entry, const char *keyword) {
	spdlog::debug("{}: not implemented!", __func__);
}
