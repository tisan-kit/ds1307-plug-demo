/*
 * peri_ds1307.h
 *
 *  Created on: 2015Äê11ÔÂ28ÈÕ
 *      Author: Administrator
 */

#ifndef APP_PERIPHERAL_PERI_DS1307_H_
#define APP_PERIPHERAL_PERI_DS1307_H_

#include "../../include/os_type.h"
#include "../util/calendar.h"

extern struct calendar g_calendar;

void ds1307_get_calendar(struct calendar *cld);
void ds1307_read_time(unsigned char * buff);
void ds1307_set_time(unsigned char *buff);
void output_rtc_str(void);
void get_pt_system_time(void);
void init_rtc_time(void);

#endif /* APP_PERIPHERAL_PERI_DS1307_H_ */
