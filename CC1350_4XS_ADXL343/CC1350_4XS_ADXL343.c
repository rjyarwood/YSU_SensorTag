/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header files */
#include "Board.h"
#include "ADXL_defs.h"
#define TASKSTACKSIZE   512

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void taskFxn(UArg arg0, UArg arg1)
{
            int i;
            uint8_t         device_id, bw_rate, power_control, data_format;
            uint8_t         x_lsb, x_msb, y_lsb, y_msb, z_lsb, z_msb;
            uint8_t         txBuffer[8];
            uint8_t         rxBuffer[16];
            I2C_Handle      i2c;
            I2C_Params      i2cParams;
            I2C_Transaction i2cTransaction;

             /* Create I2C for usage */
             I2C_Params_init(&i2cParams);
             i2cParams.bitRate = I2C_100kHz;
             i2c = I2C_open(Board_I2C_ADXL, &i2cParams);
             if (i2c == NULL) {
             System_abort("Error Initializing I2C\n");
             }
             else {
                   System_printf("I2C Initialized!\n");
             }
             System_flush();
             Task_sleep(1000000 / Clock_tickPeriod);

             txBuffer[0] = 0x00;
             i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //BME688 ADDR;
             i2cTransaction.writeBuf = txBuffer;
             i2cTransaction.writeCount = 1;
             i2cTransaction.readBuf = rxBuffer;
             i2cTransaction.readCount = 1;
             if (I2C_transfer(i2c, &i2cTransaction)) {
                 device_id = rxBuffer[0];
                 System_printf("ID 0x%x.\n", device_id);
                 System_flush();
                 Task_sleep(1000000 / Clock_tickPeriod);
             }
             else{
                 System_printf("Configure Fail.\n");
             }
                          txBuffer[0] = 0x00;
                          i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //BME688 ADDR;
                          i2cTransaction.writeBuf = txBuffer;
                          i2cTransaction.writeCount = 1;
                          i2cTransaction.readBuf = rxBuffer;
                          i2cTransaction.readCount = 1;
                          if (I2C_transfer(i2c, &i2cTransaction)) {
                              device_id = rxBuffer[0];
                              System_printf("ID 0x%x.\n", device_id);
                              System_flush();
                              Task_sleep(1000000 / Clock_tickPeriod);
                          }
                          else{
                              System_printf("Configure Fail.\n");
                          }
                          txBuffer[0] = BW_RATE;
                          txBuffer[1] = 0x0A;
                          i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //BME688 ADDR;
                          i2cTransaction.writeBuf = txBuffer;
                          i2cTransaction.writeCount = 2;
                          i2cTransaction.readBuf = rxBuffer;
                          i2cTransaction.readCount = 1;
                          if (I2C_transfer(i2c, &i2cTransaction)) {
                              bw_rate = rxBuffer[0];
                              System_printf("BW RATE 0x%x.\n", bw_rate);
                              System_flush();
                              Task_sleep(1000000 / Clock_tickPeriod);
                          }
                          else{
                              System_printf("Configure Fail.\n");
                          }

                          txBuffer[0] = DATA_FORMAT;
                          txBuffer[1] = RANGE_16G;
                          i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //BME688 ADDR;
                          i2cTransaction.writeBuf = txBuffer;
                          i2cTransaction.writeCount = 2;
                          i2cTransaction.readBuf = rxBuffer;
                          i2cTransaction.readCount = 6;
                          if (I2C_transfer(i2c, &i2cTransaction)) {
                              data_format = rxBuffer[0];
                              System_printf("Data format 0x%x.\n", data_format);
                              Task_sleep(1000000 / Clock_tickPeriod);
                          }
                          else{
                              System_printf("Configure Fail.\n");
                          }

                          txBuffer[0] = POWER_CTL;
                          txBuffer[1] = ADXL_MEASURE;
                          i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //ADXL343 ADDR;
                          i2cTransaction.writeBuf = txBuffer;
                          i2cTransaction.writeCount = 2;
                          i2cTransaction.readBuf = rxBuffer;
                          i2cTransaction.readCount = 1;
                          if (I2C_transfer(i2c, &i2cTransaction)) {
                          power_control = rxBuffer[0];
                          System_printf("Power_control 0x%x.\n", power_control);
                          Task_sleep(1000000 / Clock_tickPeriod);
                          }
                          else{
                          System_printf("Configure Fail.\n");
                          }

                          for(i = 0; i < 20; i++){
                          txBuffer[0] = DATA_X0;
                          i2cTransaction.slaveAddress = ADXL_SLAVE_ADDR;     //ADXL343 ADDR;
                          i2cTransaction.writeBuf = txBuffer;
                          i2cTransaction.writeCount = 1;
                          i2cTransaction.readBuf = rxBuffer;
                          i2cTransaction.readCount = 6;
                          if (I2C_transfer(i2c, &i2cTransaction)) {
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
                          else{
                          System_printf("Read data fail.\n");
                          }
                          }

             /* De-initialized I2C */
              I2C_close(i2c);
              System_printf("I2C closed!\n");
              System_flush();
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();
    // Board_initSPI();
    // Board_initUART();
    // Board_initWatchdog();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000000 / Clock_tickPeriod;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle) {
        System_abort("Error initializing board LED pins\n");
    }

    PIN_setOutputValue(ledPinHandle, Board_LED0, 1);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
