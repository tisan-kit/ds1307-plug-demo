/*
 * user_biz.h
 *
 *  Created on: 2015Äê12ÔÂ25ÈÕ
 *      Author: Administrator
 */

#ifndef APP_USER_USER_BIZ_H_
#define APP_USER_USER_BIZ_H_

#include "os_type.h"
#include "../util/calendar.h"

struct task_item
{
	uint16 id;
	uint8 weekday;   // Monday: 0x01; Tuesday: 0x02; Wednesday: 0x04; Thursday: 0x08; Friday: 0x10; Saturday: 0x20; Sunday: 0x40;
	uint8 hour;
	uint8 min;
	uint8 sec;
	uint32 day_counter;
	uint16 delay;
	uint8 state;
	uint8 repeat;
	uint8 enable;
};

void task_timer_set(void);

#endif /* APP_USER_USER_BIZ_H_ */
