/*
 * bme_config.c
 *
 *  Created on: Jan 11, 2022
 *
 */

#include "BME688_config.h"

int fxn_timer_callback(){

    timer_running = false;

    return timer_running;
}

void _create_timer(){
     Error_init( &eb_timer );
     Timer_Params_init( &timer_params );
     timer_params.period = 500000;
     timer_params.periodType = Timer_PeriodType_MICROSECS;
     timer_params.startMode = Timer_StartMode_USER;
     timer_params.runMode = Timer_RunMode_ONESHOT;
     timer = Timer_create( 1,  (ti_sysbios_interfaces_ITimer_FuncPtr)fxn_timer_callback(), &timer_params, &eb_timer );
}

uint8_t init_bme(struct bme *bme){

    System_printf("Initializing BME\n");
    System_flush();

    _create_timer();

    I2C_Params params;

    I2C_Params_init(&(bme->params));
    bme->params.bitRate = I2C_100kHz;

    uint8_t rslt;


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

uint8_t config_bme(struct bme *bme){
    bme->conf.filter = BME68X_FILTER_OFF;
    bme->conf.odr = BME68X_ODR_NONE;
    bme->conf.os_hum = BME68X_OS_16X;
    bme->conf.os_pres = BME68X_OS_1X;
    bme->conf.os_temp = BME68X_OS_2X;

    uint8_t rslt = bme68x_set_conf(&(bme->conf), &(bme->bme_dev));

    return rslt;
}

uint8_t config_heatr(struct bme *bme){
    bme->heatr_conf.enable = BME68X_ENABLE;
    bme->heatr_conf.heatr_temp = 300;
    bme->heatr_conf.heatr_dur = 100;
    uint8_t rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &(bme->heatr_conf), &(bme->bme_dev));

    return rslt;
}

uint8_t get_data(struct bme *bme){
    uint8_t rslt;
    uint32_t del_period;
    uint8_t n_fields;
    bool gotData = false;

    System_printf("In get_data \n");
    System_flush();
    while(!gotData){

        System_printf("About to set op_mode \n");
        System_flush();
        rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &(bme->bme_dev));
        _check_rslt("bme68x_set_op_mode", rslt);

        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &(bme->conf), &(bme->bme_dev)) + (bme->heatr_conf.heatr_dur * 1000);
        bme->bme_dev.delay_us(del_period, bme->bme_dev.intf_ptr);


        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        rslt = bme68x_get_data(BME68X_FORCED_MODE, &(bme->data), &n_fields, &(bme->bme_dev));
        _check_rslt("bme68x_get_data", rslt);

        if(n_fields){
            gotData = true;
        }

    }
    return n_fields;
}


int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t intf)
{
    int8_t rslt = BME68X_OK;


    uint8_t dev_addr = BME68X_I2C_ADDR_LOW;
    bme->read = bme68x_i2c_read;
    bme->write = bme68x_i2c_write;
    bme->intf = BME68X_I2C_INTF;
    bme->delay_us = bme68x_delay_us;
    bme->intf_ptr = dev_addr;
    bme->amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */

    return rslt;

}




BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr){

    uint8_t dev_addr = BME68X_I2C_ADDR_LOW;

    gBME->i2cTransaction.slaveAddress = dev_addr;
    gBME->i2cTransaction.writeBuf = &reg_addr;
    gBME->i2cTransaction.writeCount = 1;
    gBME->i2cTransaction.readBuf = reg_data;
    gBME->i2cTransaction.readCount = len;


    while(adxl_busy) {
        System_printf("Waiting on ADXL\n");
        System_flush();
        Task_sleep(1000000 / Clock_tickPeriod);} //Sleep until we can use the i2c interface

    /* Lock the I2C and do what we need to do */
    bme_busy = 1;
    int f = 0;
    do {
        if(f){
            System_printf("BME Read I2C failed to initialize!\n");
            System_flush();
        }
        gBME->i2c = I2C_open(Board_I2C_BME, &(gBME->params));
        f++;
    } while(gBME->i2c == NULL);


    while(!I2C_transfer(gBME->i2c, &(gBME->i2cTransaction))){
        System_printf("BME Read Fail.\n");\
        System_flush();
    }

    I2C_close(gBME->i2c);

    bme_busy = 0;

    return 0;
}

BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr){

    uint8_t buffer[15];

    uint8_t dev_addr = BME68X_I2C_ADDR_LOW;

    buffer[0] = reg_addr;
    uint8_t i = 0;
    for(i = 0; i < len; i++){
        buffer[i+1] = reg_data[i];
    }

    if(gBME == NULL){
        System_abort("Problem");
    }

    gBME->i2cTransaction.slaveAddress = dev_addr;
    gBME->i2cTransaction.writeBuf = buffer;
    gBME->i2cTransaction.writeCount = len+1;
    gBME->i2cTransaction.readBuf = NULL;
    gBME->i2cTransaction.readCount = 0;

    while(adxl_busy) {
        System_printf("Waiting on ADXL\n");
        System_flush();
        Task_sleep(1000000 / Clock_tickPeriod);} //Sleep until we can use the i2c interface

    /* Lock the I2C and do what we need to do */
    bme_busy = 1;

    int f = 0;
    do {
        if(f){
            System_printf("BME Write I2C failed to initialize!\n");
            System_flush();
        }
        gBME->i2c = I2C_open(Board_I2C_BME, &(gBME->params));
        f++;
    } while(gBME->i2c == NULL);


    while(!I2C_transfer(gBME->i2c, &(gBME->i2cTransaction))){
        System_printf("BME Write Fail.\n");\
        System_flush();
    }

    I2C_close(gBME->i2c);

    bme_busy = 0;

    return 0;
}


void bme68x_delay_us(uint32_t period, void *intf_ptr){

   System_printf("Waiting %d us", period);
   System_flush();

   bme_busy = 1;
   Task_sleep(period / Clock_tickPeriod);
}



void _check_rslt(const char api_name[], int8_t rslt)
{
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
}

void deinit_bme(struct bme *bme){
    gBME = NULL;
}
