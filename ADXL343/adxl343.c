/*
 * adxl343.c
 *
 *  Created on: Jan 19, 2022
 *      Author: rjyar
 */

#include "adxl343.h"

Void adxlTaskFxn(UArg arg0, UArg arg1)
{

    adxl_busy = 0; // Make sure we aren't blocking the i2c interface

    // Multiuse variable we will use later
    uint8_t f = 0;

    /* ADXL variables */
    uint8_t         device_id, bw_rate, power_control, data_format;
    uint8_t         x_lsb, x_msb, y_lsb, y_msb, z_lsb, z_msb;

    /* I2C Variables */
    uint8_t         txBuffer[8];
    uint8_t         rxBuffer[16];
    ADXL adxl;


    /* Create I2CParam for device */
    I2C_Params_init(&(adxl.i2cParams));
    adxl.i2cParams.bitRate = I2C_100kHz;

    gADXL = &adxl;

 while(1){

    /* Build the first package */
    txBuffer[0] =  0x0; // Empty message
    gADXL->i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
    gADXL->i2cTransaction.writeBuf = txBuffer;
    gADXL->i2cTransaction.writeCount = 1;
    gADXL->i2cTransaction.readBuf = &rxBuffer;
    gADXL->i2cTransaction.readCount = 1;

    while(bme_busy) {
        System_printf("Waiting on BME\n");
        System_flush();
        Task_sleep(1000000 / Clock_tickPeriod);} //Sleep until we can use the i2c interface

    /* Lock the I2C and do what we need to do */
    adxl_busy = 1;
    System_printf("ADXL Locking out i2c\n");
    System_flush();
    f = 0;
    do {
        if(f){
            System_printf("ADXL I2C failed to initialize!\n");
        }
        gADXL->i2c = I2C_open(Board_I2C_ADXL, &(gADXL->i2cParams));
        f++;
    } while(gADXL->i2c == NULL);

    System_printf("ADXL I2C Initialized!\n");
    while(!I2C_transfer(gADXL->i2c, &(gADXL->i2cTransaction))){
        System_printf("ADXL Configure Fail.\n");\
        System_flush();
    }
    device_id = rxBuffer[0];
    System_printf("ID 0x%x.\n", device_id);
    System_flush();


    txBuffer[0] = BW_RATE;
    txBuffer[1] = 0x0A;
    gADXL->i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
    gADXL->i2cTransaction.writeBuf = txBuffer;
    gADXL->i2cTransaction.writeCount = 2;
    gADXL->i2cTransaction.readBuf = rxBuffer;
    gADXL->i2cTransaction.readCount = 1;

    while(!I2C_transfer(gADXL->i2c, &(gADXL->i2cTransaction))){
        System_printf("BW Fail.\n");\
        System_flush();
    }
    bw_rate = rxBuffer[0];
    System_printf("BW RATE 0x%x.\n", bw_rate);
    System_flush();


    txBuffer[0] = DATA_FORMAT;
    txBuffer[1] = RANGE_16G;
    gADXL->i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //BME688 ADDR;
    gADXL->i2cTransaction.writeBuf = txBuffer;
    gADXL->i2cTransaction.writeCount = 2;
    gADXL->i2cTransaction.readBuf = rxBuffer;
    gADXL->i2cTransaction.readCount = 6;

    while(!I2C_transfer(gADXL->i2c, &(gADXL->i2cTransaction))){
        System_printf("Data Format Fail.\n");\
        System_flush();
    }
    data_format = rxBuffer[0];
    System_printf("Data format 0x%x.\n", data_format);
    System_flush();

    txBuffer[0] = POWER_CTL;
    txBuffer[1] = ADXL_MEASURE;
    gADXL->i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //ADXL343 ADDR;
    gADXL->i2cTransaction.writeBuf = txBuffer;
    gADXL->i2cTransaction.writeCount = 2;
    gADXL->i2cTransaction.readBuf = rxBuffer;
    gADXL->i2cTransaction.readCount = 1;

    while(!I2C_transfer(gADXL->i2c, &(gADXL->i2cTransaction))){
        System_printf("Power Ctrl Format Fail.\n");\
        System_flush();
    }
    power_control = rxBuffer[0];
    System_printf("Power_control 0x%x.\n", power_control);
    System_flush();


    txBuffer[0] = DATA_X0;
    gADXL->i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //ADXL343 ADDR;
    gADXL->i2cTransaction.writeBuf = txBuffer;
    gADXL->i2cTransaction.writeCount = 1;
    gADXL->i2cTransaction.readBuf = rxBuffer;
    gADXL->i2cTransaction.readCount = 6;

    uint8_t i = 0;
    for(i = 0; i < 1; i++){
        if(!I2C_transfer(gADXL->i2c, &(gADXL->i2cTransaction))){
            System_printf("Data Fail.\n");\
            System_flush();
            continue;
        }
             x_lsb = rxBuffer[0];
             x_msb = rxBuffer[1];
             y_lsb = rxBuffer[2];
             y_msb = rxBuffer[3];
             z_lsb = rxBuffer[4];
             z_msb = rxBuffer[5];
             System_printf("X 0x%x %x.\n", x_msb, x_lsb);
             System_printf("Y 0x%x %x.\n", y_msb, y_lsb);
             System_printf("Z 0x%x %x.\n", z_msb, z_lsb);
             System_flush();
    }

     I2C_close(gADXL->i2c);

     adxl_busy = 0;

     Task_sleep(1000000 / Clock_tickPeriod);

 }
}
