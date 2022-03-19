/*
 * bme688service.h
 *
 *  Created on: Mar 3, 2022
 *      Author: rjyar
 */

#ifndef BME688_BLE_BME688SERVICE_H_
#define BME688_BLE_BME688SERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "st_util.h"

/*********************************************************************
 * CONSTANTS
 */

// Service UUID
#define BME_SERV_UUID         0xBF00
#define BME_DATA_UUID         0xBF01
#define BME_CONF_UUID         0xBF02
#define BME_PERI_UUID         0xBF03

// Length of sensor data in bytes
#define BME_DATA_LEN          4

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */


/*
 * IRTemp_addService- Initializes the Sensor GATT Profile service by registering
 *          GATT attributes with the GATT server.
 */
extern bStatus_t BME_addService(void);

/*
 * IRTemp_registerAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t BME_registerAppCBs(sensorCBs_t *appCallbacks);

/*
 * IRTemp_setParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t BME_setParameter(uint8_t param, uint8_t len, void *value);

/*
 * IRTemp_getParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to read.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t BME_getParameter(uint8_t param, void *value);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif /* BME688_BLE_BME688SERVICE_H_ */
