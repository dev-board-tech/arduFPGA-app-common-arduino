#if !defined(ESP32)

#include <string.h>
#include "../inc/integer.h"
#include "../inc/fattime.h"
//#include "api/rtc_def.h"
//#include "api/rtc_api.h"

DWORD get_fattime(void) {
	/*rtc_t calendar;
	memset(&calendar, 0, sizeof(rtc_t));
	//rtc_get_calendar(&calendar);
	return ((DWORD) ((((calendar.Year + 2000) - 1980) & 0x7F)) << 25
			| (DWORD) (calendar.Month & 0x0F) << 21
			| (DWORD) (calendar.Date & 0x1F) << 16)
			| (DWORD) (calendar.Hours & 0x1F) << 11
			| (DWORD) (calendar.Minutes & 0x3F) << 5
			| (DWORD) (calendar.Seconds & 0x3F);*/
	return ((DWORD) ((((22 + 2000) - 1980) & 0x7F)) << 25
			| (DWORD) (1 & 0x0F) << 21
			| (DWORD) (1 & 0x1F) << 16)
			| (DWORD) (0 & 0x1F) << 11
			| (DWORD) (0 & 0x3F) << 5
			| (DWORD) (0 & 0x3F);
}

#endif
