
#ifndef ADXL_H
#define ADXL_H

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
extern void ADXL_createTask(void);

/*
 * Initialize Temperature sensor module
 */
extern void ADXL_init(void);

/*
 * Task Event Processor for characteristic changes
 */
extern void ADXL_processCharChangeEvt(uint8_t paramID);

/*
 * Reset Temperature sensor module
 */
extern void ADXL_reset(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORTAGMOV_H */
