/*
 * i2c_interface.c
 *
 *  Created on: Jan 29, 2022
 *      Author: rjyar
 */

#include "i2c_interface.h"

void aquireI2CInterface(I2C_Device_ID dev){
    uint8_t i;
    uint8_t sum = 0;

    for(i=0; i<total_count; i++){
        sum += devices[i];
    }

    if(sum > 0){ // If something already has the interface
        if(devices[dev]){ //Check if its the same device
            return;
        } // If it is not, sleep a while and try again
        System_printf("Device %d is waiting for the interface", dev);
        System_flush();
        Task_sleep(1000000 / Clock_tickPeriod);
        aquireI2CInterface(dev);
    } else {
        devices[dev] = 1; // Checkout the interface
        return;
    }
}


void releaseI2CInterface(I2C_Device_ID dev){
    devices[dev] = 0; // Make sure it doesn't block future acquisitions
}
