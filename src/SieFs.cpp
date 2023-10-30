#include "SieFs.h"

#include <regex>
#include <filesystem>

#include "log.h"
#include "utils.h"

static std::map<std::string, std::string> m_drives;

void SieFs::mount(const std::string &drive, const std::string &path) {
	m_drives[drive] = std::filesystem::canonical(path + "/");
	if (m_drives[drive] == "/")
		m_drives[drive] = "";
	// LOGD("mount: %s -> %s\n", drive.c_str(), m_drives[drive].c_str());
}

std::string SieFs::path2sie(const std::string &unix_path) {
	std::string siemens_path;
	for (auto [drive, mount_path]: m_drives) {
		if (unix_path == mount_path) {
			siemens_path = drive + ":" + unix_path;
			break;
		} else if (mount_path.size() > 0 && strStartsWith(unix_path, mount_path + "/")) {
			siemens_path = drive + ":" + unix_path.substr(mount_path.size());
			break;
		} else if (!mount_path.size()) {
			siemens_path = drive + ":" + unix_path; // fallback
		}
	}
	
	std::replace(siemens_path.begin(), siemens_path.end(), '/', '\\');
	// LOGD("path2sie: %s -> %s\n", unix_path.c_str(), siemens_path.c_str());
	
	if (!siemens_path.size()) {
		LOGE("Invalid unix path: %s\n", unix_path.c_str());
		abort();
	}
	
	return siemens_path;
}

std::string SieFs::sie2path(const std::string &siemens_path) {
	std::regex path_re("^([0-9a-f]):\\\\(.*?)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	
	std::smatch m;
	if (std::regex_match(siemens_path, m, path_re)) {
		auto drive = m[1].str();
		auto path = m[2].str();
		
		std::replace(path.begin(), path.end(), '\\', '/');
		
		if (m_drives.find(drive) != m_drives.end()) {
			std::string unix_path = m_drives[drive] + "/" + path;
			// LOGD("sie2path: %s -> %s\n", siemens_path.c_str(), unix_path.c_str());
			return unix_path;
		}
	}
	LOGE("Invalid siemens path: %s\n", siemens_path.c_str());
	abort();
	return "";
}
