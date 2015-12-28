/*
 * user_biz.c
 *
 *  Created on: 2015Äê12ÔÂ25ÈÕ
 *      Author: Administrator
 */

#include "user_interface.h"
#include "../peripheral/driver/uart.h"
#include "objects/plug.h"
#include "objects/devicetime.h"
#include "objects/weektask.h"
#include "../peripheral/peri_global.h"
#include "../peripheral/peri_ds1307.h"
#include "../peripheral/peri_jdq.h"
#include "user_biz.h"
#include <stdlib.h>

static os_timer_t task_timer;
struct task_item g_task_item = {0,0,0,0,0,0,0,0,0,0};

uint8 get_task_weekday(unsigned char *wd_buf)
{
	uint8 weekday = 0;
	uint8 temp_buf[7];
	uint8 i,j;

	for(i=0,j=0; i<7; i++)
	{
		if((wd_buf[i] > '0') && (wd_buf[i] <= '7'))
		{
			temp_buf[j++] = wd_buf[i];
		}
	}

	if(j == 0)
		return weekday;

	for(i=0; i<j; i++)
	{
		switch(temp_buf[i])
		{
			case '1': weekday &= 0xFE; weekday |= 0x01;
				break;
			case '2': weekday &= 0xFD; weekday |= 0x02;
				break;
			case '3': weekday &= 0xFB; weekday |= 0x04;
				break;
			case '4': weekday &= 0xF7; weekday |= 0x08;
				break;
			case '5': weekday &= 0xEF; weekday |= 0x10;
				break;
			case '6': weekday &= 0xDF; weekday |= 0x20;
				break;
			case '7': weekday &= 0xBF; weekday |= 0x40;
				break;
		}
	}
	return weekday;
}

/******************************************************************************
 * FunctionName : clear_task_week.
 * Description  : clear current weekday which is meet whit task_weekday.
 * Parameters   : twd -- task_weekday
 * 				  buf -- buffer read from flash, which will update to flash.
 * Returns      : 0: need not clear; 1: had, clear
*******************************************************************************/
char clear_task_week(unsigned char* buf, unsigned char twd)
{
	uint8 i;
	unsigned char temp = twd + '0';

	for(i=0; i<7; i++)
	{
		if(buf[i+4] == twd)
		{
			buf[i+4] = '0';
			return 1;
		}
	}
	return 0;
}

/******************************************************************************
 * FunctionName : task_check_week.
 * Description  : check current weekday which is meet whit task_weekday.
 * Parameters   : twd -- task_weekday
 * 				  wd -- current weekday
 * Returns      : 0: not meet; 1: meet
*******************************************************************************/
char task_check_week(uint8 twd, uint8 wd)
{
	char state = 0;
	uint8 temp = 0;

	switch(wd)
	{
		case 1: temp = 0x01;
			break;
		case 2: temp = 0x02;
			break;
		case 3: temp = 0x04;
			break;
		case 4: temp = 0x08;
			break;
		case 5: temp = 0x10;
			break;
		case 6: temp = 0x20;
			break;
		case 7: temp = 0x40;
			break;
	}

	if((twd & temp) > 0)
	{
		state = 1;
	}

	return state;
}

//
// result 0:
int ICACHE_FLASH_ATTR
load_task(void)
{
	uint8 i,j, temp, state;
	uint8 temp_buf[7];
	int result = 0;
	uint16 id = 0;

	//read flash
	spi_flash_read((PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
		    	(uint32 *)task_buf, 2*TASK_BUF_MAX_LENGTH);

	PRINTF("\nload_task Read flash1:%s\n", task_buf);
	PRINTF("load_task Read flash2:%s\n", &task_buf[TASK_BUF_MAX_LENGTH]);

	for(i=0; i<4; i++)
	{
		temp_buf[i] = task_buf[i];
	}
	temp_buf[i] = '\0';

	id =  (uint16)strtol(temp_buf, NULL, 16);
	if(id != g_task_item.id)
	{
		g_task_item.id = id;
		result = 2;
	}

	g_task_item.weekday = get_task_weekday(&task_buf[TASK_BUF_MAX_LENGTH+4]);
	PRINTF("\nget_task_weekday %d", g_task_item.weekday);
	if(g_task_item.weekday  == 0)
	{
		return 0;
	}

	//get hour, minute, and second
	j = TASK_BUF_MAX_LENGTH + 4 + 7;
	if((task_buf[j] < '0') || (task_buf[j] > '2'))
	{
		PRINTF("\ntask_buf[j] value is invalid:%d\n", task_buf[j] );
		return 0;
	}
	else
	{
		PRINTF("\ntask_buf[j] value is ,,,:%d\n", task_buf[j] );
		if(task_buf[j] == '2')
		{
			if((task_buf[j+1] < '0') || (task_buf[j+1] >= '4'))
			{
				PRINTF("\ntask_buf[j+1] value is invalid:%d\n", task_buf[j+1] );
				return 0;
			}
		}

		PRINTF("\ntask_buf[j] value is ,,,,:%d\n", task_buf[j] );
		if((task_buf[j+1] < '0') || (task_buf[j+1] > '9'))
		{
			return 0;
		}
		else
		{
			if(task_buf[j+2] != ':')
			{
				PRINTF("\ntask_buf[j+2] value is,:%d\n", task_buf[j+2] );
				return 0;
			}
			if((task_buf[j+3] < '0') || (task_buf[j+3] > '5'))
			{
				PRINTF("\ntask_buf[j+3] value is ,:%d\n", task_buf[j+3] );
				return 0;
			}
			if((task_buf[j+4] < '0') || (task_buf[j+4] > '9'))
			{
				PRINTF("\ntask_buf[j+4] value is :%d\n", task_buf[j+4] );
				return 0;
			}

			g_task_item.hour = (task_buf[j] - '0')*10 + (task_buf[j+1] - '0');
			g_task_item.min = (task_buf[j+3]-'0')*10 + (task_buf[j+4] - '0');
			g_task_item.sec = 0;
			g_task_item.day_counter = get_day_counter(g_task_item.hour, g_task_item.min, g_task_item.sec);
			PRINTF("\ncompute g_task_item, hour:%d,min:%d,counter:%u\n",g_task_item.hour, g_task_item.min, g_task_item.day_counter);
		}
	}

	//get delay
	j += 5;
	for(i=j; i<j+4; i++)
	{
		temp_buf[i-j] = task_buf[i];
	}
	temp_buf[i] = '\0';
	g_task_item.delay = (uint16)strtol(temp_buf, NULL, 16);
	PRINTF("\nget delay:%d", g_task_item.delay);

	//repeat
	if(task_buf[j+4] != '1')
		task_buf[j+4] = '0';
	g_task_item.repeat = task_buf[j+4] - '0';

	//enable
	if(task_buf[j+5] != '1')
		task_buf[j+5] = '0';
	g_task_item.enable = task_buf[j+5] - '0';

	PRINTF("\n weektask_item_get \n");

	if(result != 2)
	{
		result = 1;
	}

	return result;
}

void ICACHE_FLASH_ATTR
task_check(void)
{
	int temp = 0;
	static uint8 state= 0;
	uint32 cday_counter = 0, temp_day_counter = 0;

	//1. load config from flash
	temp = load_task();
	if(temp == 0)
	{
		return;
	}
	else if(temp == 2)
	{
		state = 0;
	}

	//2.check if enable
	if(g_task_item.enable != 1)
	{
		return;
	}

	//3.check time
	ds1307_get_calendar(&g_calendar);
	PRINTF("\n task_check get device calendar:%d-%d-%d %d:%d:%d, weekday:%d\n",
			g_calendar.year, g_calendar.month, g_calendar.date, g_calendar.hour, g_calendar.min, g_calendar.sec, g_calendar.weekday);
	cday_counter = get_day_counter(g_calendar.hour, g_calendar.min, g_calendar.sec);
	PRINTF("\ncompute calendar, hour:%d,min:%d,counter:%u\n",g_calendar.hour, g_calendar.min, cday_counter);

	//set timer check if task exist, or change(update or delete)
	temp = task_check_week(g_task_item.weekday, g_calendar.weekday);
	if(temp != 1)
	{
		PRINTF("\ntask_check_week:%d\n", temp);
		return;
	}

	PRINTF("\n cday_counter:%u,g_task_item.day_counter:%d ,change plug state:%d\n", cday_counter, g_task_item.day_counter,state);
	if((cday_counter >= g_task_item.day_counter) && (state == 0) && (cday_counter < (g_task_item.day_counter + g_task_item.delay)))
	{
		if(peri_jdq_get() != 1)
		{
			peri_jdq_set(1);
		}
		state = 1;
		PRINTF("\nChange plug state:%d\n", peri_jdq_get() );
		//need to solve the repeat
		if(g_task_item.repeat != 1)
		{
			PRINTF("\nWill check weekday\n");
			//how to know had over?
			if(clear_task_week(&task_buf[TASK_BUF_MAX_LENGTH], g_calendar.weekday) == 1)
			{
				PRINTF("\nclear_task_week write flash1:%s\n", task_buf);
				PRINTF("clear_task_week write flash2:%s\n", &task_buf[TASK_BUF_MAX_LENGTH]);
				//need to update falsh
				spi_flash_erase_sector(PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO);
				spi_flash_write((PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
							(uint32 *)task_buf, 2*TASK_BUF_MAX_LENGTH);
			}
		}
	}
	else if((cday_counter >= (g_task_item.day_counter + g_task_item.delay)) && (state == 1))
	{
		if(peri_jdq_get() != 0)
		{
			peri_jdq_set(0);
		}
		state = 0;
	}

}

void ICACHE_FLASH_ATTR
task_timer_set(void)
{
	//load_task();

	os_timer_disarm(&task_timer);
	os_timer_setfn(&task_timer, (os_timer_func_t *)task_check, NULL);
	os_timer_arm(&task_timer, 5000, 1);
}



