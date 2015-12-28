/*
 * i2c_ds1307.c
 *
 *  Created on: 2015Äê12ÔÂ7ÈÕ
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "i2c_ds1307.h"

/**
 * @name Pin selection and configuration block
 * @{
 */

//! @brief GPIO MUX for SDA pin
#define SDA_MUX  PERIPHS_IO_MUX_GPIO2_U
//! @brief GPIO FUNC for SDA pin
#define SDA_FUNC FUNC_GPIO2
//! @brief GPIO pin location for SDA pin
#define SDA_PIN  2
//! @brief GPIO bit location for SDA pin
#define SDA_BIT  BIT2

//! @brief GPIO MUX for SCL pin
#define SCL_MUX  PERIPHS_IO_MUX_GPIO0_U
//! @brief GPIO FUNC for SCL pin
#define SCL_FUNC FUNC_GPIO0
//! @brief GPIO pin location for SCL pin
#define SCL_PIN  0
//! @brief GPIO bit location for SCL pin
#define SCL_BIT  BIT0

//! Delay amount in-between bits, with os_delay_us(1) I get ~300kHz I2C clock
#define _DELAY os_delay_us(2)

/** @} */


// this is found in gpio.h from IoT SDK but not in FreeRTOS SDK
#ifndef GPIO_PIN_ADDR
#define GPIO_PIN_ADDR(i) (GPIO_PIN0_ADDRESS + i*4)
#endif

#define _SDA1 GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, SDA_BIT)
#define _SDA0 GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, SDA_BIT)

#define _SCL1 GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, SCL_BIT)
#define _SCL0 GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, SCL_BIT)

#define _SDAX ((GPIO_REG_READ(GPIO_IN_ADDRESS) >> SDA_PIN) & 0x01)
#define _SCLX ((GPIO_REG_READ(GPIO_IN_ADDRESS) >> SCL_PIN) & 0x01)


void ICACHE_FLASH_ATTR i2c_init(void)
{
    // MUX selection
    PIN_FUNC_SELECT(SDA_MUX, SDA_FUNC);
    PIN_FUNC_SELECT(SCL_MUX, SCL_FUNC);

    // Set SDA as OD output
    GPIO_REG_WRITE
    (
        GPIO_PIN_ADDR(GPIO_ID_PIN(SDA_PIN)),
        GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(SDA_PIN))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)
    );
    // Set SCK as OD output
    GPIO_REG_WRITE
    (
        GPIO_PIN_ADDR(GPIO_ID_PIN(SCL_PIN)),
        GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(SCL_PIN))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)
    );
    // Set idle bus high
    _SDA1;
    _SCL1;
    // Set both output
    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | SDA_BIT | SCL_BIT);
    return;
}


bool ICACHE_FLASH_ATTR i2c_start(void)
{
    _SDA1;
    _SCL1;
    _DELAY;
    if (_SDAX == 0) return false; // Bus busy
    _SDA0;
    _DELAY;
    _SCL0;
    return true;
}



void ICACHE_FLASH_ATTR i2c_stop(void)
{
    _SDA0;
    _SCL1;
    _DELAY;
    while (_SCLX == 0); // clock stretching
    _SDA1;
    _DELAY;
}


// return: true - ACK; false - NACK
bool ICACHE_FLASH_ATTR i2c_write(uint8_t data)
{
    uint8_t ibit;
    bool ret;

    for (ibit = 0; ibit < 8; ++ibit)
    {
        if (data & 0x80)
            _SDA1;
        else
            _SDA0;
        _DELAY;
        _SCL1;
        _DELAY;
        data = data << 1;
        _SCL0;
    }
    _SDA1;
    _DELAY;
    _SCL1;
    _DELAY;
    ret = (_SDAX == 0);
    _SCL0;
    _DELAY;

    return ret;
}


uint8_t ICACHE_FLASH_ATTR i2c_read(void)
{
    uint8_t data = 0;
    uint8_t ibit = 8;

    _SDA1;
    while (ibit--)
    {
        data = data << 1;
        _SCL0;
        _DELAY;
        _SCL1;
        _DELAY;
        if (_SDAX)
            data = data | 0x01;
    }
    _SCL0;

    return data;
}


void ICACHE_FLASH_ATTR i2c_set_ack(bool ack)
{
    _SCL0;
    if (ack)
        _SDA0;  // ACK
    else
        _SDA1;  // NACK
    _DELAY;
    // Send clock
    _SCL1;
    _DELAY;
    _SCL0;
    _DELAY;
    // ACK end
    _SDA1;
}

uint8_t ICACHE_FLASH_ATTR i2c_read_by_ack(bool ack)
{
	uint8_t data;

	data = i2c_read();

	i2c_set_ack(ack);

	return data;
}

