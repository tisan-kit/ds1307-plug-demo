/*
 * weektask.c
 *
 *  Created on: 2015Äê12ÔÂ22ÈÕ
 *      Author: Administrator
 */

#include "../../pando/pando_object.h"
#include "c_types.h"
#include "user_interface.h"
#include "weektask.h"
// add your own includes below
#include "../../peripheral/peri_global.h"
#include "../../util/converter.h"
#include "../../pando/protocol/pando_endian.h"
#include <stdlib.h>


unsigned char task_buf[2*TASK_BUF_MAX_LENGTH];   //id:4(id), weekday len:7(1234567), time len:5(minute,11:30 mean 11:30:00),
							//delay len:4(minute), repeat len:1, enable len:1

struct weektask {
	uint16 id;
	char* weekday;
	char* time;
	uint16 delay;
	int8 repeat;
	int8 enable;
};
void ICACHE_FLASH_ATTR
weektask_init() {
	// TODO: add your object init code here.


}
void ICACHE_FLASH_ATTR
weektask_set(struct weektask* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.
	int len1 = os_strlen(value->weekday);
	int len2 = os_strlen(value->time);
	uint8 temp[5];
	uint16 delay = net16_to_host(value->delay);   //minute
	int8 repeat = value->repeat;   //1:repeat, 0: not repeat
	int8 enable = value->enable;	//1: enable, 0: disable
	int i,j=0;
	uint16 id = 0;

	for(i=0; i<2*TASK_BUF_MAX_LENGTH; i++)
	{
		task_buf[i] = 0;
	}

	PRINTF("\nweektask_set weekday:%s, time:%s, len1:%d,len2:%d,delay:%d,repeat:%d, enable:%d\n",
			value->weekday, value->time,len1, len2, delay, repeat, enable);
	if((len1 <= 0) || (len1 > 7))
	{
		PRINTF("\nlen1:%d\n",len1);
		return;
	}
	PRINTF("\npass len1:%d\n",len1);
	if(len2 != 5)
	{
		PRINTF("\nlen2:%d\n",len2);
		return;
	}
	if((repeat != 0) && (repeat != 1))
	{
		PRINTF("\nrepeat:%d\n",repeat);
		return;
	}
	if((enable != 0) && (enable != 1))
	{
		PRINTF("\nenable:%d\n",enable);
		return;
	}

	spi_flash_read((PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
		    	(uint32 *)task_buf, TASK_BUF_MAX_LENGTH);

	for(i=0; i<4; i++)
	{
		temp[i] = task_buf[i];
	}
	temp[i] = '\0';

	id =  (uint16)strtol(temp, NULL, 16);
	if(id >= 0xFFFD)
	{
		id = 0;
	}
	id++;

	os_sprintf(&task_buf[0], "%04X", id);

	j=4;
	for(i=0; i < len1; i++)
	{
		if((value->weekday[i]> '0') && (value->weekday[i] <= '7'))
			task_buf[j++] = value->weekday[i];
	}
	if(j<11)
	{
		for(i=j; i<11; i++)
			task_buf[i] = '0';
	}
	for(i=0,j=11; i<len2; i++)
	{
		if((i == 2) && (value->time[i] != ':'))
		{
			PRINTF("\nquit i:%d\n",i);
			return;
		}
		else if((i!=2) && ((value->time[i] < '0') || (value->time[i] > '9')))
		{
			PRINTF("\nvalue->time[%d]:%X\n",i, value->time[i]);
			return;
		}
		task_buf[j++] = value->time[i];
	}

	os_sprintf(&task_buf[j], "%04X%X%X", delay, repeat, enable);

	PRINTF("\n Get weektask:%s\n", task_buf);

	for(i=0, j=TASK_BUF_MAX_LENGTH; i<TASK_BUF_MAX_LENGTH; i++,j++)
	{
		task_buf[j] = task_buf[i];
	}


	spi_flash_erase_sector(PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO);
	spi_flash_write((PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
	    	    (uint32 *)task_buf, 2*TASK_BUF_MAX_LENGTH);
}
void ICACHE_FLASH_ATTR
weektask_get(struct weektask* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	int i,j;
	uint8 state = 1;
	uint16 delay = 0;
	uint8 temp[5];
	int8 repeat, enable;
	char *endstr;

	spi_flash_read((PRIV_PARAM_START_SEC + WEEKTASK_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
	    	(uint32 *)task_buf, TASK_BUF_MAX_LENGTH);

	PRINTF("\n Read flash:%s\n", task_buf);

	value->weekday = (char *)os_malloc(8);
	for(i = 0,j=4; i<7; i++,j++)
	{
		if((task_buf[j] < '0') || (task_buf[j] > '7'))
		{
			state = 0;
			task_buf[j] = '0';
		}
		value->weekday[i] = task_buf[j];
	}
	value->weekday[7] = '\0';

	value->time = (char *)os_malloc(6);
	for(i=0,j=11; i<5; i++,j++)
	{
		if(!state)
		{
			task_buf[j] = '0';
			if(i == 2)
				task_buf[j] = ':';
		}
		value->time[i] = task_buf[j];
	}
	value->time[5] = '\0';

	for(i=j; i<j+4; i++)
	{
		temp[i-j] = task_buf[i];
	}
	temp[i] = '\0';

	if(state)
	{
		delay = (uint16)strtol(temp, NULL, 16);
	}
	else
		delay = 0;
	PRINTF("\nget delay:%d", delay);
	value->delay = delay;
	if(task_buf[j+4] != '1')
		task_buf[j+4] = '0';
	value->repeat = task_buf[j+4] - '0';
	if(task_buf[j+5] != '1')
		task_buf[j+5] = '0';
	value->enable = task_buf[j+5] - '0';

	PRINTF("\n weektask_get \n");
}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct weektask* ICACHE_FLASH_ATTR
create_weektask() {
	struct weektask* weektask = (struct weektask*)os_malloc(sizeof(weektask));
	return weektask;
}
void ICACHE_FLASH_ATTR
delete_weektask(struct weektask* weektask) {
	if(weektask){
		os_free(weektask);
	}
}
void ICACHE_FLASH_ATTR
weektask_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct weektask* weektask = create_weektask();
	weektask_get(weektask);
	uint16 len = os_strlen(weektask->weekday);

	if (add_next_uri(params, len, weektask->weekday)){
		PRINTF("Add next param failed.\n");
		return;
	}

	len = os_strlen(weektask->time);
	if (add_next_uri(params, len, weektask->time)){
		PRINTF("Add next param failed.\n");
		return;
	}

	if (add_next_uint16(params, weektask->delay)){
		PRINTF("Add next param failed.\n");
		return;
	}

	if (add_next_int8(params, weektask->repeat)){
		PRINTF("Add next param failed.\n");
		return;
	}

	if (add_next_int8(params, weektask->enable)){
		PRINTF("Add next param failed.\n");
		return;
	}

//	if(weektask->weekday != NULL)
//	{
//		os_free(weektask->weekday);
//		weektask->weekday = NULL;
//	}
//	if(weektask->time != NULL)
//	{
//		os_free(weektask->time);
//		weektask->time = NULL;
//	}

	delete_weektask(weektask);
}
void ICACHE_FLASH_ATTR
weektask_object_unpack(PARAMS* params) {
	struct weektask* weektask = create_weektask();
	uint16 length = 0;

	weektask->weekday = get_next_bytes(params, &length);
	//weektask->weekday[length] = '\0';

	weektask->time = get_next_bytes(params, &length);
	//weektask->time[length] = '\0';

	weektask->delay = get_next_uint16(params);
	weektask->repeat = get_next_int8(params);
	weektask->enable = get_next_int8(params);

	weektask_set(weektask);
	delete_weektask(weektask);
}
void ICACHE_FLASH_ATTR
weektask_object_init() {
	weektask_init();
	pando_object weektask_object = {
		3,
		weektask_object_pack,
		weektask_object_unpack,
	};
	register_pando_object(weektask_object);
}



