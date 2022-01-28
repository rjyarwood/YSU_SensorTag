/*
 * i2c_interface.h
 *
 *  Created on: Jan 20, 2022
 *      Author: rjyar
 */

#ifndef I2C_INTERFACE_I2C_INTERFACE_H_
#define I2C_INTERFACE_I2C_INTERFACE_H_

#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>

typedef enum {
    LOCKED = -1,
    SUCCESS = 0,
    I2C_OPEN_FAULT,
    I2C_TRANSFER_FAULT
}I2C_Transaction_Code;

uint8_t bme_busy;
uint8_t adxl_busy;

#endif /* I2C_INTERFACE_I2C_INTERFACE_H_ */
