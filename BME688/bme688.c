/*
 * bme688.c
 *
 *  Created on: Jan 20, 2022
 *      Author: rjyar
 */

#include "bme688.h"

Void bmeTaskFxn(UArg arg0, UArg arg1)
{
    struct bme bme;

    if(init_bme(&bme)){
        System_printf("Error initializing bme \n");
        System_flush();
        return;
    }
    System_printf("Initialized BME\n");
    System_flush();

    while(1) {
        uint8_t fields = get_data(&bme);

#ifdef BME68X_USE_FPU
            System_printf("Float Here \n Temperature: %.2f C \n"
                          "Pressure: %.2f Pa\n"
                          "Humidity: %.2f%\n"
                          "Gas Resistance: \%.2fOhmns\n"
                          "Status: 0x%x\n",
                          bme.data.temperature,
                          bme.data.pressure,
                          bme.data.humidity,
                          bme.data.gas_resistance,
                          bme.data.status);
#else
            System_printf("Temperature: %u.%d C, "
                          "Pressure: %lu.%d hPa, "
                          "Humidity: %d%% "
                          "Gas Resistance: %lu Ohmns, "
                          "Status: 0x%x.\n",
                          (bme.data.temperature / 100),(bme.data.temperature % 100),
                          (long unsigned int)(bme.data.pressure / 100),(bme.data.pressure % 100),
                          (long unsigned int)(bme.data.humidity / 1000),
                          (long unsigned int)bme.data.gas_resistance,
                          bme.data.status);
#endif
            System_flush();

            Task_sleep(1000000 / Clock_tickPeriod);
    }
}


