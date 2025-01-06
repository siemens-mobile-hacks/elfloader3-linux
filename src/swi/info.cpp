#include "src/swi/info.h"

#include <spdlog/spdlog.h>
#include <stdint.h>
#include <swilib.h>

static int missed_events_cnt = 0;
static uint16_t battery_capacity = 99;
static uint16_t battery_state = BAT_STATE_DISCHARGING;
static char ringtone_status = 1;
static char is_standby_mode = 0;
static char is_shutdown_mode = 0;
static char is_sim_present = 0;
static char is_camera_lighter_on = 0;
static void *current_appointment = NULL;
static char is_camera_running = 0;
static int total_perm_heap_size = 16 * 1024 * 1024;
static int total_temp_heap_size = 8 * 1024 * 1024;
static char is_file_cache = 0;
static char bt_device_name[] = "Siemens";
static char extended_camera_state = 0;
static char input_language = 0;
static char display_language = 0;
static char is_bt_connected = 0;
static char is_bt_handset_connected = 0;

std::string getPlatformName() {
#if defined(ELKA)
	return "ELKA";
#elif defined(NEWSGOLD)
	return "NSG";
#elif defined(X75)
	return "X75";
#else
	return "SG";
#endif
}

int GetAkku(int param1, int param2) {
	if (param1 == 1 && param2 == 3) {
		return 2970; // 25.5C
	} else if (param1 == 0 && param2 == 9) {
		return 4125; // 4.125mV
	} else {
		spdlog::debug("{}({}, {}): not implemented!", __func__, param1, param2);
	}
	return -1;
}

uint32_t GetMMIMemUsed() {
	return total_temp_heap_size; // dummy
}

int GetFreeRamAvail() {
	return total_perm_heap_size; // dummy
}

void *GetMissedEventsPtr() {
	return &missed_events_cnt;
}

char *GetCurrentLocale() {
	return (char *) "en"; // dummy
}

char GetAkkuCapacity() {
	return battery_capacity; // dummy
}

uint16_t *RamCap() {
	return &battery_capacity;
}

uint16_t *RamLS() {
	return &battery_state;
}

char *RamRingtoneStatus() {
	return &ringtone_status;
}

void *RamAccPoint(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamIsStandby() {
	return &is_standby_mode;
}

char *RamIsShutdown() {
	return &is_shutdown_mode;
}

char *RamIsSimPresent() {
	return &is_sim_present;
}

char *Get_Phone_Info(int index) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *RamIsCameraLighterOn() {
	return &is_camera_lighter_on;
}

char IsSliderClosed() {
	return 0;
}

void *RamActiveAppointment() {
	return &current_appointment;
}

int GetPeripheryState(int device, int check) {
	return 1;
}

char *RamIsRunCamera() {
	return &is_camera_running;
}

int BT_GetLocVisibility() {
	return 0;
}

void *RamIsFileCache() {
	return &is_file_cache;
}

int *RamTotalHeapSize() {
	return &total_perm_heap_size;
}

int *RamTemporaryHeapSize() {
	return &total_temp_heap_size;
}

const char *RamBluetoothDeviceName() {
	return bt_device_name;
}

char *RamExtendedCameraState() {
	return &extended_camera_state;
}

char *RamInputLanguage() {
	return &input_language;
}

char *RamDisplayLanguage() {
	return &display_language;
}

char *RamIsBTConnected() {
	return &is_bt_connected;
}

char *RamIsBTHeadsetConnected() {
	return &is_bt_handset_connected;
}
