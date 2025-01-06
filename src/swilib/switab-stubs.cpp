/* Auto-generated file!!! See @sie-js/swilib-tools! */
#include <spdlog/spdlog.h>
#include <swilib.h>
#include <swilib/openssl.h>
#include <swilib/nucleus.h>
#include <swilib/png.h>
#include <swilib/zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* swilib/misc.h */
long SetIllumination(uint8_t dev, unsigned long unk, uint16_t level, unsigned long delay) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SwitchPhoneOff() {
	spdlog::debug("{}: not implemented!", __func__);
}

void AlarmClockRing() {
	spdlog::debug("{}: not implemented!", __func__);
}

void SLI_SetState(uint8_t state) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GetVibraStatus(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int IsUnlocked(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void TempLightOn(int flags, int unk) {
	spdlog::debug("{}: not implemented!", __func__);
}

char *GetIlluminationDataTable() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void KeypressOn(int keycode) {
	spdlog::debug("{}: not implemented!", __func__);
}

void KeypressOff(int keycode) {
	spdlog::debug("{}: not implemented!", __func__);
}

void ShowMainMenu() {
	spdlog::debug("{}: not implemented!", __func__);
}

void ShowNativeMenu() {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetVibraStatus(int status) {
	spdlog::debug("{}: not implemented!", __func__);
}

const shortcut *GetShortcutRecordByName(char *shortcut_name) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

uint32_t SetVibration(uint32_t power) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

void RebootPhone() {
	spdlog::debug("{}: not implemented!", __func__);
}

void KbdLock() {
	spdlog::debug("{}: not implemented!", __func__);
}

void KbdUnlock() {
	spdlog::debug("{}: not implemented!", __func__);
}

char *RamAlarmClockState() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int *RamIsLocked(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

LIGHT_PARAM *RamScreenBrightness(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetIDLETMR(int timeout, int msg) {
	spdlog::debug("{}: not implemented!", __func__);
}

void RestartIDLETMR(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

void DisableIDLETMR(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

int Devmenu_Config_IsCheckboxOff(int checkbox_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int RefreshAlarmClock() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *RamIsAlarmClockAutorepeatOn() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamAlarmClockHours() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamAlarmClockMinutes() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetAlarmClockState(int state) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void StartNativeBrowserCore() {
	spdlog::debug("{}: not implemented!", __func__);
}

void IllumTimeRequest(int time_mode, int counter) {
	spdlog::debug("{}: not implemented!", __func__);
}

void IllumTimeRelease(int time_mode, int counter) {
	spdlog::debug("{}: not implemented!", __func__);
}

WSHDR *GetClipBoard(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void CopyTextToClipboard(const WSHDR *text, int pos, int len) {
	spdlog::debug("{}: not implemented!", __func__);
}

void ClearClipboard(char unk) {
	spdlog::debug("{}: not implemented!", __func__);
}

void IllumFilterSet(int flags, int unk) {
	spdlog::debug("{}: not implemented!", __func__);
}

/* swilib/idle.h */
void DrawMainScreen() {
	spdlog::debug("{}: not implemented!", __func__);
}

void DrawScreenSaver() {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsScreenSaver(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

SS_RAM *GetScreenSaverRAM(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void CloseScreensaver() {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsIdleUiOnTop(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

SS_RAM *RamScreenSaverCSM(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void DoIDLE(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GBS_Cmd_4209_RefreshScreensaver() {
#if defined(ELKA)
	return 0x6171;
#elif defined(NEWSGOLD)
	return 0x61E8;
#elif defined(X75)
	return 0xA1;
#else
	return 0xFFFFFFFF;
#endif
}

int ShowScreenSaverCodeProtection() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetScreenSaverType() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}


/* swilib/ui.h */
void AddIconToIconBar(int pic, short* num) {
	spdlog::debug("{}: not implemented!", __func__);
}

int MsgBoxError(int flags, int message) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int MsgBoxYesNo(int flags, int message, void (*callback)(int canceled)) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int MsgBoxOkCancel(int flags, int message, void (*callback)(int canceled)) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *RamMenuAnywhere() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int ShowMSG(int flags, int message) {
	const char *message_char = (const char *) message;
	spdlog::debug("ShowMSG: {}", message_char);
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *FindItemByID(GUI *gui, int id) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *GetDataOfItemByID(GUI *gui, int id) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int CreateMenu(int style, int zero, const MENU_DESC *menu, const HEADER_DESC *header, int selected_item, int items_count, void *user_pointer, int *to_remove) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

EDITQ *AllocEQueue(const void *malloc_func, const void *mfree_func) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

EDITCONTROL *PrepareEditControl(EDITCONTROL *ec) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void ConstructEditControl(EDITCONTROL *ec, int type, int flags, const WSHDR *title, int maxlen) {
	spdlog::debug("{}: not implemented!", __func__);
}

int AddEditControlToEditQend(EDITQ *eq, const EDITCONTROL *ec, const void *malloc_func) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreateInputTextDialog(const INPUTDIA_DESC *input_desc, const HEADER_DESC *header_desc, EDITQ *eq, int do_mfree, void *user_pointer) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SetSoftKey(void *gui, const SOFTKEY_DESC *softkey, int softkey_id) {
	spdlog::debug("{}: not implemented!", __func__);
}

int ExtractEditControl(void *gui, int n, EDITCONTROL *ec) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int StoreEditControl(void *gui, int n, const EDITCONTROL *ec) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int EDIT_GetItemNumInFocusedComboBox(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void EDIT_SetTextToFocused(void *gui, const WSHDR *value) {
	spdlog::debug("{}: not implemented!", __func__);
}

int EDIT_GetFocus(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int EDIT_GetUnFocus(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void ConstructComboBox(EDITCONTROL *ec, int type, int flags, WSHDR *title, int maxlen, int unk_zero, int items_count, int selected_item) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GetCurMenuItem(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SetMenuItemIcon(void *gui, int item_n, int icon) {
	spdlog::debug("{}: not implemented!", __func__);
}

void RefreshGUI(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

void *AllocMenuItem(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

WSHDR *AllocMenuWS(void *gui, int len) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetMenuItemIconArray(void *gui, void *item, const int *icons) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetMenuItemText(void *gui, void *item, WSHDR *text, int item_n) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_SetFocus(void *gui, int n) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetMenuItemCount(void *gui, int items_count) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetCursorToMenuItem(void *gui, int item_n) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_ExtractFocusedControl(void *gui, EDITCONTROL *ec) {
	spdlog::debug("{}: not implemented!", __func__);
}

int EDIT_StoreControlToFocused(void *gui, const EDITCONTROL *ec) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

EDITC_OPTIONS *PrepareEditCOptions(EDITC_OPTIONS *options) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetInvertToEditCOptions(EDITC_OPTIONS *options, int invert) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetFontToEditCOptions(EDITC_OPTIONS *options, int font) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetTextAttrToEditCOptions(EDITC_OPTIONS *options, int flags) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetPenColorToEditCOptions(EDITC_OPTIONS *options, int pen) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetBrushColorToEditCOptions(EDITC_OPTIONS *options, int brush) {
	spdlog::debug("{}: not implemented!", __func__);
}

void CopyOptionsToEditControl(EDITCONTROL *ec, const EDITC_OPTIONS *options) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_RemoveEditControl(void *gui, int n) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_InsertEditControl(void *gui, int n, const EDITCONTROL *ec) {
	spdlog::debug("{}: not implemented!", __func__);
}

int EDIT_GetCursorPos(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void EDIT_SetCursorPos(void *gui, int position) {
	spdlog::debug("{}: not implemented!", __func__);
}

int EDIT_IsMarkModeActive(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int EDIT_GetMarkedText(void *gui, WSHDR *selected_text) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int EDIT_SetTextToEditControl(void *gui, int n, const WSHDR *value) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *EDIT_GetUserPointer(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *MenuGetUserPointer(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int EDIT_OpenOptionMenuWithUserItems(void *gui, void (*itemhandler)(USR_MENU_ITEM *item), void *user_pointer, int to_add) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void Menu_SetItemCountDyn(void *gui, int items_count) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_SetDate(void *gui, int n, const TDate *date) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_SetTime(void *gui, int n, const TTime *time) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_GetDate(void *gui, int n, TDate *date) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EDIT_GetTime(void *gui, int n, TTime *time) {
	spdlog::debug("{}: not implemented!", __func__);
}

int ConstructEditTime(EDITCONTROL *ec, const TTime *time) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int ConstructEditDate(EDITCONTROL *ec, const TDate *date) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreateMultiLinesMenu(int style, int zero, const ML_MENU_DESC *menu, const HEADER_DESC *header, int selected_item, int items_count) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void SetMLMenuItemText(void *gui, void *item, WSHDR *text1, WSHDR *text2, int item_n) {
	spdlog::debug("{}: not implemented!", __func__);
}

void *AllocMLMenuItem(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int EDIT_SetTextInvert(void *gui, int position, int len) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void EDIT_RemoveTextInvert(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
}

int EDIT_IsBusy(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreateRadioButtonList(void *desc, int num, int unk_zero0, int unk_zero1, char selected_item, void (*handler)(int), int flags) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *RamIconBar() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void DisableIconBar(int disable) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetMenuSearchCallBack(void *gui, const MenuSearchCallBack callback) {
	spdlog::debug("{}: not implemented!", __func__);
}

void MenuSetUserPointer(void *gui, void *user_pointer) {
	spdlog::debug("{}: not implemented!", __func__);
}

void UpdateMenuCursorItem(void *gui, int item_n) {
	spdlog::debug("{}: not implemented!", __func__);
}

void *GetMenuGUI(const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *GetMultiLinesMenuGUI(const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetHeaderToMenu(void *gui, const HEADER_DESC *header, const void *malloc_fn) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetMenuToGUI(void *gui, const void *menu) {
	spdlog::debug("{}: not implemented!", __func__);
}

int GetMenuItemCount(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreatePopupGUI(int flags, void *user_pointer, const POPUP_DESC *popup, int lgp_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int CreatePopupGUI_ws(int flags, void *user_pointer, const POPUP_DESC *popup, const WSHDR *message) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *GetPopupUserPointer(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *GetHeaderPointer(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetHeaderText(void *header, const WSHDR *title, const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetMenuSoftKey(void *gui, const SOFTKEY_DESC *softkey, int softkey_id) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetHeaderIcon(void *header, const int *icons, const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
}

void *GetTabGUI(void *malloc_fn, void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetGUIDefinition(void *gui, const void *definition) {
	spdlog::debug("{}: not implemented!", __func__);
}

int SetGUIToTabGUI(void *gui, int tab_n, const int *icons, void *content_gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SetCursorTab(void *gui, int tab_n) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int UpdateTab1stItem(void *gui, int unk_0) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetCursorTab(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *GetGuiByTab(void *gui, int tab_n) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void *TViewGetGUI(const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void TViewSetDefinition(void *gui, const TVIEW_DESC *tv) {
	spdlog::debug("{}: not implemented!", __func__);
}

void TViewSetText(void *gui, const WSHDR *text, const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
}

void TViewSetUserPointer(void *gui, void *user_pointer) {
	spdlog::debug("{}: not implemented!", __func__);
}

void *TViewGetUserPointer(void *gui) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetMenuItemIconIMGHDR(void *gui, void *item, IMGHDR *icon) {
	spdlog::debug("{}: not implemented!", __func__);
}

int ShowWaitBox(int flags, int message) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int ShowPBarBox(int flags, int message, PBAR_DESC *pbar_desc, int initial_value, WSHDR *initial_ws) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SetPBarValue(int gui_id, int value) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SetPBarText(int gui_id, WSHDR *ws) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *GetPBarProc3() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void SetHeaderScrollText(void *header, const WSHDR *title, const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
}

void SetHeaderExtraText(void *header, const WSHDR *ws, const void *malloc_fn, const void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
}

/* swilib/eeprom.h */
uint32_t GetEELiteMaxID() {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

uint32_t GetEEFullMaxID() {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

void *GetEELiteBlockAddress(short block) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetEELiteBlockSizeVersion(uint32_t block, int *size, char *version) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int EEFullGetBlockInfo(uint32_t block, int *size, char *version) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int StartWriteEELiteBlock(uint32_t block, uint32_t size, uint32_t version, int *null) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int FinishWriteEELiteBlock(uint32_t block, int *null) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int DeleteEELiteBlock(uint32_t block, int *null) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int WriteEELiteBlock(uint32_t block, void *buf, uint32_t offset, uint32_t size, int *null) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void EEFullReadBlock(int block, void *buf, int offset, int size, int cepid, int msg) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EEFullWriteBlock(int block, const void *buf, int offset, int size, int cepid, int msg) {
	spdlog::debug("{}: not implemented!", __func__);
}

int EEFullDeleteBlock(uint32_t block, int cepid, int msg) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int EEFullCreateBlock(uint32_t block, int size, int version, int cepid, int msg) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

/* swilib/sound.h */
void PlaySound(long volume, long cepid, long unk1, long tone_type, long unk2) {
	spdlog::debug("{}: not implemented!", __func__);
}

short SoundAE_PlayFileAsEvent(int event_id, int cepid, int msg_id, const WSHDR *folder, const WSHDR *file) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void PlaySoundLoop(int unk1, int unk2, int duration) {
	spdlog::debug("{}: not implemented!", __func__);
}

short PlayFile(int flags, const WSHDR *folder, const WSHDR *file, int cepid, int msgid, const PLAYFILE_OPT *sfo) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int IsPlayerOn() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int PlayMelodyInMem(char unk1, const void *buffer, int size, int cepid, int msg, int unk2) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int ChangeVolume(uint8_t volume) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetPlayStatus(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *RamVolumeLevel() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamMediaIsPlaying() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int PlayMelody_ChangeVolume(int handle, int volume) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int PlayMelody_StopPlayback(int handle) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int PlayMelody_PausePlayback(int handle) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int PlayMelody_ResumePlayBack(int handle) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int PlayMelody_SetPosition(int handle, int position) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *GetPlayObjById(int handle) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetPlayObjPosition(void *obj, int *position) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetPlayObjDuration(void *obj, int *duration) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Audio_GetCurrAudioPath() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *RamIsSoundRecordingActive() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamSoundRecordingQuality() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int AACC_AudioTransferReq(int oh, uint32_t type_and_conn) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int Audio_GetObjectHandle() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *RamAudioHook() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

/* swilib/telephony.h */
char GetProvider(WSHDR *name, int unk) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int HasNewSMS(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int HasFlashSMS(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void ReadNewSMS() {
	spdlog::debug("{}: not implemented!", __func__);
}

void ReadFlashSMS() {
	spdlog::debug("{}: not implemented!", __func__);
}

int GetMissedCallsCount(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetNetAccessMode() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void EndCall(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

void EndCallBusy(int param1, int param2) {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsDirectCallActive(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int IsCalling(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetDurationFromCurrentCall(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetTypeOfCall(int type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int IsIncognitoCall(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int ShowCallList(int list, int zero) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

RAMNET *RamNet() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamMissedCalls() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamMissedSMS() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamMissedMessage() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

RAMNET *RamNetOnline() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

uint16_t *GetCI() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void freeUSSD() {
	spdlog::debug("{}: not implemented!", __func__);
}

char GetTypeUSSD() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

uint32_t SetUSSD(const StructUSSDStr *req) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int MakeVoiceCall(const char *number, int unk1, int unk2) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *Get_CC_NC() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int SendSMS(WSHDR *text, const char *number, int cepid, int msg, int flag) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

PDU *IncommingPDU(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RAM_IMSI(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamServiceProviderName(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GSMTXT_Decode(WSHDR *result, void *pkt, int len, int encoding_type, malloc_func_t malloc_fn, mfree_func_t mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void FreeGSMTXTpkt(void *pkt) {
	spdlog::debug("{}: not implemented!", __func__);
}

void GetAllEmail() {
	spdlog::debug("{}: not implemented!", __func__);
}

void SendAutoUSSDack(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

char *RamLastCallNum() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetSMSState() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int isDecryption(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char *RamCallState() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int DeleteSMS(int index, short *del_cnt) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SetSmsStatus(int index, int status) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

SMS_DATA_ROOT *SmsDataRoot() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetSmsPosIndex(SMS_POS_INDEX_DATA *buf, int data_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetSubprovider(WSHDR* name) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetRoamingState() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

RAM_EMS_ADMIN *RAM_EMS_Admin() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

/* swilib/java.h */
void StartMidlet(char *midlet) {
	spdlog::debug("{}: not implemented!", __func__);
}

char *GetLastJavaApplication() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int IsNoJava() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int8_t *RamIsRunJava() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int runMidletLinkablely(const char *midlet, WSHDR *filename) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

/* swilib/gprs.h */
void RefreshGPRSTraffic() {
	spdlog::debug("{}: not implemented!", __func__);
}

int *GetGPRSTrafficPointer() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void ResetGPRSTraffic() {
	spdlog::debug("{}: not implemented!", __func__);
}

char IsGPRSEnabled() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char IsGPRSConnected() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int RegisterCepidForSocketWork(REGSOCKCEPID_DATA *reg) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int RequestLMANConnect(LMAN_DATA *data) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void GPRS_OnOff(int enable, int unk) {
	spdlog::debug("{}: not implemented!", __func__);
}

int ActivateDialUpProfile(uint32_t cepid, int profile_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetCurrentGPRSProfile(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetDisconnectTime(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void RegisterCepIdForCurProfile(uint32_t cepid, int profile_id, uint32_t is_gprs) {
	spdlog::debug("{}: not implemented!", __func__);
}

int SetCurrentGPRSProfile(int profile_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char IsGPRSAvailable() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

char IsEDGEAvailable() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

const char *RamActiveGPRSProfileName() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

/* swilib/player.h */
const WSHDR *GetLastAudioTrackFilename() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

uint32_t *RamMPlayer_CtrlCode() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void MEDIA_PLAYLAST() {
	spdlog::debug("{}: not implemented!", __func__);
}

int IsMediaPlayerInBackground() {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void CloseMediaPlayer() {
	spdlog::debug("{}: not implemented!", __func__);
}

char *RamMediaPlayerCurrentTrackFormat() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamMediaPlayerCurrentTrackFreq() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamMediaPlayerCurrentTrackChannels() {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

const WSHDR *GetLastAudioTrackDir() {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void MediaProc_LaunchLastPlayback() {
	spdlog::debug("{}: not implemented!", __func__);
}

/* swilib/addressbook.h */
int UnpackABentry(AB_UNPRES *entry, const void *data, int size, int flags) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int FreeUnpackABentry(AB_UNPRES *entry, void *mfree_fn) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetTypeOfAB_UNPRES_ITEM(int item_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

NativeAbData *AllocNativeAbData(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int FreeNativeAbData(NativeAbData *data) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetNativeAbDataType(NativeAbData *data, int index) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

WSHDR *GetNumFromNativeAbData(NativeAbData *data, int type, int unk0) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int GetNativeAbDataStatus(NativeAbData *data, int index) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int SetNumToNativeAbData(NativeAbData *data, int unk, const WSHDR *num) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int OpenNativeAddressbook(int type, int unk1, int unk2, NativeAbData *data) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int GetFilledNAbDataCount(NativeAbData *data) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

PKT_NUM *GetDataFromNAbData(NativeAbData *data, int index) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void UnpackNumPkt(const PKT_NUM *pkt, char *buf, int maxlen) {
	spdlog::debug("{}: not implemented!", __func__);
}

/* swilib/socket.h */
int bsd_socket(int domain, int type, int protocol) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_connect(int socket, const struct bsd_sockaddr *address, bsd_socklen_t address_len) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_bind(int socket, const struct bsd_sockaddr *address, bsd_socklen_t address_len) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_closesocket(int socket) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_shutdown(int socket, int how) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_recv(int socket, void *buf, int len, int flags) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_send(int socket, const void *buf, int len, int flags) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

uint16_t bsd_htons(uint16_t p) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

uint32_t bsd_htonl(uint32_t p) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int *bsd_socklasterr(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int bsd_available0(int socket) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int bsd_async_gethostbyname(const char *name, void *response, int *req_id) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

uint32_t bsd_str2ip(const char *ip) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

/* swilib/i2c.h */
int i2c_transfer(I2C_MSG *msg) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int i2c_receive(I2C_MSG *msg) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}
