/*
 * calendar.c
 *
 *  Created on: 2015年12月17日
 *      Author: Administrator
 */

#include "calendar.h"

//Monthly revisions table
uint8 const week_amend_table[12]={0,3,3,6,1,4,6,2,5,0,3,5};
//amount of day of each month in common year
const uint8 day_month_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};


/**
*	@brief judge if it is leap year
*	@param year:
*	@retval 1: is a leap year; 0: is not a leap year
*
*/
uint8 is_leap_year(uint16 year)
{
	if(year%4==0)
	{
		if(year%100==0)
		{
			if(year%400==0)return 1;
			else return 0;
		}else return 1;
	}else return 0;
}

/**
*	@brief get rtc counter, thr return value can be the param to the RTC_SetCounter(seccount) directly
*	@param year:
*	@param month:
*	@param day: day
*	@param hour:
*	@param min:
*	@param sec:
*	@retval second counter
*/
uint32 get_rtc_counter(uint16 year,uint8 month,uint8 day,uint8 hour,uint8 min,uint8 sec)
{
    uint16 t;
    uint32 seccount;
    if(year<1970||year>2099)return 1;
	for(t=1970;t<year;t++)	//add all the second of the year before from 1970
	{
		if(is_leap_year(t))seccount+=31622400;//the amount of second of leap year
		else seccount+=31536000;			  //common year
	}
	month-=1;
	for(t=0;t<month;t++)	   //add all the second of the month before
	{
		seccount+=(uint32)day_month_table[t]*86400;
		if(is_leap_year(year)&&t==1)seccount+=86400;
	}
	seccount+=(uint32)(day-1)*86400;
	seccount+=(uint32)hour*3600;
    seccount+=(uint32)min*60;
    seccount+=(uint32)sec;

    return seccount;
}

uint32 get_day_counter(uint8 hour,uint8 min,uint8 sec)
{
    uint32 seccount = 0;

	seccount+=(uint32)hour*3600;
    seccount+=(uint32)min*60;
    seccount+=(uint32)sec;

    return seccount;
}

/**
*
*	@brief get what day is it. the Gregorian calendar(1901  ¡«¡¡2099)
*	@param year:
*	@param month:
*	@param day:
*	@retval the num of the week
*/
uint8_t get_weekday(uint16 year,uint8 month,uint8 day)
{
	uint16 temp2;
	uint8 yearH,yearL;

	yearH=year/100;	yearL=year%100;
	//
	if (yearH>19)yearL+=100;
	//
	temp2 = yearL+yearL/4;
	temp2 = temp2%7;
	temp2 = temp2 + day + week_amend_table[month-1];
	if ((yearL%4 == 0) && (month<3))temp2--;
	return(temp2 % 7);
}

/**
*
*	@brief get the calendar from time_value and time_zone
*	@param time_value:
*	@param time_zone:
*	@param cal:  out
*	@retval the num of the week
*/
void get_calendar(uint32 time_value, int time_zone, struct calendar *cal)
{
	static uint16 daycnt = 0;
	uint16 temp1 = 0;
	uint32 temp = 0;//time_value / 86400;    //a day of second is( 24 * 60 * 60 = 86400)
	int zone = 0;
	if(time_zone > 12)  //east 12 time zone
	{
		time_zone = 12;
	}
	else if(time_zone < -12) //west 12 time zone
	{
		time_zone = -12;
	}

	temp = (time_value + time_zone * 3600)/86400;

	if(daycnt != temp)
	{
		daycnt = temp;
		temp1 = 1970;
		while(temp >= 365)
		{
			if(is_leap_year(temp1))
			{
				if(temp >= 366){temp -= 366; }
				else{ temp1++; break;}
			}
			else temp -= 365;
			temp1++;
		}
		cal->year = temp1;
		temp1 = 0;

		while(temp >= 28)
		{
			if(is_leap_year(cal->year) && (temp1 == 1))
			{
				if(temp >= 29) temp -= 29;
				else break;
			}
			else
			{
				if(temp >= day_month_table[temp1])
				{
					temp -= day_month_table[temp1];
				}
				else break;
			}
			temp1++;
		}
		cal->month = temp1 + 1;
		cal->date = temp + 1;
	}

	temp = (time_value + time_zone * 3600) % 86400;
	cal->hour = temp/3600;
	cal->min  = (temp%3600)/60;
	cal->sec  = temp%3600%60;

	cal->weekday = get_weekday(cal->year, cal->month, cal->date);
}


