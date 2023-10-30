#include "../swi.h"
#include "../SieFs.h"

#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

int SWI_open(const char *path, unsigned int f, unsigned int m, unsigned int *errp) {
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

int SWI_read(int fd, void *buff, int count, unsigned int *errp) {
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

int SWI_write(int fd, const void *buff, int count, unsigned int *errp) {
	int ret = write(fd, buff, count);
	if (ret < 0) {
		perror("[SWI] write");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

int SWI_close(int fd, unsigned int *errp) {
	int ret = close(fd);
	// fprintf(stderr, "close(%d) = %d\n", fd, ret);
	if (ret < 0) {
		perror("[SWI] close");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

int SWI_flush(int fd, unsigned int *errp) {
	int ret = fsync(fd);
	if (ret < 0) {
		perror("[SWI] flush");
		if (errp)
			*errp = 1;
	} else {
		if (errp)
			*errp = 0;
	}
	return ret;
}

long SWI_lseek(int fd, unsigned int offset, unsigned int origin, unsigned int *errp, unsigned int *errp2) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_mkdir(const char * cFileName, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_GetFileAttrib(const char *cFileName, unsigned char *cAttribute, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_SetFileAttrib(const char *cFileName, unsigned char cAttribute, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_setfilesize(int FileHandler, unsigned int iNewFileSize, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_FindFirstFile(DIR_ENTRY *DIRENTRY, const char *mask,unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_FindNextFile(DIR_ENTRY *DIRENTRY,unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_FindClose(DIR_ENTRY *DIRENTRY,unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_fmove(const char * SourceFileName, const char * DestFileName, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_rmdir(const char * cDirectory, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_truncate(int FileHandler, int length, int *errornumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_isdir(const char * cDirectory, unsigned int *ErrorNumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_GetFileStats(const char *cFileName, FSTATS * StatBuffer, unsigned int *errornumber) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}
