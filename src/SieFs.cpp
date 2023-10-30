#include "SieFs.h"

#include <regex>

static std::map<std::string, std::string> m_drives;

void SieFs::mount(const std::string &drive, const std::string &path) {
	m_drives[drive] = path;
}

std::string SieFs::path2sie(const std::string &path) {
	return "";
}

std::string SieFs::sie2path(const std::string &uri) {
	std::regex path_re("^([0-9a-f]):\\\\(.*?)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	
	std::smatch m;
	if (std::regex_match(uri, m, path_re)) {
		auto drive = m[1].str();
		auto path = m[2].str();
		
		std::replace(path.begin(), path.end(), '\\', '/');
		
		if (m_drives.find(drive) != m_drives.end()) {
			std::string unix_path = m_drives[drive] + "/" + path;
			fprintf(stderr, "sie2path: %s:%s | -> %s\n", drive.c_str(), path.c_str(), unix_path.c_str());
			return unix_path;
		}
	}
	fprintf(stderr, "Invalid siemens path: %s\n", uri.c_str());
	abort();
	return "";
}
