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


/** If adding a device, add a flag here. This ensures only one device on the bus at a time */
typedef enum {
    bme = 0,
    adxl,
    total_count
}I2C_Device_ID;

typedef struct {
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;
}i2c_device;



/** Function that will ensure I2C interface is available.
 * If it is not available it will sleep the task and check again
 *
 * @param I2C_Device The type of device calling this
 */
void aquireI2CInterface(I2C_Device_ID dev);

/** Function that will release the interface for other devices to aquire.
 *
 * @param I2C_Device The type of device calling this
 */
void releaseI2CInterface(I2C_Device_ID dev);

uint8_t devices[total_count]; /// An array to keep track of what device has control of the interface

#endif /* I2C_INTERFACE_I2C_INTERFACE_H_ */
