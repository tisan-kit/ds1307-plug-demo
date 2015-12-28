/*
 * devicetime.c
 *
 *  Created on: 2015Äê12ÔÂ22ÈÕ
 *      Author: Administrator
 */

#include "../../pando/pando_object.h"
#include "c_types.h"
#include "user_interface.h"
#include "DeviceTime.h"
// add your own includes below
#include "../../peripheral/driver/i2c_ds1307.h"
#include "../../peripheral/peri_ds1307.h"
#include "../../peripheral/peri_global.h"
#include "../../util/converter.h"

#define DEVICETIME_FLASH_SECTOR_NO   2
#define DEVICETIME_LENGTH            19    //str_len("2015-12-22 11:31:33");

unsigned char time_buf[8];

struct DeviceTime {
	char* datetime;
};
void ICACHE_FLASH_ATTR
DeviceTime_init() {
	// TODO: add your object init code here.
	i2c_init();
}
void ICACHE_FLASH_ATTR
DeviceTime_set(struct DeviceTime* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.
	PRINTF("\n Will set device datetime\n");

	int len = os_strlen(value->datetime);
	uint16 year = 2000;
	uint8 month = 0, date = 0, day = 0, hour= 0, min = 0, sec = 0;
	uint8 i;

	if(len < 19)
		PRINTF("\n Not valid dat:%s\n", value->datetime);
	else
	{
		PRINTF("\nPrintf datetime: ");
		for(i=0; i<len; i++)
		{
			PRINTF("%X ", value->datetime[i]);
		}

		sec = strbuf_to_u32(&value->datetime[17], 2);
		min = strbuf_to_u32(&value->datetime[14], 2);
		hour = strbuf_to_u32(&value->datetime[11], 2);
		date = strbuf_to_u32(&value->datetime[8], 2);
		month = strbuf_to_u32(&value->datetime[5], 2);
		year += strbuf_to_u32(&value->datetime[2], 2);
		PRINTF("\nConvert:%d-%d-%d %d:%d:%d\n", year, month, date, hour, min, sec);

		DectoBCD(sec, &time_buf[0], 1);
		DectoBCD(min, &time_buf[1], 1);
		DectoBCD(hour, &time_buf[2], 1);
		DectoBCD(date, &time_buf[4], 1);
		DectoBCD(month, &time_buf[5], 1);
		DectoBCD(year, &time_buf[6], 1);
		day = get_weekday(year, month, date);
		time_buf[3] = day;
		ds1307_set_time(time_buf);
	}

	PRINTF("\n DeviceTime_set:%s\n", value->datetime);
	for(len = 0; len < 7; len++)
	{
		PRINTF("%02X ", time_buf[len]);
	}
}
void ICACHE_FLASH_ATTR
DeviceTime_get(struct DeviceTime* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	PRINTF("\n Will get device datetime\n");

	ds1307_read_time(time_buf);

	value->datetime = (char *)os_malloc(DEVICETIME_LENGTH + 1);
	value->datetime[DEVICETIME_LENGTH] = '\0';
	os_sprintf(value->datetime, "20%02X-%02X-%02X %02X:%02X:%02X",
			time_buf[6], time_buf[5], time_buf[4], time_buf[2], time_buf[1], time_buf[0]);
	PRINTF("\n Get device datetime:%s\n", value->datetime);

	ds1307_get_calendar(&g_calendar);
	PRINTF("\n ds1307_get_calendar:%d-%d-%d %d:%d:%d, weekday:%d\n",
				g_calendar.year, g_calendar.month, g_calendar.date, g_calendar.hour, g_calendar.min, g_calendar.sec, g_calendar.weekday);
}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct DeviceTime* ICACHE_FLASH_ATTR
create_DeviceTime() {
	struct DeviceTime* DeviceTime = (struct DeviceTime*)os_malloc(sizeof(DeviceTime));
	return DeviceTime;
}
void ICACHE_FLASH_ATTR
delete_DeviceTime(struct DeviceTime* DeviceTime) {
	if(DeviceTime){
		os_free(DeviceTime);
	}
}
void ICACHE_FLASH_ATTR
DeviceTime_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct DeviceTime* DeviceTime = create_DeviceTime();
	DeviceTime_get(DeviceTime);
	uint16 len = os_strlen(DeviceTime->datetime);

	if (add_next_uri(params, len, DeviceTime->datetime)){
		PRINTF("Add next param failed.\n");
		return;
	}

	PRINTF("\n devicetime pack:%s\n", DeviceTime->datetime);

//	if(DeviceTime->datetime != NULL)
//	{
//		os_free(DeviceTime->datetime);
//		DeviceTime->datetime = NULL;
//	}

	delete_DeviceTime(DeviceTime);
}
void ICACHE_FLASH_ATTR
DeviceTime_object_unpack(PARAMS* params) {
	struct DeviceTime* DeviceTime = create_DeviceTime();
	uint16 length = 0;

	DeviceTime->datetime = get_next_bytes(params, &length);  //? if DeviceTime has malloc space?
	DeviceTime->datetime[length] = '\0';

	PRINTF("\n Rev dat length:%d, dat:%s\n", length, DeviceTime->datetime);

	DeviceTime_set(DeviceTime);
	delete_DeviceTime(DeviceTime);
}
void ICACHE_FLASH_ATTR
DeviceTime_object_init() {
	DeviceTime_init();
	pando_object DeviceTime_object = {
		2,
		DeviceTime_object_pack,
		DeviceTime_object_unpack,
	};
	register_pando_object(DeviceTime_object);
}



