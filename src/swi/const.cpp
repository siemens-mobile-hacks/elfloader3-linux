#include <stdint.h>
#include <swilib/const.h>

int LP_MonthsShort() {
#if defined(ELKA)
	return 4669;
#elif defined(NEWSGOLD)
	return 4669;
#elif defined(X75)
	return 4877;
#else
	return 4759;
#endif
}

int PIT_Pic_Big() {
#if defined(ELKA)
	return 616;
#elif defined(NEWSGOLD)
	return 608;
#elif defined(X75)
	return 1279;
#else
	return 1238;
#endif
}

int PIT_Pic_Little() {
#if defined(ELKA)
	return 2096;
#elif defined(NEWSGOLD)
	return 2096;
#elif defined(X75)
	return 1146;
#else
	return 1107;
#endif
}

int PIT_Pic_Extra() {
#if defined(ELKA)
	return 2048;
#elif defined(NEWSGOLD)
	return 2048;
#elif defined(X75)
	return 1293;
#else
	return 1376;
#endif
}

int PIT_Pic_Profiles() {
#if defined(ELKA)
	return 2064;
#elif defined(NEWSGOLD)
	return 2064;
#elif defined(X75)
	return 1381;
#else
	return 1392;
#endif
}

int PIT_Pic_Vibra() {
#if defined(ELKA)
	return 2072;
#elif defined(NEWSGOLD)
	return 2072;
#elif defined(X75)
	return 787;
#else
	return 763;
#endif
}

int PIT_Pic_Call() {
#if defined(ELKA)
	return 0xFFFFFFFF;
#elif defined(NEWSGOLD)
	return 0xFFFFFFFF;
#elif defined(X75)
	return 1206;
#else
	return 1024;
#endif
}

int PIT_Pic_AdvNet() {
#if defined(ELKA)
	return 2048;
#elif defined(NEWSGOLD)
	return 2048;
#elif defined(X75)
	return 1388;
#else
	return 1408;
#endif
}

int LP_MonthsLong() {
#if defined(ELKA)
	return 4407;
#elif defined(NEWSGOLD)
	return 4407;
#elif defined(X75)
	return 4603;
#else
	return 4491;
#endif
}

int LP_Weekdays() {
#if defined(ELKA)
	return 1003;
#elif defined(NEWSGOLD)
	return 1003;
#elif defined(X75)
	return 1068;
#else
	return 1024;
#endif
}

int PIT_Pic_Calendar() {
#if defined(ELKA)
	return 1456;
#elif defined(NEWSGOLD)
	return 1456;
#elif defined(X75)
	return 1405;
#else
	return 1440;
#endif
}

int PIT_Pic_AccuGraph() {
#if defined(ELKA)
	return 0xFFFFFFFF;
#elif defined(NEWSGOLD)
	return 4608;
#elif defined(X75)
	return 1367;
#else
	return 1456;
#endif
}

int PIT_Pic_Lani() {
#if defined(ELKA)
	return 0xFFFFFFFF;
#elif defined(NEWSGOLD)
	return 0xFFFFFFFF;
#elif defined(X75)
	return 2700053993;
#else
	return 1413;
#endif
}
