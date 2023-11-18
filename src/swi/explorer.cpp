#include "swi.h"
#include "utils.h"

#include <cstdio>

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

int GetExtUidByFileName_ws(WSHDR *fn) {
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
