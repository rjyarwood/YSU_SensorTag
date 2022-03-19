/*
 * adxl343.c
 *
 *  Created on: Feb 11, 2022
 *      Author: rjyar
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

// How often to perform adxl reads (milliseconds)
#define ADXL_DEFAULT_PERIOD   1000

// Delay from adxl enable to reading measurement
// (allow for 250 ms conversion time)
#define ADXL_MEAS_DELAY         275

// Event flag for this adxl
#define ADXL_EVT              ADXL_ADXL_EVT

// Task configuration
#define ADXL_TASK_PRIORITY    1
#define ADXL_TASK_STACK_SIZE  1024

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
static ICall_EntityID adxlSelfEntity;

// Semaphore used to post events to the application thread
static ICall_Semaphore adxlSem;

// Task setup
static Task_Struct adxlTask;
static Char adxlTaskStack[ADXL_TASK_STACK_SIZE];

// Parameters
static uint8_t adxlConfig;
static uint16_t adxlPeriod;

uint8_t adxlRegistered = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void adxlTaskFxn(UArg a0, UArg a1);
static void adxlConfigChangeCB(uint8_t paramID);
static void initCharacteristicValue(uint8_t paramID, uint8_t value,
                                    uint8_t paramLen);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static sensorCBs_t adxlCallbacks =
{
  adxlConfigChangeCB,  // Characteristic value change callback
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
void ADXL_createTask(void)
{
  Task_Params taskParames;

  // Create the task for the state machine
  Task_Params_init(&taskParames);
  taskParames.stack = adxlTaskStack;
  taskParames.stackSize = ADXL_TASK_STACK_SIZE;
  taskParames.priority = ADXL_TASK_PRIORITY;

  Task_construct(&adxlTask, adxlTaskFxn, &taskParames, NULL);
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
void ADXL_processCharChangeEvt(uint8_t paramID)
{
  uint8_t newValue;

  switch (paramID)
  {
  case SENSOR_CONF:
    if (adxlConfig != ST_CFG_ERROR)
    {
      ADXL_getParameter(SENSOR_CONF, &newValue);

      if (newValue == ST_CFG_SENSOR_DISABLE)
      {
        // Reset characteristics
        initCharacteristicValue(SENSOR_DATA, 0, ADXL_DATA_LEN);

        // Deactivate task
        Task_setPri(Task_handle(&adxlTask), -1);
      }
      else
      {
        Task_setPri(Task_handle(&adxlTask), ADXL_TASK_PRIORITY);
      }

      adxlConfig = newValue;
    }
    else
    {
      // Make sure the previous characteristics value is restored
      initCharacteristicValue(SENSOR_CONF, adxlConfig, sizeof(uint8_t));
    }

    break;

  case SENSOR_PERI:
    ADXL_getParameter(SENSOR_PERI, &newValue);
    adxlPeriod = newValue * SENSOR_PERIOD_RESOLUTION;
    if(adxlPeriod < ADXL_MEAS_DELAY){
        adxlPeriod = ADXL_MEAS_DELAY + 1;
    }
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
void ADXL_init(void)
{
  // Add service
  ADXL_addService();

  // Register callbacks with profile
  ADXL_registerAppCBs(&adxlCallbacks);

  // Initialize the module state variables
  adxlPeriod = ADXL_DEFAULT_PERIOD;

  // Initialize characteristics and adxl driver
  ADXL_reset();
  initCharacteristicValue(SENSOR_PERI,
                          ADXL_DEFAULT_PERIOD / SENSOR_PERIOD_RESOLUTION,
                          sizeof(uint8_t));

  adxlRegistered = 1;
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
void ADXL_reset(void)
{
  adxlConfig = ST_CFG_SENSOR_DISABLE;
  initCharacteristicValue(SENSOR_DATA, 0, ADXL_DATA_LEN);
  initCharacteristicValue(SENSOR_CONF, adxlConfig, sizeof(uint8_t));
}


/*********************************************************************
* Private functions
*/

/*********************************************************************
 * @fn      adxlTaskFxn
 *
 * @brief   The task loop of the temperature readout task
 *
 * @param   a0 (not used)
 *
 * @param   a1 (not used)
 *
 * @return  none
 */
static void adxlTaskFxn(UArg a0, UArg a1)
{
  uint8_t data[ADXL_DATA_LEN];

  if(!adxlRegistered){
     DELAY_MS(100);
  }
  // Register task with BLE stack
  ICall_registerApp(&adxlSelfEntity, &adxlSem);

  // Deactivate task (active only when measurement is enabled)
  Task_setPri(Task_handle(&adxlTask), -1);

  // Task loop
  while (true)
  {
      bStatus_t ret = SUCCESS;
     data[0] = 0;
    if (adxlConfig == ST_CFG_SENSOR_ENABLE)
    {

      // Read data
      ADXL_read(data);

      // Update GATT
      ret = ADXL_setParameter(SENSOR_DATA, ADXL_DATA_LEN, data);


      if(ret != SUCCESS){
          DELAY_MS(adxlPeriod - ADXL_MEAS_DELAY);
      }

      // Next cycle
      DELAY_MS(adxlPeriod - ADXL_MEAS_DELAY);
    }
    else
    {
      DELAY_MS(ADXL_DEFAULT_PERIOD);
    }
  }
}


/*********************************************************************
 * @fn      adxlChangeCB
 *
 * @brief   Callback from IR Temperature Service indicating a value change
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 */
static void adxlConfigChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_ADXL, paramID);
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
  uint8_t data[ADXL_DATA_LEN];

  memset(data,value,paramLen);
  ADXL_setParameter(paramID, paramLen, data);
}


