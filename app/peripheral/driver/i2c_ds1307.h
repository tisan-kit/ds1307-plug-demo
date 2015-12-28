/*
 * i2c_test.h
 *
 *  Created on: 2015Äê12ÔÂ7ÈÕ
 *      Author: Administrator
 */

#ifndef APP_PERIPHERAL_DRIVER_I2C_TEST_H_
#define APP_PERIPHERAL_DRIVER_I2C_TEST_H_


/**
 * @brief   Initialize I2C GPIO pins
 */
void i2c_init(void);

/**
 * @brief   Send I2C start bit
 * @return  true if start successfully. Otherwise the bus is busy
 */
bool i2c_start(void);

/**
 * @brief   Send I2C stop bit
 */
void i2c_stop(void);

/**
 * @brief    Send data to I2C bus
 * @param    data Data to send
 * @return   true if ACK is received at end of sending. False if not ACK'ed
 */
bool i2c_write(uint8_t data);

/**
 * @brief    Read data from I2C bus
 * @return   Data read
 */
uint8_t i2c_read(void);

/**
 * @brief   Send acknowledge bit (at end of reading)
 * @param   ack ACK (true) or NACK (false)
 */
void i2c_set_ack(bool ack);

uint8_t i2c_read_by_ack(bool ack);

#endif /* APP_PERIPHERAL_DRIVER_I2C_TEST_H_ */
