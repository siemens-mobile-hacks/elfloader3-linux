#include "src/FFS.h"
#include "src/utils/string.h"

#include <spdlog/spdlog.h>
#include <format>
#include <regex>
#include <filesystem>

const std::regex DOS_PATH_RE("^([0-9a-f]:)(?:\\\\|/)(.*?)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
static std::map<std::string, std::string> m_mountPoints;

static void rsync(const std::string &source, const std::string &destination);

void FFS::mount(const std::string &drive, const std::string &path) {
	spdlog::debug("Mounting: {}/ -> {}", path, drive);
	m_mountPoints[drive] = std::filesystem::canonical(path + "/");
	if (m_mountPoints[drive] == "/")
		m_mountPoints[drive] = "";
}

void FFS::mountAndInit(const std::string &drive, const std::string &path, const std::string &defaultContentPath) {
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directories(path);
		if (std::filesystem::exists(defaultContentPath)) {
			spdlog::debug("Copying: {} -> {}", defaultContentPath, path);
			rsync(defaultContentPath, path);
		}
	}
	mount(drive, path);
}

std::string FFS::any2dos(const std::string &unknownPath) {
	std::smatch m;
	if (std::regex_match(unknownPath, m, DOS_PATH_RE)) {
		auto drive = m[1].str();
		auto path = m[2].str();
		std::replace(path.begin(), path.end(), '/', '\\');
		return drive + ":\\" + path;
	} else {
		return unix2dos(unknownPath);
	}
}

std::string FFS::any2unix(const std::string &unknownPath) {
	std::smatch m;
	if (std::regex_match(unknownPath, m, DOS_PATH_RE)) {
		auto drive = m[1].str();
		auto path = m[2].str();
		std::replace(path.begin(), path.end(), '/', '\\');
		return dos2unix(drive + ":\\" + path);
	} else {
		return unknownPath;
	}
}

std::string FFS::unix2dos(const std::string &unixPath) {
	std::string dosPath;
	for (auto [drive, mountPath]: m_mountPoints) {
		if (unixPath == mountPath) {
			dosPath = drive + unixPath;
			break;
		} else if (mountPath.size() > 0 && strStartsWith(unixPath, mountPath + "/")) {
			dosPath = drive + unixPath.substr(mountPath.size());
			break;
		} else if (!mountPath.size()) {
			dosPath = drive + unixPath; // fallback
		}
	}
	if (!dosPath.size())
		throw std::runtime_error(std::format("Path is not mounted: {}", unixPath));
	std::replace(dosPath.begin(), dosPath.end(), '/', '\\');
	// spdlog::debug("unix2dos: {} -> {}", unixPath, dosPath);
	return dosPath;
}

std::string FFS::dos2unix(const std::string &dosPath) {
	std::smatch m;
	if (std::regex_match(dosPath, m, DOS_PATH_RE)) {
		auto drive = m[1].str();
		auto path = m[2].str();
		std::replace(path.begin(), path.end(), '\\', '/');
		if (m_mountPoints.find(drive) != m_mountPoints.end()) {
			std::string unixPath = m_mountPoints[drive] + "/" + path;
			if (!std::filesystem::exists(unixPath))
				unixPath = caseInsensitivePath(unixPath);
			// spdlog::debug("dos2unix: {} -> {}", dosPath, unixPath);
			return unixPath;
		}
		throw std::runtime_error(std::format("Path is not mounted: {}", dosPath));
	}
	throw std::runtime_error(std::format("Invalid dos path: {}", dosPath));
}

std::string FFS::caseInsensitivePath(const std::string &path) {
	std::string tmpPath = "";
	auto parts = strSplit("/", path);
	size_t i = 0;
	for (auto &part: parts) {
		if (part == "") {
			i++;
			continue;
		}
		bool resolved = false;
		for (const auto &entry: std::filesystem::directory_iterator(tmpPath == "" ? "/" : tmpPath)) {
			if (i < parts.size() - 1 && !entry.is_directory())
				continue;
			if (strcasecmp(entry.path().filename().c_str(), part.c_str()) == 0) {
				tmpPath += "/" + std::string(entry.path().filename().c_str());
				resolved = true;
				break;
			}
		}
		if (!resolved)
			return path;
		i++;
	}
	return tmpPath;
}

static void rsync(const std::string &source, const std::string &destination) {
	if (!std::filesystem::exists(destination))
		std::filesystem::create_directories(destination);
	for (const auto &entry: std::filesystem::directory_iterator(source)) {
		const auto &entryPath = entry.path();
		auto destinationPath = destination / entryPath.filename();
		if (std::filesystem::is_directory(entryPath)) {
			rsync(entryPath, destinationPath);
		} else if (std::filesystem::is_regular_file(entryPath) || std::filesystem::is_symlink(entryPath)) {
			std::filesystem::copy(entryPath, destinationPath, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		} else {
			throw std::runtime_error(std::format("Unrecognized file type: {}", entryPath.string()));
		}
	}
}
