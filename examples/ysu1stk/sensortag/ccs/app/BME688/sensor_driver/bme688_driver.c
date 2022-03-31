/*
 *  @file bme688_driver.c
 *
 *  @brief Driver for the BME sensor
 *
 *  Created on: Jan 11, 2022
 *      Author: RJ Yarwood
 *
 */

#include "BME688_driver.h"


/** \ingroup BME_Configuration
* Global pointer for current BME device
*/
struct bme *gBME;

/* =========== INIT BME FUNCTION ============ */
uint8_t init_bme(struct bme *bme){

    #ifdef DEBUG
        System_printf("Initializing BME\n");
        System_flush();
    #endif

    // Set up the I2C parameters
    I2C_Params_init(&(bme->i2c_dev.i2cParams));
    bme->i2c_dev.i2cParams.bitRate = I2C_100kHz;

    // Variable to store the status codes
    uint8_t rslt;

    // Copy the pointer to the global version
    gBME = bme;

    rslt = bme68x_interface_init(&(bme->bme_dev), BME68X_I2C_INTF);
    _check_rslt("Interface_init", rslt);

    rslt = bme68x_init(&(bme->bme_dev));
    _check_rslt("bme68x_init", rslt);

    rslt = config_bme(bme);
    _check_rslt("set config", rslt);

    rslt = config_heatr(bme);
    _check_rslt("set heatr config", rslt);

    return rslt;
}



/* ========== CONFIGURE BME =========== */
uint8_t config_bme(struct bme *bme){
    bme->conf.filter = BME68X_FILTER_OFF;
    bme->conf.odr = BME68X_ODR_NONE;
    bme->conf.os_hum = BME68X_OS_16X;
    bme->conf.os_pres = BME68X_OS_1X;
    bme->conf.os_temp = BME68X_OS_2X;

    uint8_t rslt = bme68x_set_conf(&(bme->conf), &(bme->bme_dev));

    return rslt;
}


/* ========== CONFIGURE BME GAS HEATER =========== */
uint8_t config_heatr(struct bme *bme){
    bme->heatr_conf.enable = BME68X_ENABLE;
    bme->heatr_conf.heatr_temp = 300;
    bme->heatr_conf.heatr_dur = 100;
    uint8_t rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &(bme->heatr_conf), &(bme->bme_dev));

    return rslt;
}

/* ========== GET DATA FROM DEVICE ============ */
uint8_t get_data(struct bme *bme){
    uint8_t rslt; // Temp variable for status codes
    uint32_t del_period; // Temp variable for device delay
    uint8_t n_fields; // Temp variable for number of data fields retrieved
    bool gotData = false; // Flag to see if we got all the data

    while(!gotData){ // While we have not gotten enough data

        /* Set the operation mode of BME device */
        rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &(bme->bme_dev));
        _check_rslt("bme68x_set_op_mode", rslt);

        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &(bme->conf), &(bme->bme_dev)) + (bme->heatr_conf.heatr_dur * 1000);
        bme->bme_dev.delay_us(del_period, bme->bme_dev.intf_ptr);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        rslt = bme68x_get_data(BME68X_FORCED_MODE, &(bme->data), &n_fields, &(bme->bme_dev));
        _check_rslt("bme68x_get_data", rslt);

        if(n_fields){ // If we got any data we will exit
            gotData = true;
        }

    }
    return n_fields; // return how many data fields we got
}

/* ========== INITIALIZE I2C INTERFACE ============ */
int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t intf)
{
    int8_t rslt = BME68X_OK;


    uint8_t dev_addr = BME68X_I2C_ADDR_LOW;
    bme->read = bme68x_i2c_read;
    bme->write = bme68x_i2c_write;
    bme->intf = BME68X_I2C_INTF;
    bme->delay_us = bme68x_delay_us;
    bme->intf_ptr = &dev_addr;
    bme->amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */

    return rslt;
}



/* ========== I2C READ IMPLEMENTATION ============ */
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr){

    uint8_t dev_addr = BME68X_I2C_ADDR_LOW;

    gBME->i2c_dev.i2cTransaction.slaveAddress = dev_addr;
    gBME->i2c_dev.i2cTransaction.writeBuf = &reg_addr;
    gBME->i2c_dev.i2cTransaction.writeCount = 1;
    gBME->i2c_dev.i2cTransaction.readBuf = reg_data;
    gBME->i2c_dev.i2cTransaction.readCount = len;


    /* Lockout I2C */
    aquireI2CInterface(bme);

    /* Open I2C module */
    gBME->i2c_dev.i2c = I2C_open(Board_I2C_BME, &(gBME->i2c_dev.i2cParams));
    if(gBME->i2c_dev.i2c == NULL){
        #ifdef DEBUG
            System_printf("Error opening I2C in BME read");
            System_flush();
        #endif
        releaseI2CInterface(bme);
        return 1; // Return an error
    }


    /* Do actual transaction */
    if(!I2C_transfer(gBME->i2c_dev.i2c, &(gBME->i2c_dev.i2cTransaction))){
        #ifdef DEBUG
            System_printf("BME Read Fail.\n");
            System_flush();
        #endif
    }

    /* Close I2C interface */
    I2C_close(gBME->i2c_dev.i2c);

    /* Release the lock on I2C */
    releaseI2CInterface(bme);

    return 0;
}



/* ========== I2C WRITE IMPLEMENTATION ============ */
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr){

    uint8_t buffer[15];

    uint8_t dev_addr = BME68X_I2C_ADDR_LOW;

    buffer[0] = reg_addr;
    uint8_t i = 0;
    for(i = 0; i < len; i++){
        buffer[i+1] = reg_data[i];
    }

    gBME->i2c_dev.i2cTransaction.slaveAddress = dev_addr;
    gBME->i2c_dev.i2cTransaction.writeBuf = buffer;
    gBME->i2c_dev.i2cTransaction.writeCount = len+1;
    gBME->i2c_dev.i2cTransaction.readBuf = NULL;
    gBME->i2c_dev.i2cTransaction.readCount = 0;

    /* Lockout I2C */
    aquireI2CInterface(bme);

    /* Open I2C module */
    gBME->i2c_dev.i2c = I2C_open(Board_I2C_BME, &(gBME->i2c_dev.i2cParams));
    if(gBME->i2c_dev.i2c == NULL){
        #ifdef DEBUG
            System_printf("Error opening I2C in BME write");
            System_flush();
        #endif
        releaseI2CInterface(bme);
        return 1; // Return an error
    }


    /* Do actual transaction */
    if(!I2C_transfer(gBME->i2c_dev.i2c, &(gBME->i2c_dev.i2cTransaction))){
        #ifdef DEBUG
            System_printf("BME write Fail.\n");
            System_flush();
        #endif
    }

    /* Close I2C interface */
    I2C_close(gBME->i2c_dev.i2c);

    /* Release the lock on I2C */
    releaseI2CInterface(bme);

    return 0;
}

/* ========== DELAY IMPLEMENTATION ============ */
void bme68x_delay_us(uint32_t period, void *intf_ptr){

    #ifdef DEBUG
       System_printf("Waiting %d us", period);
       System_flush();
    #endif

   Task_sleep(period / Clock_tickPeriod);
}

/* ========== DEINIT BME ============ */
void deinit_bme(){
    gBME = NULL;
}

/* ========== READ BOSCH STATUS CODES ============ */
void _check_rslt(const char api_name[], int8_t rslt)
{
    #ifdef DEBUG
        switch (rslt)
        {
            case BME68X_OK:
                System_printf("API name [%s]  Good to go\r\n", api_name);
                System_flush();
                break;
            case BME68X_E_NULL_PTR:
                System_printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
                System_flush();
                break;
            case BME68X_E_COM_FAIL:
                System_printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
                System_flush();
                break;
            case BME68X_E_INVALID_LENGTH:
                System_printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
                System_flush();
                break;
            case BME68X_E_DEV_NOT_FOUND:
                System_printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
                System_flush();
                break;
            case BME68X_E_SELF_TEST:
                System_printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
                System_flush();
                break;
            case BME68X_W_NO_NEW_DATA:
                System_printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
                System_flush();
                break;
            default:
                System_printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
                System_flush();
                break;
        }
    #endif
}

