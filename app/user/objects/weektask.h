/*
 * weektask.h
 *
 *  Created on: 2015Äê12ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef APP_USER_OBJECTS_WEEKTASK_H_
#define APP_USER_OBJECTS_WEEKTASK_H_

#define WEEKTASK_FLASH_SECTOR_NO   	3
#define TASK_BUF_MAX_LENGTH			24

extern unsigned char task_buf[2*TASK_BUF_MAX_LENGTH];

void weektask_object_init();


#endif /* APP_USER_OBJECTS_WEEKTASK_H_ */
