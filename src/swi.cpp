#include "swi.h"
#include "charset.h"
#include "elfloader/loader.h"

void loader_swi_stub(int swi) {
	fprintf(stderr, "Unk swi 0x%04X (%d) func called\n", swi, swi);
	abort();
}

void loader_subproc_impl(void *func, void *p1) {
	fprintf(stderr, "SUBPORC!\n");
	abort();
}

int *loader_library_impl() {
	return (int *) switab_functions;
}

void Sie_elfclose(Elf32_Exec *ex) {
	loader_elf_close(ex);
}

void Sie_ShowMSG(int flag, int msg_id) {
	const char *msg = reinterpret_cast<char *>(msg_id);
	
	size_t len = strlen(msg);
	char *msg_utf8 = new char[len * 4 + 1];
	
	size_t msg_utf8_len = cp1251_to_utf8(msg, len, msg_utf8, len * 4);
	msg_utf8[msg_utf8_len] = 0;
	
	fprintf(stderr, "ShowMSG: %s\n", msg_utf8);
	
	delete msg_utf8;
}

void loader_init_switab() {
	// UI
	switab_functions[0x0148]	= (void *) Sie_ShowMSG;
	
	// System
	switab_functions[0x005F]	= (void *) GetFreeRamAvail,
	
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
	switab_functions[0x0014]	= (void *) malloc;
	switab_functions[0x0015]	= (void *) free;
	switab_functions[0x0016]	= (void *) SWI_sprintf;
	switab_functions[0x0017]	= (void *) SWI_strcat;
	switab_functions[0x0018]	= (void *) SWI_strchr;
	switab_functions[0x0019]	= (void *) SWI_strcmp;
	switab_functions[0x001A]	= (void *) SWI_strcpy;
	switab_functions[0x001B]	= (void *) SWI_strlen;
	
	// Filesystem
	switab_functions[0x000A]	= (void *) FS_open;
	switab_functions[0x000B]	= (void *) FS_read;
	switab_functions[0x000C]	= (void *) FS_write;
	switab_functions[0x000D]	= (void *) FS_close;
	switab_functions[0x000E]	= (void *) FS_flush;
	switab_functions[0x000F]	= (void *) FS_lseek;
	switab_functions[0x0010]	= (void *) FS_mkdir;
	switab_functions[0x0012]	= (void *) FS_GetFileAttrib;
	switab_functions[0x0013]	= (void *) FS_SetFileAttrib;
	switab_functions[0x003C]	= (void *) FS_setfilesize;
	switab_functions[0x006B]	= (void *) FS_FindFirstFile;
	switab_functions[0x006C]	= (void *) FS_FindNextFile;
	switab_functions[0x006D]	= (void *) FS_FindClose;
	switab_functions[0x008E]	= (void *) FS_fmove;
	switab_functions[0x008F]	= (void *) FS_rmdir;
	switab_functions[0x0090]	= (void *) FS_truncate;
	switab_functions[0x0091]	= (void *) FS_isdir;
	switab_functions[0x0084]	= (void *) FS_GetFileStats;
	
	// Date && Time
	switab_functions[0x00B4]	= (void *) GetDateTime;
	switab_functions[0x00B5]	= (void *) GetWeek;
	switab_functions[0x0230]	= (void *) GetTimeZoneShift;
	
	// Wide String
	switab_functions[0x011F]	= (void *) wstrcpy;
	switab_functions[0x0120]	= (void *) wstrncpy;
	switab_functions[0x0121]	= (void *) wstrcat;
	switab_functions[0x0122]	= (void *) wstrncat;
	switab_functions[0x0123]	= (void *) wstrlen;
	switab_functions[0x0124]	= (void *) wsprintf;
	switab_functions[0x0125]	= (void *) AllocWS;
	switab_functions[0x0126]	= (void *) CutWSTR;
	switab_functions[0x0127]	= (void *) CreateLocalWS;
	switab_functions[0x0128]	= (void *) CreateWS;
	switab_functions[0x0129]	= (void *) FreeWS;
	switab_functions[0x016C]	= (void *) str_2ws;
	switab_functions[0x02E2]	= (void *) wstrcmp;
	switab_functions[0x0214]	= (void *) wstrcpybypos;
	switab_functions[0x0215]	= (void *) wsRemoveChars;
	switab_functions[0x01E2]	= (void *) ws_2utf8;
	switab_functions[0x01E3]	= (void *) utf8_2ws;
	switab_functions[0x01DC]	= (void *) wstrchr;
	switab_functions[0x01DD]	= (void *) wstrrchr;
	switab_functions[0x001C]	= (void *) wsAppendChar;
	switab_functions[0x001D]	= (void *) wsInsertChar;
	switab_functions[0x0216]	= (void *) wstrcatprintf;
	
	// Locking
	switab_functions[0x0146]	= (void *) LockSched;
	switab_functions[0x0147]	= (void *) UnlockSched;
	switab_functions[0x0190]	= (void *) MutexCreate;
	switab_functions[0x0191]	= (void *) MutexDestroy;
	switab_functions[0x0192]	= (void *) MutexLock;
	switab_functions[0x0193]	= (void *) MutexLockEx;
	switab_functions[0x0194]	= (void *) MutexUnlock;
	
	// CSM
	switab_functions[0x0107]	= (void *) CreateCSM;
	switab_functions[0x0108]	= (void *) FindCSMbyID;
	switab_functions[0x0109]	= (void *) DoIDLE;
	switab_functions[0x0142]	= (void *) FindCSM;
	switab_functions[0x01FC]	= (void *) CloseCSM;
	
	// GUI
	switab_functions[0x014D]	= (void *) GUI_GetFocusedTop;
	switab_functions[0x0135]	= (void *) GUI_IsOnTop;
	switab_functions[0x0136]	= (void *) GUI_Create_ID;
	switab_functions[0x0137]	= (void *) GUI_Create;
	switab_functions[0x0138]	= (void *) GUI_Create_30or2;
	switab_functions[0x0139]	= (void *) GUI_CreateWithDummyCSM;
	switab_functions[0x013A]	= (void *) GUI_CreateWithDummyCSM_30or2;
	switab_functions[0x013B]	= (void *) GeneralFuncF1;
	switab_functions[0x013C]	= (void *) GeneralFuncF0;
	switab_functions[0x013D]	= (void *) GeneralFunc_flag1;
	switab_functions[0x013E]	= (void *) GeneralFunc_flag0;
	switab_functions[0x013F]	= (void *) GUI_DirectRedrawGUI;
	switab_functions[0x0140]	= (void *) GUI_DirectRedrawGUI_ID;
	switab_functions[0x0141]	= (void *) GUI_PendedRedrawGUI;
	switab_functions[0x014C]	= (void *) GUI_DrawString;
	switab_functions[0x0150]	= (void *) GUI_DrawRoundedFrame;
	switab_functions[0x01B3]	= (void *) GUI_DrawLine;
	switab_functions[0x01B4]	= (void *) GUI_DrawRectangle;
	switab_functions[0x012E]	= (void *) GUI_GetPaletteAdrByColorIndex;
	switab_functions[0x012F]	= (void *) GUI_GetRGBcolor;
	switab_functions[0x0130]	= (void *) GUI_GetRGBbyPaletteAdr;
	switab_functions[0x0131]	= (void *) GUI_SetColor;
	switab_functions[0x017D]	= (void *) GUI_SetIDLETMR,
	switab_functions[0x017E]	= (void *) GUI_RestartIDLETMR,
	switab_functions[0x017F]	= (void *) GUI_DisableIDLETMR,
	switab_functions[0x023B]	= (void *) GUI_DisableIconBar,
	switab_functions[0x0027]	= (void *) GUI_AddIconToIconBar,
	
	// Other
	switab_functions[0x2EE]		= (void *) Sie_elfclose;
	
	// RAM
	switab_functions[0x8106 - 0x8000]	= (void *) CSM_root();
	switab_functions[0x822F - 0x8000]	= (void *) RamDateTimeSettings();
	switab_functions[0x8188 - 0x8000]	= (void *) GUI_ScreenW();
	switab_functions[0x8189 - 0x8000]	= (void *) GUI_ScreenH();
	switab_functions[0x818A - 0x8000]	= (void *) GUI_HeaderH();
	switab_functions[0x818B - 0x8000]	= (void *) GUI_SoftkeyH();
}
