/**
 * @file i2c_interface.h
 * 
 * @brief Declaration of all I2C Functions used in other scopes
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

/** 
 * \addtogroup sensortag_ctrl SensorTag Control
 * @{
 */

/** 
 * \addtogroup i2c I2C Interface
 * 
 * @brief The I2C interface is used to ensure reliable and safe usage of the I2C busses when physical busses outnumber the interfaces allowed by MCU
 * 
 * @{
 */

/***************** BEGIN USER CODE 10 **********************/
/// \todo USER CODE 10: If your I2C Device is on a different physical bus add it to the enum below
/**
 * @brief Enum for devices index in 'devices' array.
 *
 */
typedef enum {
    bme = 0,
    adxl,
    total_count
}I2C_Device_ID;
/******************* END USER CODE 10 **********************/


/**
 * @brief Struct containing all the information needed to open and run an I2C bus
 *
 */
typedef struct {
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;
}i2c_device;


/**
 * @brief Function to aquire I2C semaphore.
 *
 * Function will sleep task for a defined interval before checking if I2C is available. Once it is
 * It will lock the interface for itself
 *
 * @param dev The device trying to lock it out
 */
void aquireI2CInterface(I2C_Device_ID dev);

/**
 * @brief Function to release the I2C semaphore.
 *
 * Function will release the semaphore flag being held on I2C interface
 *
 * @param dev device that should release the flag
 */
void releaseI2CInterface(I2C_Device_ID dev);

/** @} */
/** @} */

#endif /* I2C_INTERFACE_I2C_INTERFACE_H_ */
