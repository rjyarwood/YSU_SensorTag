/**
 *  @file: ysu_sensortag.h
 *
 *  @brief: This is where you will add a service ID for your new sensor and where the main task has its function definitions.
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

/** 
 * \addtogroup sensortag YSU SensorTag 
 * @{
 */

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
/**
 * \addtogroup sensortag_ctrl SensorTag Control
 * @{
 */
/************************* BEGIN USER CODE 3 ************************/
/**
 *  \addtogroup service_ids Service IDs
 *  @brief Service ID's for internal application use
 * 
 *  When adding a service, you should create a define here so that we can direct mesages to the correct place
 *  Number does not matter but for simplicity just go in order. CC is to help maintain a steady connection, do not remove
 * @{
 */
//TODO: USER CODE 3: Add your service ID here for the BLE stack to route to the right service
#define SERVICE_ID_ADXL       0x01 /**< Flag to identify ADXL BLE service */
#define SERVICE_ID_BME        0x02 /**< Flag to identify BME BLE service  */

#define SERVICE_ID_CC         0x09 /**< Flag to identify Connection Control BLE service  */
/** @} */
/** @} */
/************************** END USER CODE 3 *************************/
/** @} */


/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * VARIABLES
 */

extern ICall_Semaphore sem;                 /**< Semaphore used for the main thread */
extern gaprole_States_t gapProfileState;    /**< State of the GAP Profile */
extern ICall_EntityID selfEntityMain;       /**< ID to help route notifications to the correct task */


extern PIN_State pinGpioState;
extern PIN_Handle hGpioPin;


/*********************************************************************
 * FUNCTIONS
 */

extern void SensorTag_createTask(void);

extern void SensorTag_charValueChangeCB(uint8_t sensorID, uint8_t paramID);

extern void SensorTag_applyFactoryImage(void);

extern void SensorTag_updateAdvertisingData(uint8_t keyStatus);

extern uint8_t SensorTag_testResult(void);




/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif



#endif /* YSU_SENSORTAG_YSU_SENSORTAG_H_ */
