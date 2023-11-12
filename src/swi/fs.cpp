#include "swi.h"
#include "SieFs.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glob.h>

struct FileSearchCtx {
	glob_t glob;
	char **found;
	size_t found_cnt;
};

static void _set_errno(int ret, uint32_t *errp) {
	if (ret < 0) {
		*errp = 1;
	} else {
		*errp = 0;
	}
}

int FS_open(const char *path, uint32_t f, uint32_t m, uint32_t *errp) {
	int flags = 0;
	mode_t mode = S_IRUSR | S_IWUSR;
	
	if ((f & A_ReadOnly))
		flags |= O_RDONLY;
	
	if ((f & A_WriteOnly))
		flags |= O_WRONLY;
	
	if ((f & A_ReadWrite))
		flags |= O_RDWR;
	
	if ((f & A_Create))
		flags |= O_CREAT;
	
	if ((f & A_Exclusive))
		flags |= O_EXCL;
	
	if ((f & A_Truncate))
		flags |= O_TRUNC;
	
	if ((f & A_Append))
		flags |= O_APPEND;
	
	int ret;
	if (strcmp(path, "0:\\Misc\\stdout.txt") == 0) {
		ret = dup(STDOUT_FILENO);
	} else if (strcmp(path, "0:\\Misc\\stderr.txt") == 0) {
		ret = dup(STDERR_FILENO);
	} else {
		ret = open(SieFs::sie2path(path).c_str(), flags, mode);
	}
	
	if (SWI_TRACE) {
		fprintf(stderr, "[strace] open(%s, %d, %d) = %d\n", SieFs::sie2path(path).c_str(), flags, mode, ret);
	}
	
	_set_errno(ret, errp);
	return ret;
}

int FS_read(int fd, void *buff, int count, uint32_t *errp) {
	int ret = read(fd, buff, count);
	if (SWI_TRACE) {
		fprintf(stderr, "[strace] read(%d, %p, %d) = %d\n", fd, buff, count, ret);
	}
	_set_errno(ret, errp);
	return ret;
}

int FS_write(int fd, const void *buff, int count, uint32_t *errp) {
	int ret = write(fd, buff, count);
	if (SWI_TRACE) {
		fprintf(stderr, "[strace] write(%d, %p, %d) = %d\n", fd, buff, count, ret);
	}
	_set_errno(ret, errp);
	return ret;
}

int FS_close(int fd, uint32_t *errp) {
	int ret = close(fd);
	if (SWI_TRACE) {
		fprintf(stderr, "[strace] close(%d) = %d\n", fd, ret);
	}
	_set_errno(ret, errp);
	return ret;
}

int FS_flush(int fd, uint32_t *errp) {
	int ret = fsync(fd);
	if (SWI_TRACE) {
		fprintf(stderr, "[strace] fsync(%d) = %d\n", fd, ret);
	}
	_set_errno(ret, errp);
	return ret;
}

long FS_lseek(int fd, uint32_t offset, uint32_t origin, uint32_t *errp, uint32_t *errp2) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_mkdir(const char *pathname, uint32_t *errp) {
	int ret = mkdir(SieFs::sie2path(pathname).c_str(), 0755);
	if (SWI_TRACE) {
		fprintf(stderr, "[strace] mkdir(%s) = %d\n", SieFs::sie2path(pathname).c_str(), ret);
	}
	_set_errno(ret, errp);
	return ret;
}

int FS_GetFileAttrib(const char *cFileName, uint8_t *cAttribute, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_SetFileAttrib(const char *cFileName, uint8_t cAttribute, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_setfilesize(int FileHandler, uint32_t iNewFileSize, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
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
		strncpy(de->folder_name, SieFs::path2sie(dirname).c_str(), sizeof(de->folder_name) - 1);
	}
}

int FS_FindFirstFile(DIR_ENTRY *de, const char *pattern, uint32_t *errp) {
	FileSearchCtx *ctx = new FileSearchCtx;
	
	std::string unix_path = SieFs::sie2path(pattern);
	
	int ret = glob(unix_path.c_str(), 0, NULL, &ctx->glob);
	if (ret != 0) {
		if (errp)
			*errp = 1;
		delete ctx;
		return 0;
	} else {
		if (errp)
			*errp = 0;
		
		ctx->found = ctx->glob.gl_pathv;
		ctx->found_cnt = ctx->glob.gl_pathc;
		
		if (ctx->found_cnt > 0) {
			de->priv = (void *) ctx;
			_fillDirEntry(de, *ctx->found);
			ctx->found++;
			ctx->found_cnt--;
			return 1;
		} else {
			de->priv = nullptr;
			delete ctx;
			globfree(&ctx->glob);
		}
		
		return 0;
	}
}

int FS_FindNextFile(DIR_ENTRY *de, uint32_t *errp) {
	FileSearchCtx *ctx = reinterpret_cast<FileSearchCtx *>(de->priv);
	if (ctx) {
		if (errp)
			*errp = 0;
		if (ctx->found_cnt > 0) {
			_fillDirEntry(de, *ctx->found);
			ctx->found++;
			ctx->found_cnt--;
			return 1;
		}
		return 0;
	} else {
		if (errp)
			*errp = 1;
		return 0;
	}
}

int FS_FindClose(DIR_ENTRY *de, uint32_t *errp) {
	FileSearchCtx *ctx = reinterpret_cast<FileSearchCtx *>(de->priv);
	if (ctx) {
		if (errp)
			*errp = 0;
		globfree(&ctx->glob);
		de->priv = nullptr;
		delete ctx;
		return 1;
	} else {
		if (errp)
			*errp = 1;
		return 0;
	}
}

int FS_fmove(const char * SourceFileName, const char * DestFileName, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_rmdir(const char * cDirectory, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_truncate(int FileHandler, int length, int *errornumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_isdir(const char * cDirectory, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int FS_GetFileStats(const char *siemens_file, FSTATS *out_st, uint32_t *errp) {
	std::string file = SieFs::sie2path(siemens_file);
	struct stat st;
	if (lstat(file.c_str(), &st) == 0) {
		if (errp)
			*errp = 0;
		
		out_st->size = st.st_size;
		
		std::string filename = std::filesystem::path(file).filename();
		
		if (strStartsWith(filename, "."))
			out_st->file_attr |= FA_HIDDEN;
		
		if (access(file.c_str(), F_OK | W_OK) != 0)
			out_st->file_attr |= FA_READONLY;
		
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			out_st->file_attr |= FA_DIRECTORY;
		
		return 0;
	} else {
		if (errp)
			*errp = 1;
	}
	return 1;
}
