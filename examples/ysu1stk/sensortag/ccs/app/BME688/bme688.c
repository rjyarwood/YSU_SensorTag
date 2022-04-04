/**
 *  @file bme688.c
 *
 *  @brief Where the BME Task lives
 *
 *  Created on: Mar 2, 2022
 *      Author: RJ Yarwood
 */

/*********************************************************************
 * INCLUDES
 */

#include "./sensor_driver/bme688_driver.h"

#include "gatt.h"
#include "gattservapp.h"

#include "./ble/bme688service.h"
#include "bme688.h"

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
 * \addtogroup bme BME
 * @brief 
 * @{
 */

/// How often to perform sensor reads (milliseconds)
#define SENSOR_DEFAULT_PERIOD   1000

/// Delay from sensor enable to reading measurement (allow for 250 ms conversion time)
#define BME_DELAY_PERIOD         275

/// Length of the data for this sensor
#define SENSOR_DATA_LEN         BME_DATA_LEN

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
uint8_t bmeRegistered = 0;

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
 * @fn      BME_createTask
 *
 * @brief   Task creation function for the BME sensor. This can basically be copied and pasted for new sensor
 *
 * @param   none
 *
 * @return  none
 */
void BME_createTask(void)
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
 * @fn      BME_processCharChangeEvt
 *
 * @brief   SensorTag BME event handling. This can basically be copied and pasted for new sensor
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 *
 */
void BME_processCharChangeEvt(uint8_t paramID)
{
  uint8_t newValue;

  switch (paramID)
  {
  case SENSOR_CONF:
    if (sensorConfig != ST_CFG_ERROR)
    {
      BME_getParameter(SENSOR_CONF, &newValue);

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
    BME_getParameter(SENSOR_PERI, &newValue);
    sensorPeriod = newValue * SENSOR_PERIOD_RESOLUTION;
    break;

  default:
    // Should not get here
    break;
  }
}

/*******************************************************************//**
 * @fn      BME_init
 *
 * @brief   Initialize the BME service
 *
 * @param   none
 *
 * @return  none
 */
void BME_init(void)
{
  // Add service
  BME_addService();

  // Register callbacks with profile
  BME_registerAppCBs(&sensorCallbacks);

  // Initialize the module state variables
  sensorPeriod = SENSOR_DEFAULT_PERIOD;

  // Initialize characteristics and sensor driver
  BME_reset();
  initCharacteristicValue(SENSOR_PERI,
                          SENSOR_DEFAULT_PERIOD / SENSOR_PERIOD_RESOLUTION,
                          sizeof(uint8_t));


  bmeRegistered = 1;
}

/*******************************************************************//**
 * @fn      BME_reset
 *
 * @brief   Reset characteristics
 *
 * @param   none
 *
 * @return  none
 */
void BME_reset(void)
{
  sensorConfig = ST_CFG_SENSOR_DISABLE;
  initCharacteristicValue(SENSOR_DATA, 0, BME_DATA_LEN);
  initCharacteristicValue(SENSOR_CONF, sensorConfig, sizeof(uint8_t));
}


/*********************************************************************
* Private functions
*/

/*******************************************************************//**
 * @fn      sensorTaskFxn
 *
 * @brief   The task loop of the BME readout task
 *
 * @param   a0 (not used)
 *
 * @param   a1 (not used)
 *
 * @return  none
 */
static void sensorTaskFxn(UArg a0, UArg a1)
{
  uint8_t data[BME_DATA_LEN]; // Local data buffer

  if(!bmeRegistered){ //If service is not registered give up the CPU.
      DELAY_MS(100);
  }

  // Register task with BLE stack
  ICall_registerApp(&sensorSelfEntity, &sensorSem);

  // Deactivate task (active only when measurement is enabled)
  Task_setPri(Task_handle(&sensorTask), -1);

  struct bme bme; // Local instance of BME

  while(init_bme(&bme)); // Don't proceed with task if bme is not init

  // Task loop
  while (true)
  {
    if (sensorConfig == ST_CFG_SENSOR_ENABLE)
    {

      ///Note: You could turn sensor on before reading then off immediatly after if power consumption is more important
      // Read data
      get_data(&bme);

      data[0] = bme.data.temperature;
      data[1] = bme.data.temperature >> 8;
      data[2] = bme.data.pressure;
      data[3] = bme.data.pressure >> 8;
      data[4] = bme.data.pressure >> 16;
      data[5] = bme.data.pressure >> 24;
      data[6] = bme.data.humidity;
      data[7] = bme.data.humidity >> 8;
      data[8] = bme.data.humidity >> 16;
      data[9] = bme.data.humidity >> 24;
      data[10] = bme.data.gas_resistance;
      data[11] = bme.data.gas_resistance >> 8;
      data[12] = bme.data.gas_resistance >> 16;
      data[13] = bme.data.gas_resistance >> 24;

      // Update GATT
      BME_setParameter(SENSOR_DATA, BME_DATA_LEN, data);

      // Next cycle
      DELAY_MS(sensorPeriod - BME_DELAY_PERIOD);
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
  SensorTag_charValueChangeCB(SERVICE_ID_BME, paramID);
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
  uint8_t data[BME_DATA_LEN];

  memset(data,value,paramLen);
  BME_setParameter(paramID, paramLen, data);
}


/** @} */
