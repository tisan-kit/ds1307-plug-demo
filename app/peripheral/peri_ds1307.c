/*
 * peri_ds1307.c
 *
 *  Created on: 2015Äê11ÔÂ28ÈÕ
 *      Author: Administrator
 */

#include "peri_ds1307.h"
//#include "driver/i2c.h"
//#include "driver/i2c_master.h"
#include "driver/i2c_ds1307.h"
#include "user_interface.h"
#include "../pando/gateway/pando_system_time.h"
#include "../util/converter.h"
#include "mem.h"

#define ACK		1
#define NO_ACK	0
#define SLAVE	0xD0
#define WRITE   0x00
#define READ    0x01
#define ERR_ACK 0x01

//#define Start()  i2c_start()
//#define Stop()	 i2c_stop()
//#define ReadI2C(x)  i2c_readByte2(x)
//#define WriteI2C(x) i2c_writeByte(x)

#define Start()  i2c_start()
#define Stop()	 i2c_stop()
#define ReadI2C(x)  i2c_read_by_ack(x)
#define WriteI2C(x) i2c_write(x)

static uint8 rtc_time_dat[8] = {0};
unsigned char  RTC_ARR[7];  // Buffer for second,minute,.....,year

static uint64 pt_time = 0;
static int time_zone = 8;
struct calendar g_calendar = {0,0,0,0,0,0,0};

//-------------------------------
// Convert BCD 1 byte to HEX 1 byte
//-------------------------------
unsigned char BCD2HEX(unsigned int bcd)
{
	unsigned char temp;
	temp=((bcd>>8)*100)|((bcd>>4)*10)|(bcd&0x0f);
	return temp;

}

void ICACHE_FLASH_ATTR
get_pt_system_time(void)
{
	pt_time = pando_get_system_time();
    int i = 0;
    uint32 temp = (uint32)(pt_time/1000);

    for(i = 0; i<sizeof(pt_time); i++)
    {
    	PRINTF("%02x ", *((unsigned char*)(&pt_time) +i));
    }

    PRINTF("\n");

    get_calendar(temp, time_zone, &g_calendar);

    PRINTF("\n get_calendar: %d-%d-%d %d:%d:%d\n",
    		g_calendar.year,g_calendar.month, g_calendar.date,
			g_calendar.hour, g_calendar.min, g_calendar.sec);

}

void ICACHE_FLASH_ATTR
get_pt_system_time2(void)
{
	uint64 temp = pando_get_system_time();
    int i = 0;

    struct calendar *cal = (struct calendar*)os_malloc(sizeof(struct calendar));

    get_calendar(temp, time_zone, cal);

    PRINTF("\nDS get system time: ");
    for(i = 0; i<sizeof(temp); i++)
    {
    	PRINTF("%02x ", *((unsigned char*)(&temp) +i));
    }

    PRINTF("\n");


    PRINTF("\n get_calendar: %d-%d-%d %d:%d:%d\n",
    		cal->year,cal->month, cal->date, cal->hour, cal->min, cal->sec);
    os_free(cal);
    cal = NULL;
}

//-------------------------------
// Read RTC (all real time)
//-------------------------------
void ICACHE_FLASH_ATTR
ds1307_read_time2(unsigned char * buff)
{
//	Start();
//	WriteI2C(SLAVE);
//	WriteI2C(0x00);

	Start();
	WriteI2C(SLAVE + 0x01);
	*(buff+0)=ReadI2C(ACK);	// Second
	*(buff+1)=ReadI2C(ACK);	// Minute
	*(buff+2)=ReadI2C(ACK);	// hour
	*(buff+3)=ReadI2C(ACK);	// Day
	*(buff+4)=ReadI2C(ACK);	// date
	*(buff+5)=ReadI2C(ACK);	// month
	*(buff+6)=ReadI2C(NO_ACK);	// year
	Stop();
}

void ICACHE_FLASH_ATTR
ds1307_read_time(unsigned char * buff)
{
	Start();
	WriteI2C(SLAVE);
	WriteI2C(0x00);

	Start();
	WriteI2C(SLAVE + 0x01);
	*(buff+0)=ReadI2C(ACK);	// Second
	*(buff+1)=ReadI2C(ACK);	// Minute
	*(buff+2)=ReadI2C(ACK);	// hour
	*(buff+3)=ReadI2C(ACK);	// Day
	*(buff+4)=ReadI2C(ACK);	// date
	*(buff+5)=ReadI2C(ACK);	// month
	*(buff+6)=ReadI2C(NO_ACK);	// year
	Stop();
}

void ICACHE_FLASH_ATTR
ds1307_get_calendar(struct calendar *cld)
{
	Start();
	WriteI2C(SLAVE);
	WriteI2C(0x00);

	Start();
	WriteI2C(SLAVE + 0x01);
	*(RTC_ARR+0)=ReadI2C(ACK);	// Second
	*(RTC_ARR+1)=ReadI2C(ACK);	// Minute
	*(RTC_ARR+2)=ReadI2C(ACK);	// hour
	*(RTC_ARR+3)=ReadI2C(ACK);	// Day
	*(RTC_ARR+4)=ReadI2C(ACK);	// date
	*(RTC_ARR+5)=ReadI2C(ACK);	// month
	*(RTC_ARR+6)=ReadI2C(NO_ACK);	// year
	Stop();

	cld->year = 2000 + BCDtoHEX(RTC_ARR[6]);
	cld->month = BCDtoHEX(RTC_ARR[5]);
	cld->date = BCDtoHEX(RTC_ARR[4]);
	cld->weekday = BCDtoHEX(RTC_ARR[3]);
	cld->hour = BCDtoHEX(RTC_ARR[2]);
	cld->min = BCDtoHEX(RTC_ARR[1]);
	cld->sec = BCDtoHEX(RTC_ARR[0]);
}

//-------------------------------
// Set RTC
//-------------------------------
void ICACHE_FLASH_ATTR
ds1307_set_time(unsigned char *buff)
{

	Start();
	WriteI2C(SLAVE);
	WriteI2C(0x00);
	WriteI2C(*(buff+0));
	WriteI2C(*(buff+1));
	WriteI2C(*(buff+2));
	WriteI2C(*(buff+3));
	WriteI2C(*(buff+4));
	WriteI2C(*(buff+5));
	WriteI2C(*(buff+6));
	Stop();
}

void ICACHE_FLASH_ATTR
output_rtc_str(void)
{
	PRINTF("\nTime: ");
	ds1307_read_time(&rtc_time_dat[0]);
	PRINTF("Year:20%X ", rtc_time_dat[6]);
	PRINTF("Mon:%X ", rtc_time_dat[5]);
	PRINTF("Date:%X ", rtc_time_dat[4]);
	PRINTF("Day:%X ", rtc_time_dat[3]);
	PRINTF("Hou:%X ", rtc_time_dat[2]);
	PRINTF("Min:%X ", rtc_time_dat[1]);
	PRINTF("Sec:%X ", rtc_time_dat[0]);
	PRINTF("\n");
}



void ICACHE_FLASH_ATTR
init_rtc_time(void)
{
	ds1307_read_time(&RTC_ARR[0]);
	RTC_ARR[0] = RTC_ARR[0] & 0x7F;	// enable oscillator (bit 7=0)
	RTC_ARR[1] = 0x59;	// minute = 59
	RTC_ARR[2] = 0x23;	// hour = 05 ,24-hour mode(bit 6=0)
	RTC_ARR[3] = 0x04;	// Day = 1 or sunday
	RTC_ARR[4] = 0x31;	// Date = 30
	RTC_ARR[5] = 0x10;	// month = August
	RTC_ARR[6] = 0x05;	// year = 05 or 2005
	ds1307_set_time(&RTC_ARR[0]);	// Set RTC
}

