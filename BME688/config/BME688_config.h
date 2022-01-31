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

/*! \defgroup BME_Configuration
 * @brief Group of functions and data-types called by the user to interact with BME688 device.
 */

/**
 * \ingroup BME_Configuration
 *
 * @brief Struct for a BME device.
 *  Used to keep all of the BME information in one place and organized and can be passed between functions while using BME device
 */
struct bme {

    /*! Struct from Bosch API for the BME device */
    struct bme68x_dev bme_dev;

    /*! Struct from Bosch API with all the configuration info */
    struct bme68x_conf conf;

    /*! Struct from Bosch API with all the configuration info for the gas heater */
    struct bme68x_heatr_conf heatr_conf;

    /*! Struct from Bosch API with all the data retrieved from the BME device */
    struct bme68x_data data;

    /*! Contains all the relevant i2c information */
    i2c_device i2c_dev;
};

/** \ingroup BME_Configuration
* Global pointer for current BME device
*/
struct bme *gBME;

/**
 * \ingroup BME_Configuration
 *
 * @brief Initialize the BME device
 *
 * @param bme pointer to the bme device to initialize
 *
 * @retval Status process status code
 */
uint8_t init_bme(struct bme *bme);

/**
 * \ingroup BME_Configuration
 *
 * @brief Configure the BME device
 *
 * @param bme pointer to the bme device to configure
 *
 * @retval Status process status code
 */
uint8_t config_bme(struct bme *bme);

/**
 * \ingroup BME_Configuration
 *
 * @brief Configure the gas heater on the BME device
 *
 * @param bme pointer to the bme device to configure
 *
 * @retval Status process status code
 */
uint8_t config_heatr(struct bme *bme);

/**
 * \ingroup BME_Configuration
 *
 * @brief Get the data from the BME device
 *
 * @param bme pointer to the bme device to retrieve data from
 *
 * @retval Fields how many fields of data were retrieved
 */
uint8_t get_data(struct bme *bme);

/* \ingroup BME_Configuration
 * @brief API functions from BOSCH to override
*/
int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t intf);
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
void bme68x_delay_us(uint32_t period, void *intf_ptr);
void _check_rslt(const char api_name[], int8_t rslt);

/** \ingroup BME_Configuration
 * @brief Points the global BME device to null
*/
void deinit_bme();

#endif /* BME_CONFIG_H_ */
