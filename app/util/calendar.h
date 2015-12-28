/*
 * calendar.h
 *
 *  Created on: 2015Äê12ÔÂ17ÈÕ
 *      Author: Administrator
 */

#ifndef APP_UTIL_CALENDAR_H_
#define APP_UTIL_CALENDAR_H_

#include "c_types.h"

// calendar
struct calendar
{
	int hour;
	int min;
	int sec;
	//
	int  year;
	int  month;
	int  date;
	int  weekday;
};

uint8_t get_weekday(uint16 year,uint8 month,uint8 day);
uint32 get_rtc_counter(uint16 year,uint8 month,uint8 day,uint8 hour,uint8 min,uint8 sec);
uint32 get_day_counter(uint8 hour,uint8 min,uint8 sec);
void get_calendar(uint32 time_value, int time_zone, struct calendar *cal);

#endif /* APP_UTIL_CALENDAR_H_ */
