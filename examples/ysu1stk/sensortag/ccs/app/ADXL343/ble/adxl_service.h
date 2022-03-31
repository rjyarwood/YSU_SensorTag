/******************************************************************************

 @file  adxl_service.h

 @brief This file contains ADXL Profile definitions.

    Author: RJ Yarwood

********************************************************************************/


#ifndef BLE_ADXL_H
#define BLE_ADXL_H

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
#define ADXL_DATA_DESCR       "ADXL. Data"
#define ADXL_CONFIG_DESCR     "ADXL. Conf."
#define ADXL_PERIOD_DESCR     "ADXL. Period"

// Service UUID
#define ADXL_SERV_UUID         0xAD00
#define ADXL_DATA_UUID         0xAD01
#define ADXL_CONF_UUID         0xAD02
#define ADXL_PERI_UUID         0xAD03

// Length of sensor data in bytes
#define ADXL_DATA_LEN          6

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
 * ADXL_addService- Initializes the Sensor GATT Profile service by registering
 *          GATT attributes with the GATT server.
 */
extern bStatus_t ADXL_addService(void);

/*
 * ADXL_registerAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ADXL_registerAppCBs(sensorCBs_t *appCallbacks);

/*
 * ADXL_setParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t ADXL_setParameter(uint8_t param, uint8_t len, void *value);

/*
 * ADXL_getParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to read.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t ADXL_getParameter(uint8_t param, void *value);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
