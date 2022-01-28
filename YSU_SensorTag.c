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

/* Board Header files */
#include "Board.h"

#include "ADXL343/adxl343.h"
#include "BME688/bme688.h"

#define TASKSTACKSIZE   1024

Task_Struct adxlTaskStruct;
Char adxlTaskStack[TASKSTACKSIZE];

Task_Struct bmeTaskStruct;
Char bmeTaskStack[TASKSTACKSIZE];



/*
 *  ======== main ========
 */
int main(void){
    Task_Params adxlTaskParams;
    Task_Params bmeTaskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&adxlTaskParams);
    adxlTaskParams.arg0 = 10000000 / Clock_tickPeriod;
    adxlTaskParams.stackSize = TASKSTACKSIZE;
    adxlTaskParams.stack = &adxlTaskStack;
    adxlTaskParams.priority = 2;
    Task_construct(&adxlTaskStruct, (Task_FuncPtr)adxlTaskFxn, &adxlTaskParams, NULL);


    Task_Params_init(&bmeTaskParams);
    bmeTaskParams.arg0 = 10000000 / Clock_tickPeriod;
    bmeTaskParams.stackSize = TASKSTACKSIZE;
    bmeTaskParams.stack = &bmeTaskStack;
    bmeTaskParams.priority = 2;
    Task_construct(&bmeTaskStruct, (Task_FuncPtr)bmeTaskFxn, &bmeTaskParams, NULL);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
