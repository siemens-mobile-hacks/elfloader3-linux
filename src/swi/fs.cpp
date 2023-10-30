#include "../swi.h"
#include "../SieFs.h"
#include "../utils.h"

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

int SWI_open(const char *path, uint32_t f, uint32_t m, uint32_t *errp) {
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
	
	// fprintf(stderr, "open(%s) = %d\n", path, ret);
	
	if (ret < 0) {
		perror("[SWI] open");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

int SWI_read(int fd, void *buff, int count, uint32_t *errp) {
	int ret = read(fd, buff, count);
	if (ret < 0) {
		perror("[SWI] read");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

int SWI_write(int fd, const void *buff, int count, uint32_t *errp) {
	int ret = write(fd, buff, count);
	if (ret < 0) {
		fprintf(stderr, "fd=%d\n", fd);
		perror("[SWI] write");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

int SWI_close(int fd, uint32_t *errp) {
	int ret = close(fd);
	// fprintf(stderr, "close(%d) = %d\n", fd, ret);
	if (ret != 0) {
		perror("[SWI] close");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

int SWI_flush(int fd, uint32_t *errp) {
	int ret = fsync(fd);
	if (ret != 0) {
		perror("[SWI] flush");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

long SWI_lseek(int fd, uint32_t offset, uint32_t origin, uint32_t *errp, uint32_t *errp2) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_mkdir(const char * cFileName, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_GetFileAttrib(const char *cFileName, uint8_t *cAttribute, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_SetFileAttrib(const char *cFileName, uint8_t cAttribute, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_setfilesize(int FileHandler, uint32_t iNewFileSize, uint32_t *ErrorNumber) {
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

int SWI_FindFirstFile(DIR_ENTRY *de, const char *pattern, uint32_t *errp) {
	FileSearchCtx *ctx = new FileSearchCtx;
	
	std::string unix_path = SieFs::sie2path(pattern);
	
	int ret = glob(unix_path.c_str(), 0, NULL, &ctx->glob);
	if (ret != 0) {
		perror("[SWI] glob");
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

int SWI_FindNextFile(DIR_ENTRY *de, uint32_t *errp) {
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

int SWI_FindClose(DIR_ENTRY *de, uint32_t *errp) {
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

int SWI_fmove(const char * SourceFileName, const char * DestFileName, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_rmdir(const char * cDirectory, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_truncate(int FileHandler, int length, int *errornumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_isdir(const char * cDirectory, uint32_t *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_GetFileStats(const char *cFileName, FSTATS * StatBuffer, uint32_t *errornumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}
