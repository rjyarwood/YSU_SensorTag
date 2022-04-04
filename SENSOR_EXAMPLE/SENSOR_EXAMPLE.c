/**
 *  @file SENSOR_EXAMPLE.c
 *
 *  @brief Where the SENSOR_EXAMPLE Task lives
 * 
 *  This is an example file meant to help someone create their own sensor. 
 *  Start by copying this file's contents and replacing SENSOR_EXAMPLE with
 *  the name of your sensor. This naming convention is not required but will 
 *  help you later.
 *
 *  Created on: Apr 4, 2022
 *      Author: RJ Yarwood
 */

/*********************************************************************
 * INCLUDES
 */

#include "./sensor_driver/bme688_driver.h"

#include "gatt.h"
#include "gattservapp.h"

#include "./ble/SENSOR_EXAMPLE_service.h"
#include "SENSOR_EXAMPLE.h"

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
 * \addtogroup SENSOR_EXAMPLE SENSOR EXAMPLE
 * @brief 
 * @{
 */

/// How often to perform sensor reads (milliseconds)
#define SENSOR_DEFAULT_PERIOD   1000

/// Delay from sensor enable to reading measurement (allow for 250 ms conversion time)
#define SENSOR_EXAMPLE_DELAY_PERIOD         275

/// Length of the data for this sensor
#define SENSOR_DATA_LEN         SENSOR_EXAMPLE_DATA_LEN

/// Task configuration
#define SENSOR_TASK_PRIORITY    1
#define SENSOR_TASK_STACK_SIZE  1024

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
/// Entity ID used to check for source and/or destination of messages
static ICall_EntityID sensorSelfEntity;

/// Semaphore used to post events to the application thread
static ICall_Semaphore sensorSem;

/// Task setup
static Task_Struct sensorTask;
static Char sensorTaskStack[SENSOR_TASK_STACK_SIZE];

/// Parameters
static uint8_t sensorConfig;
static uint16_t sensorPeriod;


/// Make a flag for your sensor so we know ICALL sees it before we try to use it
uint8_t SENSOR_EXAMPLERegistered = 0;

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
 * @fn      SENSOR_EXAMPLE_createTask
 *
 * @brief   Task creation function for the SENSOR_EXAMPLE sensor.
 *
 * @param   none
 *
 * @return  none
 */
void SENSOR_EXAMPLE_createTask(void)
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
 * @fn      SENSOR_EXAMPLE_processCharChangeEvt
 *
 * @brief   SENSOR_EXAMPLE event handling.
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 *
 */
void SENSOR_EXAMPLE_processCharChangeEvt(uint8_t paramID)
{
  uint8_t newValue;

  switch (paramID)
  {
  case SENSOR_CONF:
    if (sensorConfig != ST_CFG_ERROR)
    {
      SENSOR_EXAMPLE_getParameter(SENSOR_CONF, &newValue);

      if (newValue == ST_CFG_SENSOR_DISABLE)
      {
        // Reset characteristics
        initCharacteristicValue(SENSOR_DATA, 0xFF, SENSOR_DATA_LEN);

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
    SENSOR_EXAMPLE_getParameter(SENSOR_PERI, &newValue);
    sensorPeriod = newValue * SENSOR_PERIOD_RESOLUTION;
    break;

  default:
    // Should not get here
    break;
  }
}

/*******************************************************************//**
 * @fn      SENSOR_EXAMPLE_init
 *
 * @brief   Initialize the SENSOR_EXAMPLE service
 *
 * @param   none
 *
 * @return  none
 */
void SENSOR_EXAMPLE_init(void)
{
  // Add service
  SENSOR_EXAMPLE_addService();

  // Register callbacks with profile
  SENSOR_EXAMPLE_registerAppCBs(&sensorCallbacks);

  // Initialize the module state variables
  sensorPeriod = SENSOR_DEFAULT_PERIOD;

  // Initialize characteristics and sensor driver
  SENSOR_EXAMPLE_reset();
  initCharacteristicValue(SENSOR_PERI,
                          SENSOR_DEFAULT_PERIOD / SENSOR_PERIOD_RESOLUTION,
                          sizeof(uint8_t));


  SENSOR_EXAMPLERegistered = 1;
}

/*******************************************************************//**
 * @fn      SENSOR_EXAMPLE_reset
 *
 * @brief   Reset characteristics
 *
 * @param   none
 *
 * @return  none
 */
void SENSOR_EXAMPLE_reset(void)
{
  sensorConfig = ST_CFG_SENSOR_DISABLE;
  initCharacteristicValue(SENSOR_DATA, 0, SENSOR_EXAMPLE_DATA_LEN);
  initCharacteristicValue(SENSOR_CONF, sensorConfig, sizeof(uint8_t));
}


/*********************************************************************
* Private functions
*/

/*******************************************************************//**
 * @fn      sensorTaskFxn
 *
 * @brief   The task loop of the SENSOR_EXAMPLE readout task
 *
 * @param   a0 (not used)
 *
 * @param   a1 (not used)
 *
 * @return  none
 */
static void sensorTaskFxn(UArg a0, UArg a1)
{
  uint8_t data[SENSOR_EXAMPLE_DATA_LEN]; // Local data buffer

  if(!SENSOR_EXAMPLERegistered){ //If service is not registered give up the CPU.
      DELAY_MS(100);
  }

  // Register task with BLE stack
  ICall_registerApp(&sensorSelfEntity, &sensorSem);

  // Deactivate task (active only when measurement is enabled)
  Task_setPri(Task_handle(&sensorTask), -1);

  // Task loop
  while (true)
  {
    if (sensorConfig == ST_CFG_SENSOR_ENABLE)
    {

      ///Note: You could turn sensor on before reading then off immediatly after if power consumption is more important
      // Read data
      SENSOR_EXAMPLE_get_data(&data);

      // Update GATT
      SENSOR_EXAMPLE_setParameter(SENSOR_DATA, SENSOR_EXAMPLE_DATA_LEN, data);

      // Next cycle
      DELAY_MS(sensorPeriod - SENSOR_EXAMPLE_DELAY_PERIOD);
    }
    else
    {
      DELAY_MS(SENSOR_DEFAULT_PERIOD);
    }
  }
}


/*******************************************************************//**
 * @fn      sensorChangeCB
 *
 * @brief   Callback from BME Service indicating a value change
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 */
static void sensorConfigChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_SENSOR_EXAMPLE, paramID);
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
  uint8_t data[SENSOR_EXAMPLE_DATA_LEN];

  memset(data,value,paramLen);
  SENSOR_EXAMPLE_setParameter(paramID, paramLen, data);
}


/** @} */
