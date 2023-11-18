#include "swi.h"
#include "log.h"
#include "utils.h"
#include "SieFs.h"

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <regex>
#include <string>
#include <map>

struct SettingsAE {
	int id;
	std::map<std::string, std::string> values;
};

static const char *SET_TO_PATH[] = {
	[0]		= "1:\\Default\\fak.pd",
	[1]		= "1:\\Default\\mymenu.pd",
	[2]		= "1:\\Default\\vibralight.pd",
	[3]		= "1:\\Default\\userprofiles.pd",
	[4]		= "1:\\Default\\setup.pd",
	[5]		= "1:\\Default\\developer_menu.pd",
	[6]		= "1:\\Default\\apidc_setup.pd",
	[7]		= "1:\\Default\\mediaplayer.pd",
	[8]		= "1:\\Default\\soundrecorder.pd",
	[9]		= "1:\\Default\\frdb.pd",
	[10]	= "1:\\Default\\avatar.pd",
	[11]	= "1:\\Default\\radio.pd",
	[12]	= "1:\\Default\\orangehomescreen.pd",
};

static constexpr int MAX_SETTINGS_ID = (sizeof(SET_TO_PATH) / sizeof(SET_TO_PATH[0])) - 1;

static std::string SettingsAE_GetPath(int id);
static void SettingsAE_Load(SettingsAE *settings, int id);
static std::string SettingsAE_Key(char *entry, char *keyword);

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
	LOGE("%s: undefined entry and keyword\n", __func__);
	abort();
	return "";
}

std::map<std::string, std::string> SettingsAE_Load(int id) {
	std::string file_path = SieFs::sie2path(SettingsAE_GetPath(id));
	
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

std::tuple<bool, std::string> SettingsAE_GetValue(int id, char *entry, char *keyword) {
	auto values = SettingsAE_Load(id);
	auto key = SettingsAE_Key(entry, keyword);
	if (values.find(key) != values.end())
		return { true, values[key] };
	return { false, "" };
}

int SettingsAE_Read_ws(WSHDR *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	auto [status, value] = SettingsAE_GetValue(id, entry, keyword);
	if (status)
		utf8_2ws(rvalue, value.c_str(), rvalue->maxlen - 1);
	return status;
}

int SettingsAE_Update_ws(WSHDR *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SettingsAE_Read(int *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	auto [status, value] = SettingsAE_GetValue(id, entry, keyword);
	if (status)
		*rvalue = strToInt(value);
	return status;
}

int SettingsAE_Update(int val, int id, char *entry, char *keyword) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SettingsAE_GetFlag(int *rvalue, int id, char *entry, char *keyword) {
	assert(rvalue != nullptr);
	auto [status, value] = SettingsAE_GetValue(id, entry, keyword);
	if (status)
		*rvalue = (value == "1" ? 1 : 0);
	return status;
}

int SettingsAE_SetFlag(int val, int id, char * entry, char *keyword) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

void *SettingsAE_GetEntryList(int id) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SettingsAE_RemoveEntry(int id, char *entry, int flag) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}
