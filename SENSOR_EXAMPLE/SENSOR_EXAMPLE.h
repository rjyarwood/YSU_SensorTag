/**
 *  @file SENSOR_EXAMPLE.h
 *
 *  @brief Defines anything for to initialize SENSOR_EXAMPLE
 * 
 * 
 *  This is an example file meant to help someone create their own sensor. 
 *  Start by copying this file's contents and replacing SENSOR_EXAMPLE with
 *  the name of your sensor. This naming convention is not required but will 
 *  help you later.
 *
 *  Created on: Apr 4, 2022
 *      Author: RJ YArwood
 */

#ifndef SENSOR_EXAMPLE_SENSOR_EXAMPLE_H_
#define SENSOR_EXAMPLE_SENSOR_EXAMPLE_H_


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
 * Create the SENSOR_EXAMPLE sensor task
 */
extern void SENSOR_EXAMPLE_createTask(void);

/*
 * Initialize SENSOR_EXAMPLE sensor module
 */
extern void SENSOR_EXAMPLE_init(void);

/*
 * Task Event Processor for characteristic changes
 */
extern void SENSOR_EXAMPLE_processCharChangeEvt(uint8_t paramID);

/*
 * Reset SENSOR_EXAMPLE sensor module
 */
extern void SENSOR_EXAMPLE_reset(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_EXAMPLE_SENSOR_EXAMPLE_H_ */
