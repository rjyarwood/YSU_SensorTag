/**
 * @file i2c_interface.c
 * 
 * @brief Implementation of I2C Interface that is used to handle multiple sensors having simultaneous communication.
 * 
 * This is especially important when there are multiple physical busses present on the PCB as the CC chips have one 1 interface available.
 * This means the interface must be opened for each operation and ensured it is closed after use.
 *
 *  Created on: Jan 29, 2022
 *      Author: rjyar
 */

#include "i2c_interface.h"

/** 
 * \addtogroup i2c I2C Interface
 */


/**
 * \ingroup i2c
 * @brief An array representing all the devices that use seperate I2C busses.
 *
 * Will store a 1 at index I2C_Device_ID if device is holding the flag.
 */
uint8_t i2c_devices[total_count]; /// An array to keep track of what device has control of the interface

void aquireI2CInterface(I2C_Device_ID dev){
    uint8_t i;
    uint8_t sum = 0;

    for(i=0; i<total_count; i++){
        sum += i2c_devices[i];
    }

    if(sum != 0){ // If something already has the interface
        if(i2c_devices[dev]){ //Check if its the same device
            return;
        } // If it is not, sleep a while and try again
        System_printf("Device %d is waiting for the interface", dev);
        System_flush();
        Task_sleep(1000 / Clock_tickPeriod);
        aquireI2CInterface(dev);
    } else {
        i2c_devices[dev] = 1; // Checkout the interface
        return;
    }
}


void releaseI2CInterface(I2C_Device_ID dev){
    i2c_devices[dev] = 0; // Make sure it doesn't block future acquisitions
}
