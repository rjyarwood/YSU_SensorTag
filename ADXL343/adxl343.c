/*
 * adxl343.c
 *
 *  Created on: Jan 19, 2022
 *      Author: rjyar
 */

#include "adxl343.h"

Void adxlTaskFxn(UArg arg0, UArg arg1)
{
    /* ADXL variables */
    uint8_t         device_id, bw_rate, power_control, data_format;
    uint8_t         x_lsb, x_msb, y_lsb, y_msb, z_lsb, z_msb;

    /* I2C Variables */
    uint8_t         txBuffer[8];
    uint8_t         rxBuffer[16];
    i2c_device      dev;


    /* Create I2CParam for device */
    I2C_Params_init(&(dev.i2cParams));
    dev.i2cParams.bitRate = I2C_100kHz;

     while(1){ //Task main loop

        aquireI2CInterface(adxl);  // Checkout the I2C Interface

        #ifdef DEBUG
            System_printf("ADXL Locking out i2c\n");
            System_flush();
        #endif

        /* Open the I2C Interface */
        dev.i2c = I2C_open(Board_I2C_ADXL, &(dev.i2cParams));
        /* If we cannot open it, print the error and quit the task */
        if(dev.i2c == NULL){
            #ifdef DEBUG
                System_printf("Error opening ADXL I2C Interface");
                System_flush();
            #endif
            releaseI2CInterface(adxl);
            return;
        }


        #ifdef DEBUG
            System_printf("ADXL I2C Initialized!\n");
            System_flush();
        #endif

        /* ================================================== */
        /* ================ BEGIN CONFIGURATION ============= */
        /* ================================================== */

        /* Build the first package */
        txBuffer[0] =  0x0; // Empty message
        dev.i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
        dev.i2cTransaction.writeBuf = txBuffer;
        dev.i2cTransaction.writeCount = 1;
        dev.i2cTransaction.readBuf = &rxBuffer;
        dev.i2cTransaction.readCount = 1;

        while(!I2C_transfer(dev.i2c, &(dev.i2cTransaction))){
            #ifdef DEBUG
                System_printf("ADXL Configure Fail.\n");
                System_flush();
            #endif
        }
        device_id = rxBuffer[0];

        #ifdef CONSOLE
            System_printf("ID 0x%x.\n", device_id);
            System_flush();
        #endif

        txBuffer[0] = BW_RATE;
        txBuffer[1] = 0x0A;
        dev.i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
        dev.i2cTransaction.writeBuf = txBuffer;
        dev.i2cTransaction.writeCount = 2;
        dev.i2cTransaction.readBuf = rxBuffer;
        dev.i2cTransaction.readCount = 1;

        while(!I2C_transfer(dev.i2c, &(dev.i2cTransaction))){
            #ifdef DEBUG
                System_printf("BW Fail.\n");
                System_flush();
            #endif
        }
        bw_rate = rxBuffer[0];

        #ifdef CONSOLE
            System_printf("BW RATE 0x%x.\n", bw_rate);
            System_flush();
        #endif


        txBuffer[0] = DATA_FORMAT;
        txBuffer[1] = RANGE_16G;
        dev.i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
        dev.i2cTransaction.writeBuf = txBuffer;
        dev.i2cTransaction.writeCount = 2;
        dev.i2cTransaction.readBuf = rxBuffer;
        dev.i2cTransaction.readCount = 6;

        while(!I2C_transfer(dev.i2c, &(dev.i2cTransaction))){
            #ifdef DEBUG
                System_printf("Data Format Fail.\n");
                System_flush();
            #endif
        }


        data_format = rxBuffer[0];
        #ifdef CONSOLE
            System_printf("Data format 0x%x.\n", data_format);
            System_flush();
        #endif


        txBuffer[0] = POWER_CTL;
        txBuffer[1] = ADXL_MEASURE;
        dev.i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
        dev.i2cTransaction.writeBuf = txBuffer;
        dev.i2cTransaction.writeCount = 2;
        dev.i2cTransaction.readBuf = rxBuffer;
        dev.i2cTransaction.readCount = 1;

        while(!I2C_transfer(dev.i2c, &(dev.i2cTransaction))){
            #ifdef DEBUG
                System_printf("Power Ctrl Format Fail.\n");
                System_flush();
            #endif
        }

        power_control = rxBuffer[0];

        #ifdef CONSOLE
            System_printf("Power_control 0x%x.\n", power_control);
            System_flush();
        #endif

        /* ================================================== */
        /* ================ END CONFIGURATION =============== */
        /* ================================================== */


        /* Set up packet to receive data */
        txBuffer[0] = DATA_X0;
        dev.i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;
        dev.i2cTransaction.writeBuf = txBuffer;
        dev.i2cTransaction.writeCount = 1;
        dev.i2cTransaction.readBuf = rxBuffer;
        dev.i2cTransaction.readCount = 6;

        uint8_t i = 0;
        for(i = 0; i < ADXL_TRANSACTIONS; i++){
            if(!I2C_transfer(dev.i2c, &(dev.i2cTransaction))){
                #ifdef DEBUG
                    System_printf("Data Fail.\n");\
                    System_flush();
                #endif
                continue;
            }
                 x_lsb = rxBuffer[0];
                 x_msb = rxBuffer[1];
                 y_lsb = rxBuffer[2];
                 y_msb = rxBuffer[3];
                 z_lsb = rxBuffer[4];
                 z_msb = rxBuffer[5];
                 #ifdef CONSOLE
                     System_printf("X 0x%x %x.\n", x_msb, x_lsb);
                     System_printf("Y 0x%x %x.\n", y_msb, y_lsb);
                     System_printf("Z 0x%x %x.\n", z_msb, z_lsb);
                     System_flush();
                 #endif
        }

         I2C_close(dev.i2c);

         releaseI2CInterface(adxl);

         Task_sleep(TASK_SLEEP_TIME / Clock_tickPeriod);

     }
}
