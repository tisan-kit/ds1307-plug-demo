/******************************************************************************
 /*******************************************************
 * File name: user_main.c
 * Author:Chongguang Li
 * Versions:1.0
 * Description:entry file of user application.
 *
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
*******************************************************************************/

#include "../peripheral/driver/uart.h"
#include "os_type.h"
#include "user_interface.h"
#include "../peripheral/peri_key.h"
#include "wifi_config.h"
#include "../pando/pando_framework.h"
#include "objects/plug.h"
#include "objects/devicetime.h"
#include "objects/weektask.h"
#include "../peripheral/peri_ds1307.h"
#include "user_biz.h"


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	uart_init(115200, 115200); // serial bound rate:11520.

	//long press gpio4, enter into wifi config mode.
	peri_config_key_init(4);
	base_keys_init();               //base keys init at the last of every single key init.
	//auto_check_connect_init();

	// add you object init here.
	plug_object_init();
	DeviceTime_object_init();
	weektask_object_init();

	task_timer_set();

	pando_framework_init();
}

