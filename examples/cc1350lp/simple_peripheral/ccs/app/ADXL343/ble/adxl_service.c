/**********************************************************************************************
 *
 * @file adxl_service.c
 *
 * @brief ADXL BLE service
 *
 */
/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "string.h"

#include "adxl_service.h"
#include "st_util.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// The temperature adxl does not support the 100 ms update rate
#undef ADXL_MIN_UPDATE_PERIOD
#define ADXL_MIN_UPDATE_PERIOD  300 // Minimum 300 milliseconds

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Service UUID
static CONST uint8_t adxlServiceUUID[TI_UUID_SIZE] =
{
  TI_UUID(ADXL_SERV_UUID),
};

// Characteristic UUID: data
static CONST uint8_t adxlDataUUID[TI_UUID_SIZE] =
{
  TI_UUID(ADXL_DATA_UUID),
};

// Characteristic UUID: config
static CONST uint8_t adxlCfgUUID[TI_UUID_SIZE] =
{
  TI_UUID(ADXL_CONF_UUID),
};

// Characteristic UUID: period
static CONST uint8_t adxlPeriodUUID[TI_UUID_SIZE] =
{
  TI_UUID(ADXL_PERI_UUID),
};


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static sensorCBs_t *adxl_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Profile Service attribute
static CONST gattAttrType_t adxlService = { TI_UUID_SIZE, adxlServiceUUID };

// Characteristic Value: data
static uint8_t adxlData[ADXL_DATA_LEN] = { 0, 0, 0, 0, 0, 0};

// Characteristic Properties: data
static uint8_t adxlDataProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic Configuration: data
static gattCharCfg_t *adxlDataConfig;

#ifdef USER_DESCRIPTION
// Characteristic User Description: data
static uint8_t adxlDataUserDescr[] = ADXL_DATA_DESCR;
#endif

// Characteristic Properties: configuration
static uint8_t adxlCfgProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic Value: configuration
static uint8_t adxlCfg = 0;

#ifdef USER_DESCRIPTION
// Characteristic User Description: configuration
static uint8_t adxlCfgUserDescr[] = ADXL_CONFIG_DESCR;
#endif

// Characteristic Properties: period
static uint8_t adxlPeriodProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic Value: period
static uint8_t adxlPeriod = ADXL_MIN_UPDATE_PERIOD / SENSOR_PERIOD_RESOLUTION;

#ifdef USER_DESCRIPTION
// Characteristic User Description: period
static uint8_t adxlPeriodUserDescr[] = ADXL_PERIOD_DESCR;
#endif

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t adxlAttrTable[] =
{
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8_t *)&adxlService                 /* pValue */
  },

    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &adxlDataProps
    },

      // Characteristic Value "Data"
      {
        { TI_UUID_SIZE, adxlDataUUID },
        GATT_PERMIT_READ,
        0,
        adxlData
      },

      // Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&adxlDataConfig
      },

#ifdef USER_DESCRIPTION
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        adxlDataUserDescr
      },
#endif
    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &adxlCfgProps
    },

      // Characteristic Value "Configuration"
      {
        { TI_UUID_SIZE, adxlCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &adxlCfg
      },

#ifdef USER_DESCRIPTION
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        adxlCfgUserDescr
      },
#endif
     // Characteristic Declaration "Period"
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &adxlPeriodProps
    },

      // Characteristic Value "Period"
      {
        { TI_UUID_SIZE, adxlPeriodUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &adxlPeriod
      },

#ifdef USER_DESCRIPTION
      // Characteristic User Description "Period"
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        adxlPeriodUserDescr
      },
#endif
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t adxl_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method);
static bStatus_t adxl_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// IR Temperature Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an
// operation on a characteristic requires authorization the Stack will call
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be
// made within these functions.
static CONST gattServiceCBs_t adxlCBs =
{
  adxl_ReadAttrCB,  // Read callback function pointer
  adxl_WriteAttrCB, // Write callback function pointer
  NULL                // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      IRTemp_addService
 *
 * @brief   Initializes the IR Temperature Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @return  Success or Failure
 */
bStatus_t ADXL_addService(void)
{
  // Allocate Client Characteristic Configuration table
  adxlDataConfig = (gattCharCfg_t *)ICall_malloc(sizeof(gattCharCfg_t) *
                                                    linkDBNumConns);
  if (adxlDataConfig == NULL)
  {
    return (bleMemAllocError);
  }

  // Register with Link DB to receive link status change callback
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, adxlDataConfig);

  // Register GATT attribute list and CBs with GATT Server App
  return GATTServApp_RegisterService(adxlAttrTable,
                                      GATT_NUM_ATTRS (adxlAttrTable),
                                      GATT_MAX_ENCRYPT_KEY_SIZE,
                                      &adxlCBs);
}


/*********************************************************************
 * @fn      IRTemp_registerAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t ADXL_registerAppCBs(sensorCBs_t *appCallbacks)
{
  if (adxl_AppCBs == NULL)
  {
    if (appCallbacks != NULL)
    {
      adxl_AppCBs = appCallbacks;
    }

    return (SUCCESS);
  }

  return (bleAlreadyInRequestedMode);
}

/*********************************************************************
 * @fn      IRTemp_setParameter
 *
 * @brief   Set a parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ADXL_setParameter(uint8_t param, uint8_t len, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (param)
  {
    case SENSOR_DATA:
    if (len == ADXL_DATA_LEN)
    {
      memcpy(adxlData, value, ADXL_DATA_LEN);
      // See if Notification has been enabled
      ret = GATTServApp_ProcessCharCfg(adxlDataConfig, adxlData, FALSE,
                                 adxlAttrTable,
                                 GATT_NUM_ATTRS(adxlAttrTable),
                                 INVALID_TASK_ID, adxl_ReadAttrCB);
    }
    else
    {
      ret = bleInvalidRange;
    }
    break;

    case SENSOR_CONF:
      if (len == sizeof(uint8_t))
      {
        adxlCfg = *((uint8_t*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SENSOR_PERI:
      if (len == sizeof(uint8_t))
      {
        adxlPeriod = *((uint8_t*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
}

/*********************************************************************
 * @fn      IRTemp_getParameter
 *
 * @brief   Get a Sensor Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ADXL_getParameter(uint8_t param, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (param)
  {
    case SENSOR_DATA:
      memcpy(value, adxlData, ADXL_DATA_LEN);
      break;

    case SENSOR_CONF:
      *((uint8_t*)value) = adxlCfg;
      break;

    case SENSOR_PERI:
      *((uint8_t*)value) = adxlPeriod;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
}

/*********************************************************************
 * @fn          adxl_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t adxl_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method)
{
  uint16_t uuid;
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if (offset > 0)
  {
    return (ATT_ERR_ATTR_NOT_LONG);
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    *pLen = 0;
    return ATT_ERR_INVALID_HANDLE;
  }

  switch (uuid)
  {
    // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
    // gattserverapp handles those reads
    case ADXL_DATA_UUID:
      *pLen = ADXL_DATA_LEN;
      memcpy(pValue, pAttr->pValue, ADXL_DATA_LEN);
      break;

    case ADXL_CONF_UUID:
    case ADXL_PERI_UUID:
      *pLen = 1;
      pValue[0] = *pAttr->pValue;
      break;

    default:
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
    }

  return (status);
}

/*********************************************************************
 * @fn      adxl_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t adxl_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8_t notifyApp = 0xFF;
  uint16_t uuid;

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  switch (uuid)
  {
    case ADXL_DATA_UUID:
      // Should not get here
      break;

    case ADXL_CONF_UUID:
      // Validate the value
      // Make sure it's not a blob oper
      if (offset == 0)
      {
        if (len != 1)
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if (status == SUCCESS)
      {
        uint8_t *pCurValue = (uint8_t *)pAttr->pValue;

        *pCurValue = pValue[0];

        if (pAttr->pValue == &adxlCfg)
        {
          notifyApp = SENSOR_CONF;
        }
      }
      break;

    case ADXL_PERI_UUID:
      // Validate the value
      // Make sure it's not a blob oper
      if (offset == 0)
      {
        if (len != 1)
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }
      // Write the value
      if (status == SUCCESS)
      {
        if (pValue[0]>=(ADXL_MIN_UPDATE_PERIOD/SENSOR_PERIOD_RESOLUTION))
        {

          uint8_t *pCurValue = (uint8_t *)pAttr->pValue;
          *pCurValue = pValue[0];

          if (pAttr->pValue == &adxlPeriod)
          {
            notifyApp = SENSOR_PERI;
          }
        }
        else
        {
           status = ATT_ERR_INVALID_VALUE;
        }
      }
      break;

    case GATT_CLIENT_CHAR_CFG_UUID:
      status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                              offset, GATT_CLIENT_CFG_NOTIFY);
      break;

    default:
      // Should never get here!
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
  }

  // If a characteristic value changed then callback function
  // to notify application of change
  if ((notifyApp != 0xFF) && adxl_AppCBs && adxl_AppCBs->pfnSensorChange)
  {
    adxl_AppCBs->pfnSensorChange(notifyApp);
  }

  return (status);
}

/*********************************************************************
*********************************************************************/

