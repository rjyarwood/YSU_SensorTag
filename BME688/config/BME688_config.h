/*
 * BME_config.h
 *
 *  Created on: Jan 11, 2022
 *      Author: joaog
 */

#ifndef BME_CONFIG_H_
#define BME_CONFIG_H_

#include <BME688/api/BME688_api.h>
#include <ti/drivers/I2C.h>
#include "Board.h"
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/family/arm/cc26xx/Timer.h>

#include "i2c_interface/i2c_interface.h"

struct bme {
    struct bme68x_dev bme_dev;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data data;
    uint8_t test;

    I2C_Handle i2c;
    I2C_Params params;
    I2C_Transaction i2cTransaction;
};

Timer_Handle timer;
Timer_Params timer_params;
Error_Block eb_timer;
volatile bool timer_running;



struct bme *gBME;

uint8_t init_bme(struct bme *bme);
uint8_t config_bme(struct bme *bme);
uint8_t config_heatr(struct bme *bme);

uint8_t get_data(struct bme *bme);


int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t intf);
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
void bme68x_delay_us(uint32_t period, void *intf_ptr);
void _check_rslt(const char api_name[], int8_t rslt);

void deinit_bme(struct bme *bme);

#endif /* BME_CONFIG_H_ */
