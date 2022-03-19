/*
 * bme688.h
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
 * Create the IR temperature sensor task
 */
extern void BME_createTask(void);

/*
 * Initialize Temperature sensor module
 */
extern void BME_init(void);

/*
 * Task Event Processor for characteristic changes
 */
extern void BME_processCharChangeEvt(uint8_t paramID);

/*
 * Reset Temperature sensor module
 */
extern void BME_reset(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* BME688_BME688_H_ */
