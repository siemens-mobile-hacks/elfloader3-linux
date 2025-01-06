#include <cstdint>
#include <time.h>
#include <swilib/date.h>

static char is_autotime_enabled = 0;
static TDateTimeSettings time_settings = {
	.yearNormBudd	= 1,
	.dateFormat		= 0, // DD.MM.YYYY
	.timeFormat		= 0, // 24h
	.timeZone		= 0, // UTC
	.isAutoTime1	= 0,
	.isAutoTime2	= 0,
};

void GetDateTime(struct TDate *date, struct TTime *time) {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts); // Get current time

	struct tm *local_time = localtime(&ts.tv_sec); // Convert to localtime structure

	date->year = local_time->tm_year + 1900;
	date->month = local_time->tm_mon + 1;
	date->day = local_time->tm_mday;

	time->hour = local_time->tm_hour;
	time->min = local_time->tm_min;
	time->sec = local_time->tm_sec;
	time->millisec = ts.tv_nsec / 1000000; // Convert nanoseconds to milliseconds
}

char GetWeek(const struct TDate *date) {
	struct tm timeinfo = {0};

	timeinfo.tm_year = date->year - 1900; // tm_year is years since 1900
	timeinfo.tm_mon = date->month - 1; // tm_mon is zero-based
	timeinfo.tm_mday = date->day;

	if (mktime(&timeinfo) == -1)
		return -1;

	// Calculate day of the week for Jan 1
	struct tm jan1 = {0};
	jan1.tm_year = date->year - 1900;
	jan1.tm_mon = 0; // January
	jan1.tm_mday = 1;
	mktime(&jan1);

	// Calculate week number according to ISO 8601
	int day_of_year = timeinfo.tm_yday + 1; // tm_yday starts at 0
	int week_number = (day_of_year - jan1.tm_wday + 10) / 7;

	return week_number;
}

char *RamIsAutoTimeEnabled(void) {
	return &is_autotime_enabled;
}

TDateTimeSettings *RamDateTimeSettings(void) {
	return &time_settings;
}

int GetTimeZoneShift(const TDate *date, const TTime *time, int timezone_id) {
	return 60 * timezone_id; // stub
}

int GetSecondsFromDateTime(int *seconds, const TDate *date, const TTime *time, const TDate *epoch) {
	struct tm timeinfo = {0};
	timeinfo.tm_year = date->year - 1900;
	timeinfo.tm_mon = date->month - 1;
	timeinfo.tm_mday = date->day;
	timeinfo.tm_hour = time->hour;
	timeinfo.tm_min = time->min;
	timeinfo.tm_sec = time->sec;

	time_t epoch_time = mktime(&timeinfo);
	if (epoch_time == -1)
		return -1;

	struct tm epoch_tm = {0};
	epoch_tm.tm_year = epoch->year - 1900;
	epoch_tm.tm_mon = epoch->month - 1;
	epoch_tm.tm_mday = epoch->day;

	time_t epoch_base = mktime(&epoch_tm);
	if (epoch_base == -1)
		return -1;

	*seconds = (int) difftime(epoch_time, epoch_base);
	return 0;
}

int GetSecondsFromDateTimeSince1997(int *seconds, const TDate *date, const TTime *time) {
	TDate epoch = { 1997, 1, 1 };
	return GetSecondsFromDateTime(seconds, date, time, &epoch);
}

int GetSecondsFromTime(const TTime *time) {
	return time->hour * 3600 + time->min * 60 + time->sec;
}

int GetDateTimeFromSeconds(const int *seconds, TDate *date, TTime *time, const TDate *epoch) {
	struct tm epoch_tm = {0};
	epoch_tm.tm_year = epoch->year - 1900;
	epoch_tm.tm_mon = epoch->month - 1;
	epoch_tm.tm_mday = epoch->day;

	time_t epoch_time = mktime(&epoch_tm);
	if (epoch_time == -1)
		return -1;

	time_t target_time = epoch_time + *seconds;
	struct tm *result_timeinfo = gmtime(&target_time);

	if (result_timeinfo == NULL)
		return -1;

	date->year = result_timeinfo->tm_year + 1900;
	date->month = result_timeinfo->tm_mon + 1;
	date->day = result_timeinfo->tm_mday;
	GetTimeFromSeconds(time, result_timeinfo->tm_hour * 3600 + result_timeinfo->tm_min * 60 + result_timeinfo->tm_sec);
	return 0;
}

int GetDateTimeFromSecondsSince1997(const int *seconds, TDate *date, TTime *time) {
	TDate epoch = { 1997, 1, 1 };
	return GetDateTimeFromSeconds(seconds, date, time, &epoch);
}

void GetTimeFromSeconds(TTime *time, int seconds) {
	time->hour = seconds / 3600;
	seconds %= 3600;
	time->min = seconds / 60;
	time->sec = seconds % 60;
	time->millisec = 0;
}

void InitDate(TDate *date, unsigned long year, uint8_t month, uint8_t day) {
	date->year = year;
	date->month = month;
	date->day = day;
}

void InitTime(TTime *time, uint8_t hour, uint8_t min, uint8_t sec, unsigned long millisec) {
	time->hour = hour;
	time->min = min;
	time->sec = sec;
	time->millisec = millisec;
}

int CmpDates(const TDate *date1, const TDate *date2) {
	if (date1->year != date2->year)
		return date1->year > date2->year ? 1 : -1;
	if (date1->month != date2->month)
		return date1->month > date2->month ? 1 : -1;
	return date1->day - date2->day;
}

int CmpTimes(const TTime *time1, const TTime *time2) {
	if (time1->hour != time2->hour)
		return time1->hour > time2->hour ? 1 : -1;
	if (time1->min != time2->min)
		return time1->min > time2->min ? 1 : -1;
	if (time1->sec != time2->sec)
		return time1->sec > time2->sec ? 1 : -1;
	return time1->millisec - time2->millisec;
}

int GetDaysFromMonth(const TDate *date) {
	struct tm timeinfo = {0};
	timeinfo.tm_year = date->year - 1900;
	timeinfo.tm_mon = date->month; // Set to next month
	timeinfo.tm_mday = 1; // First day of next month

	// mktime will normalize it to the proper month/day
	if (mktime(&timeinfo) == -1)
		return -1;

	timeinfo.tm_mday = 0; // Move to last day of the requested month
	return mktime(&timeinfo) == -1 ? -1 : timeinfo.tm_mday;
}

int IsLeapYear(const TDate *date) {
	struct tm timeinfo = {0};
	timeinfo.tm_year = date->year - 1900;
	timeinfo.tm_mon = 1; // February
	timeinfo.tm_mday = 29; // Check for leap day
	// Using mktime to check if Feb 29 is a valid date for the given year
	return mktime(&timeinfo) != -1;
}

void GetDate_ws(WSHDR *ws, const TDate *date, unsigned int unk) {
	wsprintf(ws, "%02d.%02d.%04d", date->day, date->month, date->year);
}

void GetTime_ws(WSHDR *ws, const TTime *time, unsigned int unk) {
	wsprintf(ws, "%02d:%02d", time->hour, time->min);
}
