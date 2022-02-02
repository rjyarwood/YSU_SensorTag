/*
 * bme688.h
 *
 *  Created on: Jan 20, 2022
 *      Author: rjyar
 */

#ifndef BME688_BME688_H_
#define BME688_BME688_H_


#include "api/BME688_api.h"
#include "config/BME688_config.h"

/**
 * @brief Main task function for BME device. 
 * 
 * Called from BIOS
 * 
 */
Void bmeTaskFxn(UArg arg0, UArg arg1);

#endif /* BME688_BME688_H_ */
