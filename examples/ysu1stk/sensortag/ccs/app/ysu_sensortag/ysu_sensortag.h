/*
 *  @file: ysu_sensortag.h
 *
 *  @brief: This is where you will add a service ID for your new sensor and where the main task has its function definitions.
 *
 *
 *  Created on: Feb 11, 2022
 *      Author: rjyar
 */

#ifndef YSU_SENSORTAG_YSU_SENSORTAG_H_
#define YSU_SENSORTAG_YSU_SENSORTAG_H_



#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ICall.h"
#include "peripheral.h"

#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/PIN.h>


/*********************************************************************
 * CONSTANTS
 */

/************************* BEGIN USER CODE 3 ************************/
///TODO: USER CODE 3: Add your service ID here for the BLE stack to route to the right service
// Service ID's for internal application use
#define SERVICE_ID_ADXL       0x01
#define SERVICE_ID_BME        0x02
#define SERVICE_ID_CC         0x09
/************************** END USER CODE 3 *************************/



/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * VARIABLES
 */
extern ICall_Semaphore sem;
extern gaprole_States_t gapProfileState;
extern ICall_EntityID selfEntityMain;
extern PIN_State pinGpioState;
extern PIN_Handle hGpioPin;


/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for SensorTag
 */
extern void SensorTag_createTask(void);

/*
 * Function to call when a characteristic value has changed
 */
extern void SensorTag_charValueChangeCB(uint8_t sensorID, uint8_t paramID);

/*
 * Function to load the factory image and reboot it
 */
extern void SensorTag_applyFactoryImage(void);

/*
 * Update the advertising data with the latest key press status
 */
extern void SensorTag_updateAdvertisingData(uint8_t keyStatus);

/*
 * Return the self-test result
 */
extern uint8_t SensorTag_testResult(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif



#endif /* YSU_SENSORTAG_YSU_SENSORTAG_H_ */
