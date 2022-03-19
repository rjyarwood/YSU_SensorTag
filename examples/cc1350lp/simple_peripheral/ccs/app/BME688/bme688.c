/*
 * bme688.c
 *
 *  Created on: Mar 2, 2022
 *      Author: rjyar
 */

/*********************************************************************
 * INCLUDES
 */
#include "gatt.h"
#include "gattservapp.h"

#include "./ble/bme688service.h"
#include "bme688.h"
#include "./sensor_driver/bme688_driver.h"

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

// How often to perform sensor reads (milliseconds)
#define SENSOR_DEFAULT_PERIOD   1000

// Delay from sensor enable to reading measurement
// (allow for 250 ms conversion time)
#define TEMP_MEAS_DELAY         275

// Length of the data for this sensor
#define SENSOR_DATA_LEN         BME_DATA_LEN

// Event flag for this sensor
#define SENSOR_EVT              BME_SENSOR_EVT

// Task configuration
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
// Entity ID used to check for source and/or destination of messages
static ICall_EntityID sensorSelfEntity;

// Semaphore used to post events to the application thread
static ICall_Semaphore sensorSem;

// Task setup
static Task_Struct sensorTask;
static Char sensorTaskStack[SENSOR_TASK_STACK_SIZE];

// Parameters
static uint8_t sensorConfig;
static uint16_t sensorPeriod;


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
  sensorConfigChangeCB,  // Characteristic value change callback
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SensorTagTmp_createTask
 *
 * @brief   Task creation function for the SensorTag
 *
 * @param   none
 *
 * @return  none
 */
void BME_createTask(void)
{
  Task_Params taskParames;

  // Create the task for the state machine
  Task_Params_init(&taskParames);
  taskParames.stack = sensorTaskStack;
  taskParames.stackSize = SENSOR_TASK_STACK_SIZE;
  taskParames.priority = SENSOR_TASK_PRIORITY;

  Task_construct(&sensorTask, sensorTaskFxn, &taskParames, NULL);
}

/*********************************************************************
 * @fn      SensorTagTmp_processCharChangeEvt
 *
 * @brief   SensorTag IR temperature event handling
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
    BME_getParameter(SENSOR_PERI, &newValue);
    sensorPeriod = newValue * SENSOR_PERIOD_RESOLUTION;
    break;

  default:
    // Should not get here
    break;
  }
}

/*********************************************************************
 * @fn      SensorTagTmp_init
 *
 * @brief   Initialize the module
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

/*********************************************************************
 * @fn      SensorTagTmp_reset
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
  initCharacteristicValue(SENSOR_DATA, 0, SENSOR_DATA_LEN);
  initCharacteristicValue(SENSOR_CONF, sensorConfig, sizeof(uint8_t));
}


/*********************************************************************
* Private functions
*/

/*********************************************************************
 * @fn      sensorTaskFxn
 *
 * @brief   The task loop of the temperature readout task
 *
 * @param   a0 (not used)
 *
 * @param   a1 (not used)
 *
 * @return  none
 */
static void sensorTaskFxn(UArg a0, UArg a1)
{
  typedef union
  {
    struct
    {
      uint16_t tempTarget, tempLocal;
    } v;
    uint16_t a[2];
  } Data_t;

  if(!bmeRegistered){
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
      Data_t data;

//      // Read data
//      SensorTmp007_enable(true);
//      DELAY_MS(TEMP_MEAS_DELAY);
//      SensorTmp007_read(&data.v.tempLocal, &data.v.tempTarget);
//      SensorTmp007_enable(false);

      // Update GATT
      BME_setParameter(SENSOR_DATA, SENSOR_DATA_LEN, data.a);

      // Next cycle
      DELAY_MS(sensorPeriod - TEMP_MEAS_DELAY);
    }
    else
    {
      DELAY_MS(SENSOR_DEFAULT_PERIOD);
    }
  }
}


/*********************************************************************
 * @fn      sensorChangeCB
 *
 * @brief   Callback from IR Temperature Service indicating a value change
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


/*********************************************************************
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
  uint8_t data[SENSOR_DATA_LEN];

  memset(data,value,paramLen);
  BME_setParameter(paramID, paramLen, data);
}



