#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glob.h>
#include <swilib/file.h>
#include <spdlog/spdlog.h>
#include <swilib/wstring.h>

#include "src/FFS.h"
#include "src/swi/wstring.h"
#include "src/utils/string.h"

struct FileSearchCtx {
	glob_t glob = {};
	char **found = nullptr;
	size_t found_cnt = 0;
};

static std::unordered_map<size_t, FileSearchCtx *> search_contexts;

static void _set_errno(int ret, uint32_t *errp) {
	if (errp) {
		if (ret < 0) {
			*errp = 1;
		} else {
			*errp = 0;
		}
	}
}

int sys_open(const char *path, uint32_t mode, uint32_t permission, uint32_t *err) {
	int unix_flags = 0;
	mode_t unix_mode = S_IRUSR | S_IWUSR;

	if ((mode & A_ReadOnly))
		unix_flags |= O_RDONLY;

	if ((mode & A_WriteOnly))
		unix_flags |= O_WRONLY;

	if ((mode & A_ReadWrite))
		unix_flags |= O_RDWR;

	if ((mode & A_Create))
		unix_flags |= O_CREAT;

	if ((mode & A_Exclusive))
		unix_flags |= O_EXCL;

	if ((mode & A_Truncate))
		unix_flags |= O_TRUNC;

	if ((mode & A_Append))
		unix_flags |= O_APPEND;

	int ret;
	if (strcmp(path, "0:\\Misc\\stdout.txt") == 0 || strcmp(path, "4:\\stdout.txt") == 0) {
		ret = dup(STDOUT_FILENO);
	} else if (strcmp(path, "0:\\Misc\\stderr.txt") == 0 || strcmp(path, "4:\\stderr.txt") == 0) {
		ret = dup(STDERR_FILENO);
	} else {
		ret = open(FFS::dos2unix(path).c_str(), unix_flags, unix_mode);
	}

	_set_errno(ret, err);
	return ret;
}

int sys_read(int fd, void *buffer, int length, uint32_t *err) {
	int ret = read(fd, buffer, length);
	_set_errno(ret, err);
	return ret;
}

int sys_write(int fd, const void *buffer, int length, uint32_t *err) {
	int ret = write(fd, buffer, length);
	_set_errno(ret, err);
	return ret;
}

int sys_close(int fd, uint32_t *err) {
	int ret = close(fd);
	_set_errno(ret, err);
	return ret;
}

int sys_fflush(int fd, uint32_t *err) {
	int ret = fsync(fd);
	_set_errno(ret, err);
	return ret;
}

long sys_lseek(int fd, uint32_t offset, uint32_t origin, uint32_t *err, uint32_t *err2) {
	int ret = lseek(fd, offset, origin);
	_set_errno(ret, err);
	_set_errno(ret, err2);
	return ret;
}

int sys_mkdir(const char *dirname, uint32_t *err) {
	int ret = mkdir(FFS::dos2unix(dirname).c_str(), 0755);
	_set_errno(ret, err);
	return ret;
}

int GetFileAttrib(const char *path, uint8_t *attr, uint32_t *err) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SetFileAttrib(const char *path, uint8_t attr, uint32_t *err) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int setfilesize(int fd, uint32_t length, uint32_t *err) {
	int ret = ftruncate(fd, length);
	_set_errno(ret, err);
	return ret;
}

int GetWavLen(const char *path) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

static void _fillDirEntry(DIR_ENTRY *de, const std::string &file) {
	struct stat st;
	if (lstat(file.c_str(), &st) == 0) {
		de->file_size = st.st_size;
		de->create_date_time = st.st_ctime;

		std::string filename = std::filesystem::path(file).filename();
		std::string dirname = std::filesystem::path(file).parent_path();

		if (strStartsWith(filename, "."))
			de->file_attr |= FA_HIDDEN;

		if (access(file.c_str(), F_OK | W_OK) != 0)
			de->file_attr |= FA_READONLY;

		if ((st.st_mode & S_IFMT) == S_IFDIR)
			de->file_attr |= FA_DIRECTORY;

		strncpy(de->file_name, filename.c_str(), sizeof(de->file_name) - 1);
		strncpy(de->folder_name, FFS::unix2dos(dirname).c_str(), sizeof(de->folder_name) - 1);
	}
}

int FindFirstFile(DIR_ENTRY *dir_entry, const char *mask, uint32_t *err) {
	std::string unix_path = FFS::dos2unix(mask);

	auto search_id = reinterpret_cast<size_t>(dir_entry);
	if (search_contexts.find(search_id) != search_contexts.end()) {
		if (err)
			*err = 1;
		return 0;
	}

	FileSearchCtx *ctx = new FileSearchCtx;
	int ret = glob(unix_path.c_str(), 0, NULL, &ctx->glob);
	if (ret != 0) {
		if (err)
			*err = 1;
		delete ctx;
		return 0;
	} else {
		if (err)
			*err = 0;

		ctx->found = ctx->glob.gl_pathv;
		ctx->found_cnt = ctx->glob.gl_pathc;

		if (ctx->found_cnt > 0) {
			search_contexts[search_id] = ctx;
			_fillDirEntry(dir_entry, *ctx->found);
			ctx->found++;
			ctx->found_cnt--;
			return 1;
		} else {
			globfree(&ctx->glob);
			delete ctx;
		}

		return 0;
	}
}

int FindNextFile(DIR_ENTRY *dir_entry, uint32_t *err) {
	auto search_id = reinterpret_cast<size_t>(dir_entry);
	if (search_contexts.find(search_id) != search_contexts.end()) {
		FileSearchCtx *ctx = search_contexts[search_id];
		if (err)
			*err = 0;
		if (ctx->found_cnt > 0) {
			_fillDirEntry(dir_entry, *ctx->found);
			ctx->found++;
			ctx->found_cnt--;
			return 1;
		}
		return 0;
	} else {
		if (err)
			*err = 1;
		return 0;
	}
}

int FindClose(DIR_ENTRY *dir_entry, uint32_t *err) {
	auto search_id = reinterpret_cast<size_t>(dir_entry);
	if (search_contexts.find(search_id) != search_contexts.end()) {
		FileSearchCtx *ctx = search_contexts[search_id];
		if (err)
			*err = 0;
		globfree(&ctx->glob);
		search_contexts.erase(search_id);
		delete ctx;
		return 1;
	} else {
		if (err)
			*err = 1;
		return 0;
	}
}

int sys_stat(const char *path, FSTATS *stat, uint32_t *err) {
	std::string file = FFS::dos2unix(path);
	struct stat st;
	int ret = lstat(file.c_str(), &st);

	if (ret == 0) {
		_set_errno(0, err);
		stat->size = st.st_size;
		if (access(file.c_str(), F_OK | W_OK) != 0)
			stat->file_attr |= FA_READONLY;
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			stat->file_attr |= FA_DIRECTORY;
		return 0;
	} else {
		_set_errno(ret, err);
		return ret;
	}
}

int GetFreeFlexSpace(int drive, uint32_t *err) {
	return 8 * 1024 * 1024; // stub
}

int GetTotalFlexSpace(int drive, uint32_t *err) {
	return 64 * 1024 * 1024; // stub
}

int sys_rename(const char *old_name, const char *new_name, uint32_t *err) {
	std::string src_path = FFS::dos2unix(old_name);
	std::string dst_path = FFS::dos2unix(new_name);
	int ret = rename(src_path.c_str(), dst_path.c_str());
	_set_errno(ret, err);
	return ret;
}

int sys_rmdir(const char *dirname, uint32_t *err) {
	int ret = rmdir(FFS::dos2unix(dirname).c_str());
	_set_errno(ret, err);
	return ret;
}

int sys_truncate(int fd, int length, uint32_t *err) {
	int ret = ftruncate(fd, length);
	_set_errno(ret, err);
	return ret;
}

int isdir(const char *path, uint32_t *err) {
	std::string file = FFS::dos2unix(path);
	struct stat st;
	int ret = lstat(file.c_str(), &st);

	if (ret == 0) {
		return ((st.st_mode & S_IFMT) == S_IFDIR);
	} else {
		_set_errno(ret, err);
		return 0;
	}
}

int sys_unlink(const char *path, uint32_t *err) {
	int ret = unlink(FFS::dos2unix(path).c_str());
	_set_errno(ret, err);
	return ret;
}

int GetFileProp(FILE_PROP *props, const WSHDR *file, const WSHDR *path) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int fexists(WSHDR *path) {
	return access(ws2string(path).c_str(), F_OK) == 0;
}

int sys_format(uint16_t drive, uint32_t *err) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int sys_fstat(int fd, FSTATS *stat, uint32_t *err) {
	struct stat st;
	int ret = fstat(fd, &st);

	if (ret == 0) {
		_set_errno(0, err);
		stat->size = st.st_size;
		if (faccessat(fd, "", F_OK | W_OK, AT_EMPTY_PATH) != 0)
			stat->file_attr |= FA_READONLY;
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			stat->file_attr |= FA_DIRECTORY;
		return 0;
	} else {
		_set_errno(ret, err);
		return ret;
	}
}

int sys_mkdir_ws(const WSHDR *dirname, uint32_t *err) {
	return sys_mkdir(ws2string(dirname).c_str(), err);
}

int sys_rename_ws(const WSHDR *old_name, const WSHDR *new_name, uint32_t *err) {
	return sys_rename(ws2string(old_name).c_str(), ws2string(new_name).c_str(), err);
}

int isdir_ws(const WSHDR *path, uint32_t *err) {
	return isdir(ws2string(path).c_str(), err);
}
