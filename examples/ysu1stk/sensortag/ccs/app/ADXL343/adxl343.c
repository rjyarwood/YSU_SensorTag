/**
 *  @file adxl343.c
 *
 *  @brief Where the ADXL Task lives
 *
 *  Created on: Feb 11, 2022
 *      Author: RJ Yarwood
 */
/*********************************************************************
 * INCLUDES
 */
#include "gatt.h"
#include "gattservapp.h"

#include "./ble/adxl_service.h"
#include "adxl343.h"
#include "./sensor_driver/adxl_sensor.h"

#include "SensorUtils.h"
#include "board.h"

#include "string.h"
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/// \addtogroup sensortag

/**
 * \ingroup sensortag
 * \addtogroup adxl ADXL
 * 
 * @{
 */

/// How often to perform sensor reads (milliseconds)
#define SENSOR_DEFAULT_PERIOD   1000

/// Time start measurement and data ready
#define ADXL_DELAY_PERIOD        15

/// Length of the data for this sensor
#define SENSOR_DATA_LEN         ADXL_DATA_LEN

/// Task configuration
#define SENSOR_TASK_PRIORITY    1
#define SENSOR_TASK_STACK_SIZE  600

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

/// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID sensorSelfEntity;

/// Semaphore globally used to post events to the application thread
static ICall_Semaphore sensorSem;

/// Task setup
static Task_Struct sensorTask;
static Char sensorTaskStack[SENSOR_TASK_STACK_SIZE];

/// Parameters
static uint8_t sensorConfig;
static uint16_t sensorPeriod;


/// Make a flag for your sensor so we know ICALL sees it before we try to use it
uint8_t adxlRegistered = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void sensorTaskFxn(UArg a0, UArg a1);
static void sensorConfigChangeCB(uint8_t paramID);
static void initCharacteristicValue(uint8_t paramID, uint8_t value,
                                    uint8_t paramLen);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static sensorCBs_t sensorCallbacks =
{
  sensorConfigChangeCB,  /**< Characteristic value change callback */
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*******************************************************************//**
 * @fn      ADXL_createTask
 *
 * @brief   Task creation function for the ADXL
 *
 * @param   none
 *
 * @return  none
 */
void ADXL_createTask(void)
{
  Task_Params taskParams;

  // Create the task for the state machine
  Task_Params_init(&taskParams);
  taskParams.stack = sensorTaskStack;
  taskParams.stackSize = SENSOR_TASK_STACK_SIZE;
  taskParams.priority = SENSOR_TASK_PRIORITY;

  Task_construct(&sensorTask, sensorTaskFxn, &taskParams, NULL);
}

/*******************************************************************//**
 * @fn      ADXL_processCharChangeEvt
 *
 * @brief   SensorTag ADXL event handling
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 *
 */
void ADXL_processCharChangeEvt(uint8_t paramID)
{
  uint8_t newValue;

  switch (paramID)
  {
  case SENSOR_CONF:
    if (sensorConfig != ST_CFG_ERROR)
    {
      ADXL_getParameter(SENSOR_CONF, &newValue);

      if (newValue == ST_CFG_SENSOR_DISABLE)
      {
        // Reset characteristics
        initCharacteristicValue(SENSOR_DATA, 0, SENSOR_DATA_LEN);

        // Deactivate task
        Task_setPri(Task_handle(&sensorTask), -1);
      }
      else
      {
        Task_setPri(Task_handle(&sensorTask), SENSOR_TASK_PRIORITY);
      }

      sensorConfig = newValue;
    }
    else
    {
      // Make sure the previous characteristics value is restored
      initCharacteristicValue(SENSOR_CONF, sensorConfig, sizeof(uint8_t));
    }

    break;

  case SENSOR_PERI:
    ADXL_getParameter(SENSOR_PERI, &newValue);
    sensorPeriod = newValue * SENSOR_PERIOD_RESOLUTION;
    break;

  default:
    // Should not get here
    break;
  }
}

/*******************************************************************//**
 * @fn      ADXL_init
 *
 * @brief   Initialize the module
 *
 * @param   none
 *
 * @return  none
 */
void ADXL_init(void)
{
  // Add service
  ADXL_addService();

  // Register callbacks with profile
  ADXL_registerAppCBs(&sensorCallbacks);

  // Initialize the module state variables
  sensorPeriod = SENSOR_DEFAULT_PERIOD;

  // Initialize characteristics and adxl driver
  ADXL_reset();
  initCharacteristicValue(SENSOR_PERI,
                          SENSOR_DEFAULT_PERIOD / SENSOR_PERIOD_RESOLUTION,
                          sizeof(uint8_t));

  adxlRegistered = 1;
}

/*******************************************************************//**
 * @fn      ADXL_reset
 *
 * @brief   Reset characteristics
 *
 * @param   none
 *
 * @return  none
 */
void ADXL_reset(void)
{
  sensorConfig = ST_CFG_SENSOR_DISABLE;
  initCharacteristicValue(SENSOR_DATA, 0, ADXL_DATA_LEN);
  initCharacteristicValue(SENSOR_CONF, sensorConfig, sizeof(uint8_t));
}


/*********************************************************************
* Private functions
*/

/*******************************************************************//**
 * @fn      sensorTaskFxn
 *
 * @brief   The task loop of the ADXL readout task
 *
 * @param   a0 (not used)
 *
 * @param   a1 (not used)
 *
 * @return  none
 */
static void sensorTaskFxn(UArg a0, UArg a1)
{
  uint8_t data[ADXL_DATA_LEN];

  if(!adxlRegistered){
     DELAY_MS(100);
  }
  // Register task with BLE stack
  ICall_registerApp(&sensorSelfEntity, &sensorSem);

  // Deactivate task (active only when measurement is enabled)
  Task_setPri(Task_handle(&sensorTask), -1);

  // Task loop
  while (true)
  {
      bStatus_t ret = SUCCESS;
     data[0] = 0;
    if (sensorConfig == ST_CFG_SENSOR_ENABLE)
    {

      data[0] = 0xFF;
      // Read data
      ADXL_read(data);

      // Update GATT
      ret = ADXL_setParameter(SENSOR_DATA, ADXL_DATA_LEN, data);


      // Next cycle
      DELAY_MS(sensorPeriod - ADXL_DELAY_PERIOD);
    }
    else
    {
      DELAY_MS(SENSOR_DEFAULT_PERIOD);
    }
  }
}


/*******************************************************************//**
 * @fn      sensorConfigChangeCB
 *
 * @brief   Callback from ADXL Service indicating a value change
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 */
static void sensorConfigChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_ADXL, paramID);
}


/*******************************************************************//**
 * @fn      initCharacteristicValue
 *
 * @brief   Initialize a characteristic value
 *
 * @param   paramID - parameter ID of the value to be initialized
 *
 * @param   value - value to initialize with
 *
 * @param   paramLen - length of the parameter
 *
 * @return  none
 */
static void initCharacteristicValue(uint8_t paramID, uint8_t value,
                                    uint8_t paramLen)
{
  uint8_t data[ADXL_DATA_LEN];

  memset(data,value,paramLen);
  ADXL_setParameter(paramID, paramLen, data);
}

/** @} */
