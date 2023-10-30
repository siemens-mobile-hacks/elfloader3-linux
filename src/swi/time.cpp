#include "../swi.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

static TDateTimeSettings time_settings = {
	.timeZone = 123
};

TDateTimeSettings *SWI_RamDateTimeSettings(void) {
	return &time_settings;
}

void SWI_GetDateTime(TDate *d, TTime *t) {
	time_t now = time(NULL);
	struct tm local_tm = {};
	
	localtime_r(&now, &local_tm);
	
	if (d) {
		d->year = local_tm.tm_year + 1900;
		d->month = local_tm.tm_mon + 1;
		d->day = local_tm.tm_mday;
	}
	
	if (t) {
		t->hour = local_tm.tm_hour;
		t->min = local_tm.tm_min;
		t->sec = local_tm.tm_sec;
		t->millisec = 0;
	}
	
}

char SWI_GetWeek(TDate *param1) {
	fprintf(stderr, "%s not implemented!\n", __func__);
	abort();
	return 0;
}

int SWI_GetTimeZoneShift(TDate *d, TTime *t, int timeZone) {
	struct tm local_tm = {};
	local_tm.tm_year = d->year - 1900;
	local_tm.tm_mon = d->month - 1;
	local_tm.tm_mday = d->day;
	
	local_tm.tm_hour = t->hour;
	local_tm.tm_min = t->min;
	local_tm.tm_sec = t->sec;
	
	time_t unix_timestamp = mktime(&local_tm);
	localtime_r(&unix_timestamp, &local_tm);
	
	return -local_tm.tm_gmtoff / 60;
}
