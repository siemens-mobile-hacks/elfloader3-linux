#include "swi.h"
#include "elfloader/loader.h"

void loader_swi_stub(int swi) {
	fprintf(stderr, "Unk swi 0x%04X (%d) func called\n", swi, swi);
	abort();
}

int *loader_library_impl() {
	return (int *) switab_functions;
}

void SWI_elfclose(Elf32_Exec *ex) {
	loader_elf_close(ex);
}

void SWI_ShowMSG(int flag, int msg) {
	fprintf(stderr, "ShowMSG: %s\n", reinterpret_cast<char *>(msg));
}

void loader_init_switab() {
	// UI
	switab_functions[0x0148]	= (void *) SWI_ShowMSG;
	
	// LIBC
	switab_functions[0x0113]	= (void *) SWI_strpbrk;
	switab_functions[0x0113]	= (void *) SWI_strcspn;
	switab_functions[0x0114]	= (void *) SWI_strncat;
	switab_functions[0x0115]	= (void *) SWI_strncmp;
	switab_functions[0x0116]	= (void *) SWI_strncpy;
	switab_functions[0x0117]	= (void *) SWI_strrchr;
	switab_functions[0x0118]	= (void *) SWI_strstr;
	switab_functions[0x0119]	= (void *) SWI_strtol;
	switab_functions[0x011A]	= (void *) SWI_strtoul;
	switab_functions[0x011C]	= (void *) SWI_memcmp;
	switab_functions[0x011D]	= (void *) SWI_zeromem;
	switab_functions[0x011E]	= (void *) SWI_memcpy;
	switab_functions[0x0132]	= (void *) SWI_memmove;
	switab_functions[0x0162]	= (void *) SWI_setjmp;
	switab_functions[0x011B]	= (void *) SWI_snprintf;
	switab_functions[0x00BB]	= (void *) SWI_memset;
	switab_functions[0x0092]	= (void *) SWI_calloc;
	switab_functions[0x0085]	= (void *) SWI_strcmpi;
	switab_functions[0x0054]	= (void *) SWI_StrToInt;
	switab_functions[0x0014]	= (void *) SWI_malloc;
	switab_functions[0x0015]	= (void *) SWI_free;
	switab_functions[0x0016]	= (void *) SWI_sprintf;
	switab_functions[0x0017]	= (void *) SWI_strcat;
	switab_functions[0x0018]	= (void *) SWI_strchr;
	switab_functions[0x0019]	= (void *) SWI_strcmp;
	switab_functions[0x001A]	= (void *) SWI_strcpy;
	switab_functions[0x001B]	= (void *) SWI_strlen;
	
	// Filesystem
	switab_functions[0x000A]	= (void *) SWI_open;
	switab_functions[0x000B]	= (void *) SWI_read;
	switab_functions[0x000C]	= (void *) SWI_write;
	switab_functions[0x000D]	= (void *) SWI_close;
	switab_functions[0x000E]	= (void *) SWI_flush;
	switab_functions[0x000F]	= (void *) SWI_lseek;
	switab_functions[0x0010]	= (void *) SWI_mkdir;
	switab_functions[0x0012]	= (void *) SWI_GetFileAttrib;
	switab_functions[0x0013]	= (void *) SWI_SetFileAttrib;
	switab_functions[0x003C]	= (void *) SWI_setfilesize;
	switab_functions[0x006B]	= (void *) SWI_FindFirstFile;
	switab_functions[0x006C]	= (void *) SWI_FindNextFile;
	switab_functions[0x006D]	= (void *) SWI_FindClose;
	switab_functions[0x008E]	= (void *) SWI_fmove;
	switab_functions[0x008F]	= (void *) SWI_rmdir;
	switab_functions[0x0090]	= (void *) SWI_truncate;
	switab_functions[0x0091]	= (void *) SWI_isdir;
	switab_functions[0x0084]	= (void *) SWI_GetFileStats;
	
	// Date && Time
	switab_functions[0x00B4]	= (void *) SWI_GetDateTime;
	switab_functions[0x00B5]	= (void *) SWI_GetWeek;
	switab_functions[0x0230]	= (void *) SWI_GetTimeZoneShift;
	
	// Wide String
	switab_functions[0x011F]	= (void *) SWI_wstrcpy,
	switab_functions[0x0120]	= (void *) SWI_wstrncpy,
	switab_functions[0x0121]	= (void *) SWI_wstrcat,
	switab_functions[0x0122]	= (void *) SWI_wstrncat,
	switab_functions[0x0123]	= (void *) SWI_wstrlen,
	switab_functions[0x0124]	= (void *) SWI_wsprintf,
	switab_functions[0x0125]	= (void *) SWI_AllocWS,
	switab_functions[0x0126]	= (void *) SWI_CutWSTR,
	switab_functions[0x0127]	= (void *) SWI_CreateLocalWS,
	switab_functions[0x0128]	= (void *) SWI_CreateWS,
	switab_functions[0x0129]	= (void *) SWI_FreeWS,
	switab_functions[0x016C]	= (void *) SWI_str_2ws,
	switab_functions[0x02E2]	= (void *) SWI_wstrcmp,
	switab_functions[0x0214]	= (void *) SWI_wstrcpybypos,
	switab_functions[0x0215]	= (void *) SWI_wsRemoveChars,
	switab_functions[0x01E2]	= (void *) SWI_ws_2utf8,
	switab_functions[0x01E3]	= (void *) SWI_utf8_2ws,
	switab_functions[0x01DC]	= (void *) SWI_wstrchr,
	switab_functions[0x01DD]	= (void *) SWI_wstrrchr,
	switab_functions[0x001C]	= (void *) SWI_wsAppendChar,
	switab_functions[0x001D]	= (void *) SWI_wsInsertChar,
	switab_functions[0x0216]	= (void *) SWI_wstrcatprintf,
	
	// Locking
	switab_functions[0x0146]	= (void *) SWI_LockSched,
	switab_functions[0x0147]	= (void *) SWI_UnlockSched,
	switab_functions[0x0190]	= (void *) SWI_MutexCreate,
	switab_functions[0x0191]	= (void *) SWI_MutexDestroy,
	switab_functions[0x0192]	= (void *) SWI_MutexLock,
	switab_functions[0x0193]	= (void *) SWI_MutexLockEx,
	switab_functions[0x0194]	= (void *) SWI_MutexUnlock,
	
	// Other
	switab_functions[0x2EE]		= (void *) SWI_elfclose;
	
	// RAM
	switab_functions[0x822F - 0x8000]	= (void *) SWI_RamDateTimeSettings();
}
