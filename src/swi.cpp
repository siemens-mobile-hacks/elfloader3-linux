#include "swi.h"
#include "charset.h"
#include "elfloader/loader.h"
#include "elfloader/env.h"

void loader_swi_stub(int swi) {
	fprintf(stderr, "Unk swi 0x%04X (%d) func called\n", swi, swi);
	abort();
}

void loader_subproc_impl(void *func, void *p1) {
	SUBPROC(func, (int) p1);
}

int *loader_library_impl() {
	return (int *) switab_functions;
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
	switab_functions[0x005F]	= (void *) SYS_GetFreeRamAvail;
	
	switab_functions[0x80C8 - 0x8000]	= (void *) SYS_RamCap();
	
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
	switab_functions[0x0085]	= (void *) SWI_strcmpi;
	switab_functions[0x0054]	= (void *) SWI_StrToInt;
	switab_functions[0x0092]	= (void *) calloc;
	switab_functions[0x00BA]	= (void *) realloc;
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
	switab_functions[0x016B]	= (void *) FS_unlink;
	
	// Date && Time
	switab_functions[0x00B4]	= (void *) GetDateTime;
	switab_functions[0x00B5]	= (void *) GetWeek;
	switab_functions[0x0230]	= (void *) GetTimeZoneShift;
	
	switab_functions[0x822F - 0x8000]	= (void *) RamDateTimeSettings();
	
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
	switab_functions[0x00A3]	= (void *) ws_2str;
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
	switab_functions[0x0107]	= (void *) CSM_Create;
	switab_functions[0x0108]	= (void *) FindCSMbyID;
	switab_functions[0x0109]	= (void *) DoIDLE;
	switab_functions[0x0142]	= (void *) FindCSM;
	switab_functions[0x01FC]	= (void *) CloseCSM;
	
	switab_functions[0x8106 - 0x8000]	= (void *) CSM_root();
	
	// GUI
	switab_functions[0x014D]	= (void *) GUI_GetFocusedTop;
	switab_functions[0x0135]	= (void *) GUI_IsOnTop;
	switab_functions[0x0136]	= (void *) GUI_Create_ID;
	switab_functions[0x0137]	= (void *) GUI_Create;
	switab_functions[0x0138]	= (void *) GUI_Create_30or2;
	switab_functions[0x0139]	= (void *) GUI_CreateWithDummyCSM;
	switab_functions[0x013A]	= (void *) GUI_CreateWithDummyCSM_30or2;
	switab_functions[0x013B]	= (void *) GUI_GeneralFuncF1;
	switab_functions[0x013C]	= (void *) GUI_GeneralFuncF0;
	switab_functions[0x013D]	= (void *) GUI_GeneralFunc_flag1;
	switab_functions[0x013E]	= (void *) GUI_GeneralFunc_flag0;
	switab_functions[0x013F]	= (void *) GUI_DirectRedrawGUI;
	switab_functions[0x0140]	= (void *) GUI_DirectRedrawGUI_ID;
	switab_functions[0x0141]	= (void *) GUI_PendedRedrawGUI;
	switab_functions[0x0172]	= (void *) GUI_REDRAW;
	switab_functions[0x014C]	= (void *) GUI_DrawString;
	switab_functions[0x0150]	= (void *) GUI_DrawRoundedFrame;
	switab_functions[0x01B3]	= (void *) GUI_DrawLine;
	switab_functions[0x01B4]	= (void *) GUI_DrawRectangle;
	switab_functions[0x0202]	= (void *) GUI_DrawTriangle;
	switab_functions[0x0203]	= (void *) GUI_DrawPixel;
	switab_functions[0x0204]	= (void *) GUI_DrawArc;
	switab_functions[0x0023]	= (void *) GUI_DrawImg;
	switab_functions[0x0025]	= (void *) GUI_DrawCanvas;
	switab_functions[0x0026]	= (void *) GUI_DrawImgBW;
	switab_functions[0x012A]	= (void *) GUI_DrawObject;
	switab_functions[0x014A]	= (void *) GUI_FreeDrawObject;
	switab_functions[0x014B]	= (void *) GUI_DrawObjectSetColor;
	
	switab_functions[0x01FD]	= (void *) GUI_SetProp2Rect;			// type=0x0
	switab_functions[0x0149]	= (void *) GUI_SetProp2Text;			// type=0x1
	switab_functions[0x0205]	= (void *) GUI_SetProp2RectEx;			// type=0x4
	switab_functions[0x0201]	= (void *) GUI_SetProp2ImageOrCanvas;	// type=0x5
	switab_functions[0x0151]	= (void *) GUI_SetProp2Image;			// type=0x5
	switab_functions[0x038C]	= (void *) GUI_SetProp2Line;			// type=0xF
	switab_functions[0x038E]	= (void *) GUI_SetProp2Arc;				// type=0x13
	switab_functions[0x038D]	= (void *) GUI_SetProp2Triangle;		// type=0x15
	switab_functions[0x0386]	= (void *) GUI_SetProp2EImage;			// type=0x17
	
	switab_functions[0x012E]	= (void *) GUI_GetPaletteAdrByColorIndex;
	switab_functions[0x012F]	= (void *) GUI_GetRGBcolor;
	switab_functions[0x0130]	= (void *) GUI_GetRGBbyPaletteAdr;
	switab_functions[0x0131]	= (void *) GUI_SetColor;
	switab_functions[0x017D]	= (void *) GUI_SetIDLETMR;
	switab_functions[0x017E]	= (void *) GUI_RestartIDLETMR;
	switab_functions[0x017F]	= (void *) GUI_DisableIDLETMR;
	switab_functions[0x023B]	= (void *) GUI_DisableIconBar;
	switab_functions[0x0027]	= (void *) GUI_AddIconToIconBar;
	switab_functions[0x0133]	= (void *) GUI_StoreXYWHtoRECT;
	switab_functions[0x0134]	= (void *) GUI_StoreXYXYtoRECT;
	switab_functions[0x01C5]	= (void *) GUI_GetFontYSIZE;
	switab_functions[0x01E1]	= (void *) GUI_GetSymbolWidth;
	switab_functions[0x0208]	= (void *) GUI_GetStringWidth;
	switab_functions[0x0388]	= (void *) GUI_SetDepthBuffer;
	switab_functions[0x0389]	= (void *) GUI_SetDepthBufferOnLCDLAYER;
	switab_functions[0x001E]	= (void *) GUI_GetPITaddr;
	switab_functions[0x0021]	= (void *) GUI_GetImgHeight;
	switab_functions[0x0022]	= (void *) GUI_GetImgWidth;
	
	switab_functions[0x8188 - 0x8000]	= (void *) GUI_ScreenW();
	switab_functions[0x8189 - 0x8000]	= (void *) GUI_ScreenH();
	switab_functions[0x818A - 0x8000]	= (void *) GUI_HeaderH();
	switab_functions[0x818B - 0x8000]	= (void *) GUI_SoftkeyH();
	switab_functions[0x80E0 - 0x8000]	= (void *) GUI_RamScreenBuffer();
	switab_functions[0x80F5 - 0x8000]	= (void *) GUI_RamMainLCDLayer();
	switab_functions[0x80F6 - 0x8000]	= (void *) GUI_RamMMILCDLayer();
	
	// SettingsAE
	switab_functions[0x02E4]	= (void *) SettingsAE_Update_ws;
	switab_functions[0x02E5]	= (void *) SettingsAE_Read_ws;
	switab_functions[0x02E6]	= (void *) SettingsAE_SetFlag;
	switab_functions[0x02E7]	= (void *) SettingsAE_GetFlag;
	switab_functions[0x02E8]	= (void *) SettingsAE_Update;
	switab_functions[0x02E9]	= (void *) SettingsAE_Read;
	switab_functions[0x02EA]	= (void *) SettingsAE_GetEntryList;
	switab_functions[0x02EB]	= (void *) SettingsAE_RemoveEntry;
	
	// Obs
	switab_functions[0x0259]	= (void *) Obs_CreateObject;
	switab_functions[0x025A]	= (void *) Obs_DestroyObject;
	switab_functions[0x025B]	= (void *) Obs_SetInput_File;
	switab_functions[0x025C]	= (void *) Obs_GetInputImageSize;
	switab_functions[0x025D]	= (void *) Obs_SetOutputImageSize;
	switab_functions[0x025E]	= (void *) Obs_Start;
	switab_functions[0x025F]	= (void *) Obs_Output_GetPictstruct;
	switab_functions[0x0260]	= (void *) Obs_Graphics_SetClipping;
	switab_functions[0x0261]	= (void *) Obs_SetRotation;
	switab_functions[0x0262]	= (void *) Obs_GetInfo;
	switab_functions[0x0263]	= (void *) Obs_SetScaling;
	switab_functions[0x0264]	= (void *) Obs_TranslateMessageGBS;
	switab_functions[0x0265]	= (void *) Obs_Pause;
	switab_functions[0x0266]	= (void *) Obs_Resume;
	switab_functions[0x0267]	= (void *) Obs_Stop;
	switab_functions[0x0268]	= (void *) Obs_Prepare;
	switab_functions[0x0269]	= (void *) Obs_SetRenderOffset;
	switab_functions[0x026A]	= (void *) Obs_SetPosition;
	switab_functions[0x026B]	= (void *) Obs_Mam_SetPurpose;
	switab_functions[0x026C]	= (void *) Obs_Sound_SetVolumeEx;
	switab_functions[0x026D]	= (void *) Obs_Sound_GetVolume;
	switab_functions[0x026E]	= (void *) Obs_Sound_SetPurpose;
	
	// GBS
	switab_functions[0x004D]	= (void *) GBS_StartTimerProc;
	switab_functions[0x0143]	= (void *) GBS_StartTimer;
	switab_functions[0x0144]	= (void *) GBS_StopTimer;
	switab_functions[0x01A6]	= (void *) GBS_IsTimerProcessing;
	switab_functions[0x0105]	= (void *) GBS_CreateProc;
	switab_functions[0x0145]	= (void *) GBS_GetCurCepid;
	switab_functions[0x0154]	= (void *) GBS_KillProc;
	switab_functions[0x0104]	= (void *) GBS_PendMessage;
	switab_functions[0x0100]	= (void *) GBS_SendMessage;
	switab_functions[0x0101]	= (void *) GBS_ReciveMessage;
	switab_functions[0x0102]	= (void *) GBS_AcceptMessage;
	switab_functions[0x0103]	= (void *) GBS_ClearMessage;
	switab_functions[0x0153]	= (void *) GBS_RecActDstMessage;
	switab_functions[0x0185]	= (void *) GBS_WaitForMsg;
	switab_functions[0x018C]	= (void *) GBS_DelTimer;
	
	// Images
	switab_functions[0x0258]	= (void *) IMG_CalcBitmapSize;
	switab_functions[0x01E9]	= (void *) IMG_CreateIMGHDRFromPngFile,
	
	// Explorer
	switab_functions[0x02E1]	= (void *) GetExtUidByFileName_ws;
	
	// Helper
	switab_functions[0x0171]	= (void *) SUBPROC;
	
	switab_functions[0x81B9 - 0x8000]	= (void *) Helper_PngTop();
	switab_functions[0x81BA - 0x8000]	= (void *) Helper_LibTop();
	
	// Other
	switab_functions[0x2EE]		= (void *) loader_elf_close;
	switab_functions[0x2F2]		= (void *) loader_setenv;
	switab_functions[0x2F3]		= (void *) loader_unsetenv;
	switab_functions[0x2F4]		= (void *) loader_getenv;
	switab_functions[0x2F5]		= (void *) loader_clearenv;
}
