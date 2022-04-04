/**
 *  @file bme688.h
 *
 *  @brief Defines anything for the BME Task
 *
 *  Created on: Mar 2, 2022
 *      Author: rjyar
 */

#ifndef BME688_BME688_H_
#define BME688_BME688_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "ysu_sensortag.h"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Create the BME sensor task
 */
extern void BME_createTask(void);

/*
 * Initialize BME sensor module
 */
extern void BME_init(void);

/*
 * Task Event Processor for characteristic changes
 */
extern void BME_processCharChangeEvt(uint8_t paramID);

/*
 * Reset BME sensor module
 */
extern void BME_reset(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* BME688_BME688_H_ */
