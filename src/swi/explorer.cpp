#include "explorer.h"
#include <cstdio>
#include <spdlog/spdlog.h>
#include <swilib/explorer.h>
#include "src/swi/wstring.h"
#include "src/utils/string.h"

struct ExplorerExt {
	int uid;
	const char name[32];
};

static ExplorerExt EXTENSIONS[] = {
	{ UID_MP3, "mp3" },
	{ UID_M3U, "m3u" },
	{ UID_JAR, "jar" },
	{ UID_JAD, "jad" },
	{ UID_MID, "mid" },
	{ UID_AMR, "amr" },
	{ UID_IMY, "imy" },
	{ UID_SRT, "srt" },
	{ UID_AAC, "aac" },
	{ UID_WAV, "wav" },
	{ UID_JTS, "jts" },
	{ UID_XMF, "xmf" },
	{ UID_M4A, "m4a" },
	{ UID_BMX, "bmx" },
	{ UID_WBMP, "wbmp" },
	{ UID_JPG, "jpg" },
	{ UID_JPG, "jpeg" },
	{ UID_PNG, "png" },
	{ UID_GIF, "gif" },
	{ UID_SVG, "svg" },
	{ UID_3GP, "3gp" },
	{ UID_MP4, "mp4" },
	{ UID_SDP, "sdp" },
	{ UID_PVX, "pvx" },
	{ UID_SDT, "sdt" },
	{ UID_LDB, "ldb" },
	{ UID_TXT, "txt" },
	{ UID_URL, "url" },
	{ 0, "" }
};

void RegExplorerExt(const REGEXPLEXT *ext) {
	spdlog::debug("{}: not implemented!", __func__);
}

int ExecuteFile(const WSHDR *filepath, const WSHDR *mime, void *param) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int UnRegExplorerExt(const REGEXPLEXT *ext) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

TREGEXPLEXT *EXT_TOP(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

TREGEXPLEXT *get_regextpnt_by_uid(int uid) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetExtUid_ws(const WSHDR *ext) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int ExplorerCopyFile(const WSHDR *from, const WSHDR *to, int overwrite, char transfer_id, uint32_t *errp) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int ExplorerSetTransferState(char transfer_id, int state) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CardExplGetCurItem(void *csm) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void CardExplGetFName(void *csm, int index, WSHDR *filename) {
	spdlog::debug("{}: not implemented!", __func__);
}

int OpenExplorer(NativeExplorerData *data) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int StartNativeExplorer(NativeExplorerData *data) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void MediaSendCSM_Open(const WSHDR *file, const WSHDR *path) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GetExtUidByFileName_ws(const WSHDR *fn) {
	std::string filename = ws2string(fn);
	auto parts = strSplit(".", filename);
	if (parts.size() < 2)
		return 0;

	std::string ext = parts[parts.size() - 1];

	ExplorerExt *cursor = &EXTENSIONS[0];
	while (cursor && cursor->uid) {
		if (strcasecmp(cursor->name, ext.c_str()) == 0)
			return cursor->uid;
		cursor++;
	}

	return 0;
}

int GetMimeType(int uid, WSHDR *mime_type, int unk_0) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int PersMenuCSM_Open(const WSHDR *file_name, const WSHDR *dir, int flag, int open_mode) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int BuildPath(WSHDR *path, const WSHDR *file_name, const WSHDR *dir) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}
