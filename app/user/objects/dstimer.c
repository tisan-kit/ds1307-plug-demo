/*
 * dstimer.c
 *
 *  Created on: 2015Äê12ÔÂ8ÈÕ
 *      Author: wfq
 */

#include "../../pando/pando_object.h"
#include "c_types.h"
#include "user_interface.h"
#include "dstimer.h"
#include "../../peripheral/driver/i2c_ds1307.h"
#include "../../peripheral/peri_ds1307.h"
#include "../../peripheral/peri_global.h"

// add your own includes below
#define DST_FLASH_SECTOR_NO   2
#define DELAY_STEP_SECOND	  5
static os_timer_t ds_timer;
static uint32 delay_counter = 0;   //second

struct dstimer {
	uint32 delay;
	uint8 plug_state;
	uint8 enable;
};

void ICACHE_FLASH_ATTR
dstimer_init() {
	// TODO: add your object init code here.
	i2c_init();

//	os_timer_disarm(&ds_timer);
//	os_timer_setfn(&ds_timer, (os_timer_func_t *)output_rtc_str, NULL);
//	os_timer_arm(&ds_timer, DELAY_STEP_SECOND, 1);

}
void ICACHE_FLASH_ATTR
dstimer_set(struct dstimer* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.
//    spi_flash_erase_sector(PRIV_PARAM_START_SEC + DST_FLASH_SECTOR_NO);
//	spi_flash_write((PRIV_PARAM_START_SEC + DST_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
//	    value, sizeof(uint32));
//	PRINTF("dstimer_set,delay: %d,state:%d", value->delay, value->plug_state);
	get_pt_system_time();
}
void ICACHE_FLASH_ATTR
dstimer_get(struct dstimer* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
//    spi_flash_read((PRIV_PARAM_START_SEC + DST_FLASH_SECTOR_NO) * SPI_FLASH_SEC_SIZE,
//    	value, sizeof(struct dstimer));
//    PRINTF("dstimer_get, delay:%d, state:%d", value->delay, value->plug_state);
}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct dstimer* ICACHE_FLASH_ATTR
create_dstimer() {
	struct dstimer* dstimer = (struct dstimer*)os_malloc(sizeof(dstimer));
	return dstimer;
}
void ICACHE_FLASH_ATTR
delete_dstimer(struct dstimer* dstimer) {
	if(dstimer){
		os_free(dstimer);
	}
}
void ICACHE_FLASH_ATTR
dstimer_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct dstimer* dstimer = create_dstimer();
	dstimer_get(dstimer);

	if (add_next_uint32(params, dstimer->delay)){
		PRINTF("Add next param failed.\n");
		return;
	}

	if (add_next_uint8(params, dstimer->plug_state)){
		PRINTF("Add next param failed.\n");
		return;
	}

	if (add_next_uint8(params, dstimer->enable)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_dstimer(dstimer);
}
void ICACHE_FLASH_ATTR
dstimer_object_unpack(PARAMS* params) {
	struct dstimer* dstimer = create_dstimer();

	dstimer->delay = get_next_uint32(params);
	dstimer->plug_state = get_next_uint8(params);
	dstimer->enable = get_next_uint8(params);

	dstimer_set(dstimer);
	delete_dstimer(dstimer);
}
void ICACHE_FLASH_ATTR
dstimer_object_init() {
	dstimer_init();
	pando_object dstimer_object = {
		2,
		dstimer_object_pack,
		dstimer_object_unpack,
	};
	register_pando_object(dstimer_object);
}

