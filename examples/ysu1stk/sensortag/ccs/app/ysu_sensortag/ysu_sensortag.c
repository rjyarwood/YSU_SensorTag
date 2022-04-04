/**
 *  @file: ysu_sensortag.c
 *
 *  @brief: This is the main thread file. This does everything from setting up BLE to running the main task
 *
 *  Created on: Feb 11, 2022
 *      Author: RJ Yarwood
 */

/***************************************/
#include <string.h>
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>

#include <ICall.h>

#include "gatt.h"
#include "hci.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "osal_snv.h"
#include "util.h"
#include "icall_apimsg.h"
#include "devinfoservice.h"
/***************************************/


#include "sensortag_conn_ctrl.h"
#include "st_util.h"
#include "board.h"
#include "ysu_sensortag.h"

/**
 *  \addtogroup sensortag YSU SensorTag 
 * @{
 */

/************************* BEGIN USER CODE 4 ************************/
/** \todo USER CODE 4: Add your sensor task file here. The ADXL service is only here for advertising. If you would like to
*                    advertise another service see below           
*/
#include "ADXL343/ble/adxl_service.h"
#include "ADXL343/adxl343.h"
#include "BME688/bme688.h"
/************************** END USER CODE 4 *************************/



/*************************** BEGIN BLE CONFIG FLAGS ******************/
/**
 * \addtogroup sensortag_ble SensorTag BLE 
 * @{
 */

/// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160  

/// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED

/// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

/// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

/// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

/// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          100

/// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_WAIT_BOTH_PARAMS

/// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         1
/** @} */

/**
 *  \addtogroup sensortag_ctrl SensorTag Control
 * @{
 */

/// How often to perform periodic event (in milliseconds)
#define ST_PERIODIC_EVT_PERIOD               1000 

/// ID of SensorTag device
#define ST_DEVICE_ID                          0x03
#define ST_KEY_DATA_ID                        0x00

/// Length of board address
#define B_ADDR_STR_LEN                        15

/// Task configuration
#define ST_TASK_PRIORITY                      1

#ifndef ST_TASK_STACK_SIZE
/// Stack size may be overridden by project settings
#define ST_TASK_STACK_SIZE                    1024
#endif

/// Mask for an RTOS state event
#define ST_STATE_CHANGE_EVT                   0x0001
/// Mask for a BLE Characteristic event
#define ST_CHAR_CHANGE_EVT                    0x0002
/// Mask for a Periodic event
#define ST_PERIODIC_EVT                       0x0004
/// Mask for an OAD write event
#define SBP_OAD_WRITE_EVT                     0x0008

/// Misc.
#define INVALID_CONNHANDLE                    0xFFFF
#define TEST_INDICATION_BLINKS                5  // Number of blinks
#define OAD_PACKET_SIZE                       18
#define KEY_STATE_OFFSET                      13 // Offset in advertising data

/****************************** END BLE CONFIG FLAGS ********************************/

/// Maybe change this but it should be universal for all things developed with this code so don't just change for your project
#define YSU_COMPANY_ID                        0x0FFA

/******************************************************************************/
/*************************** BEGIN DO NOT EDIT ********************************/
/******************************************************************************/

/*******************************************************************************
 * TYPEDEFS
 */

/** 
 * @brief App event passed from profiles.
 */
typedef struct
{
  uint8_t event;  /**< The event */
  uint8_t serviceID; /**< Which service */
  uint8_t paramID; /**< Which characteristic flag */
} stEvt_t;

/** @} */
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/**
 * \addtogroup sensortag_ble SensorTag BLE 
 * @{
 */
/// Profile state and parameters
gaprole_States_t gapProfileState = GAPROLE_INIT;

/// Semaphore globally used to post events to the application thread
ICall_Semaphore sem;

/// Entity ID globally used to check for source and/or destination of messages
ICall_EntityID selfEntityMain;
/** @} */ 


/**
 *  \addtogroup sensortag_ctrl SensorTag Control
 * @{
 */
/// Global pin resources
PIN_State pinGpioState;
PIN_Handle hGpioPin;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/// Task configuration
static Task_Struct sensorTagTask;
static Char sensorTagTaskStack[ST_TASK_STACK_SIZE];

/// Clock instances for internal periodic events.
static Clock_Struct periodicClock;

/// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

/// events flag for internal application events.
static uint16_t events;
/** @} */
/******************************************************************************/
/*************************** END DO NOT EDIT **********************************/
/******************************************************************************/



// Uncomment this if you are going to add a selfcheck feature
// self-test result
//static uint8_t selfTestMap;

/**
 * \addtogroup sensortag_ble SensorTag BLE 
 * @{
 */
/// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0x16,                             /// Length of the scan response name, do not edit this, just add a ifdef around
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
#if defined(YSU1)
  'Y', 'S', 'U', '1', '.', '0',' ', /// Do not change this major version, add a new flag
#elif defined(CC1350_LAUNCHXL)
  'C', 'C', '1', '3', '5', '0', ' ',
#else
  'C', 'C', '2', '6', '5', '0', ' ',
#endif
  'S', 'e', 'n',  's',  'o',  'r',  'T',  'a',  'g',

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

/// GAP - Advertisement data (max size = 31 bytes, though this is
/// best kept short to conserve power while advertising)
static uint8_t advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE, // some of the UUID's, but not all
#ifdef ADXL_SERV_UUID                   ///This is where you would advertise your service. You do not need to for the service to work
  LO_UINT16(ADXL_SERV_UUID),
  HI_UINT16(ADXL_SERV_UUID),
#else
  LO_UINT16(0xFF),
  HI_UINT16(0xFF),
#endif

  // Manufacturer specific advertising data
  0x06,
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,
  LO_UINT16(YSU_COMPANY_ID),
  HI_UINT16(YSU_COMPANY_ID),
  ST_DEVICE_ID,
  ST_KEY_DATA_ID,
  0x00                                    // Key state
};

/********************* BEGIN USER CODE 5 ******************/
/// \todo USER CODE 5: Add your device info. Do not overwrite, add an ifdef for your device
/// Device information parameters
#ifdef YSU
static const uint8_t devInfoModelNumber[] = "YSU SensorTag";
#elif  defined(CC1350_LAUNCHXL)
static const uint8_t devInfoModelNumber[] = "CC1350 LaunchPad";
#else
static const uint8_t devInfoModelNumber[] = "CC2650 LaunchPad";
#endif
static const uint8_t devInfoNA[] =          "N.A.";
static const uint8_t devInfoFirmwareRev[] = "1.0";
static const uint8_t devInfoMfrName[] =     "Youngstown State University";
static const uint8_t *devInfoHardwareRev =  devInfoNA;
/********************** END USER CODE 5 *******************/




/// GAP GATT Attributes
static const uint8_t *attDeviceName = devInfoModelNumber;
/** @} */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void SensorTag_init(void);
static void SensorTag_taskFxn(UArg a0, UArg a1);
static void SensorTag_processStackMsg(ICall_Hdr *pMsg);
static void SensorTag_processGATTMsg(gattMsgEvent_t *pMsg);
static void SensorTag_processAppMsg(stEvt_t *pMsg);
static void SensorTag_processStateChangeEvt(gaprole_States_t newState) ;
static void SensorTag_processCharValueChangeEvt(uint8_t serviceID, uint8_t paramID) ;
static void SensorTag_stateChangeCB(gaprole_States_t newState);
static void SensorTag_resetAllModules(void);
static void SensorTag_clockHandler(UArg arg);
static void SensorTag_enqueueMsg(uint8_t event, uint8_t serviceID, uint8_t paramID);
static void SensorTag_callback(PIN_Handle handle, PIN_Id pinId);
static void SensorTag_setDeviceInfo(void);
/***********************************************************************************/

/**
 * \addtogroup sensortag_ble SensorTag BLE 
 * @{
 */
/// GAP Role Callbacks
static gapRolesCBs_t sensorTag_gapRoleCBs =
{
  SensorTag_stateChangeCB     // Profile State Change Callbacks
};

/// GAP Bond Manager Callbacks
/// Where you would add security callbacks
static gapBondCBs_t sensorTag_bondMgrCBs =
{
  NULL, // Passcode callback (not used by application)
  NULL  // Pairing / Bonding state Callback (not used by application)
};
/** @} */

/********************************************************************************/
/************************** BEGIN CONTROL FUNCTIONS *****************************/
/*****************************************************************************//**
 * \addtogroup sensortag_ctrl SensorTag Control
 *
 * @brief Functions to control the flow of the software and hardware of the SensorTag
 * 
 * @{
 */

/**
 * @brief Creates the main task for the YSU SensorTag
 * 
 * @param none
 * @return none
 * 
 * This function sets up the SensorTag Task and registers it with the TIRTOS Kernel
 */
void SensorTag_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sensorTagTaskStack;
  taskParams.stackSize = ST_TASK_STACK_SIZE;
  taskParams.priority = ST_TASK_PRIORITY;

  Task_construct(&sensorTagTask, SensorTag_taskFxn, &taskParams, NULL);
}

/**
 * @brief Initializes the SensorTag and its BLE configurations.
 *  
 * @param none
 * @return none
 * 
 * This function initializes BLE and any board drivers. This includes
 * registering the entire app with the ICALL dispatcher, creating a message
 * queue for RTOS messages. It also initializes a clock for periodic events and initializes the 
 * GAP and GATT. This is where you call any functions that register services.
 * 
 */
static void SensorTag_init(void)
{

  /****************** BEGIN USER CODE 6 *********************/
  /// \todo USER CODE 6: Initialize any board level drivers needed
  Board_initI2C();
  /******************* END USER CODE 6 **********************/


  /**************************** BEGIN BOILERPLATE ****************************/

  // **************************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApP
  // **************************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntityMain, &sem);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&periodicClock, SensorTag_clockHandler,
                      ST_PERIODIC_EVT_PERIOD, 0, false, ST_PERIODIC_EVT);

  // Setup the GAP
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the GAP Peripheral Role Profile
  {
    // For all hardware platforms, device starts advertising upon initialization
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                         &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                         &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                         &desiredConnTimeout);
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void*)attDeviceName);

  // Set advertising interval
  {
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager
  {
    uint32_t passkey = 0; // passkey "000000"
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8_t mitm = FALSE;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t bonding = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
                            &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes


  // Add device info service.
  DevInfo_AddService();

  /**************************** END BOILERPLATE ****************************/



  // Add application specific device information
  SensorTag_setDeviceInfo();

  /************************ BEGIN USER CODE 7 *******************/
  /// \todo USER CODE 7: Call the function to set up your Bluetooth service
  ADXL_init();
  BME_init();
  /************************ END USER CODE 7 *********************/


  // Start the Device
  GAPRole_StartDevice(&sensorTag_gapRoleCBs);

  // Start Bond Manager
  VOID GAPBondMgr_Register(&sensorTag_bondMgrCBs);
}


/**
 * @brief   Main application thread
 * 
 * This manages the flow of ICALL and RTOS messages. It also manages the periodic clock and can be used to poll sensors that
 * do not have their own RTOS task
 */
static void SensorTag_taskFxn(UArg a0, UArg a1)
{

  /***************************************************************************/
  /************************** BEGIN DO NOT EDIT ******************************/
  /***************************************************************************/

  // Initialize application
  SensorTag_init();

  // Application main loop
  for (;;)
  {
    // Waits for a signal to the semaphore associated with the calling thread.
    // Note that the semaphore associated with a thread is signalled when a
    // message is queued to the message receive queue of the thread or when
    // ICall_signal() function is called onto the semaphore.
    ICall_Errno errno = ICall_wait(ICALL_TIMEOUT_FOREVER);

    if (errno == ICALL_ERRNO_SUCCESS)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntityMain))
        {
          // Process inter-task message
          SensorTag_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      while (!Queue_empty(appMsgQueue))
      {
        stEvt_t *pMsg = (stEvt_t *)Util_dequeueMsg(appMsgQueue);
        if (pMsg)
        {
          // Process message.
          SensorTag_processAppMsg(pMsg);

          // Free the space from the message.
          ICall_free(pMsg);
        }
      }
    }

    if (!!(events & ST_PERIODIC_EVT))
    {
      events &= ~ST_PERIODIC_EVT;

      if (gapProfileState == GAPROLE_CONNECTED ||
          gapProfileState == GAPROLE_ADVERTISING)
      {
        Util_startClock(&periodicClock);
      }
    }
    /***************************************************************************/
    /**************************** END DO NOT EDIT ******************************/
    /***************************************************************************/



    /*************************** BEGIN USER CODE 8 *****************************/
    /// \todo USER CODE 8: If you are not using a task for your sensor, poll it here
    /**************************** END USER CODE 8 ******************************/


  } // End of Task Loop
}



/**
 * @brief   Handler function for clock time-outs.
 *
 * @param   arg - event type
 *
 * @return  none
 */
static void SensorTag_clockHandler(UArg arg)
{
  // Store the event.
  events |= arg;

  // Wake up the application.
  Semaphore_post(sem);
}


/**
 * @brief   Reset all modules, typically when a connection is terminated.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_resetAllModules(void)
{

    /// If you are going to implement a reset this is where you would do it

}

/** 
 * @brief If you are going to add hwi callbacks, do it here
 * 
 * @param handle 
 * @param pinId
 * 
 * @return none
 */
static void SensorTag_callback(PIN_Handle handle, PIN_Id pinId)
{
  switch (pinId)
  {

  default:
    /* Do nothing */
    break;
  }
}


/**
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SensorTag_processAppMsg(stEvt_t *pMsg)
{
  switch (pMsg->event)
  {
    case ST_STATE_CHANGE_EVT:
      SensorTag_processStateChangeEvt((gaprole_States_t)pMsg->serviceID);
      break;

    case ST_CHAR_CHANGE_EVT:
      SensorTag_processCharValueChangeEvt(pMsg->serviceID, pMsg->paramID);
      break;

    default:
      // Do nothing.
      break;
  }
}

/** @} **************************************************************************/
/************************* END OF CONTROL FUNCTIONS *****************************/
/********************************************************************************/


/********************************************************************************/
/************************** BEGIN BLE FUNCTIONS *********************************/
/*****************************************************************************//**
 * \addtogroup sensortag_ble SensorTag BLE
 * @brief Functions that interface with the BLE stack
 * @{
 */
/********************************************************************************/


/**
 * @brief Set application specific Device Information
 * This sets Model Number, Serial Number, Software Revision, Firmware Revision, Hardware Revision, Manufacturing Name.
 * These are set at the top of this file. Define guards should be used
 * 
 * @param   none
 *
 * @return  none
 */
static void SensorTag_setDeviceInfo(void)
{
  DevInfo_SetParameter(DEVINFO_MODEL_NUMBER, sizeof(devInfoModelNumber),
                       (void*)devInfoModelNumber);
  DevInfo_SetParameter(DEVINFO_SERIAL_NUMBER, sizeof(devInfoNA),
                       (void*)devInfoNA);
  DevInfo_SetParameter(DEVINFO_SOFTWARE_REV, sizeof(devInfoNA),
                       (void*)devInfoNA);
  DevInfo_SetParameter(DEVINFO_FIRMWARE_REV, sizeof(devInfoFirmwareRev),
                       (void*)devInfoFirmwareRev);
  DevInfo_SetParameter(DEVINFO_HARDWARE_REV, sizeof(devInfoHardwareRev),
                       (void*)devInfoHardwareRev);
  DevInfo_SetParameter(DEVINFO_MANUFACTURER_NAME, sizeof(devInfoMfrName),
                       (void*)devInfoMfrName);
}


/**
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SensorTag_stateChangeCB(gaprole_States_t newState)
{
  SensorTag_enqueueMsg(ST_STATE_CHANGE_EVT, newState, NULL);
}



/**
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SensorTag_processStateChangeEvt(gaprole_States_t newState)
{

  switch (newState)
  {
  case GAPROLE_STARTED:
    {
      uint8_t ownAddress[B_ADDR_LEN];
      uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

      GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

      // use 6 bytes of device address for 8 bytes of system ID value
      systemId[0] = ownAddress[0];
      systemId[1] = ownAddress[1];
      systemId[2] = ownAddress[2];

      // set middle bytes to zero
      systemId[4] = 0x00;
      systemId[3] = 0x00;

      // shift three bytes up
      systemId[7] = ownAddress[5];
      systemId[6] = ownAddress[4];
      systemId[5] = ownAddress[3];

      DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
    }
    break;

  case GAPROLE_ADVERTISING:
    // Start the clock
    if (!Util_isActive(&periodicClock))
    {
      Util_startClock(&periodicClock);
    }

    // Make sure key presses are not stuck
    SensorTag_updateAdvertisingData(0);
    break;

  case GAPROLE_CONNECTED:
    {
      // Start the clock
      if (!Util_isActive(&periodicClock))
      {
        Util_startClock(&periodicClock);
      }


      // Update connection parameters
      SensorTagConnectionControl_update();
    }
    break;

  case GAPROLE_CONNECTED_ADV:
    break;

  case GAPROLE_WAITING:
  case GAPROLE_WAITING_AFTER_TIMEOUT:
    SensorTag_resetAllModules();
    break;

  case GAPROLE_ERROR:
    SensorTag_resetAllModules();
    break;

  default:
    break;
  }

  gapProfileState = newState;

#ifdef INCLUDE_DISPLAY
  SensorTagDisplay_showStatus();
#endif // INCLUDE_DISPLAY
}



/**
 * @brief   Callback from Sensor Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
void SensorTag_charValueChangeCB(uint8_t serviceID, uint8_t paramID)
{
  SensorTag_enqueueMsg(ST_CHAR_CHANGE_EVT, serviceID, paramID);
}



/**
 * @brief   Process pending Profile characteristic value change
 *          events. The events are generated by the network task (BLE)
 *
 * @param   serviceID - ID of the affected service
 * @param   paramID - ID of the affected parameter
 *
 * @return  none
 */
static void SensorTag_processCharValueChangeEvt(uint8_t serviceID,
                                                uint8_t paramID)
{
  switch (serviceID)
  {

  /***************** BEGIN USER CODE 9 **********************/
  /// \todo USER CODE 9: This is where you add a case statement for that ID you defined earlier.
  ///This ensures all BLE messages get where they need to go
  case SERVICE_ID_ADXL:
    ADXL_processCharChangeEvt(paramID);
    break;

  case SERVICE_ID_BME:
    BME_processCharChangeEvt(paramID);
    break;
  /****************** END USER CODE 9 ***********************/


  default:
    break;
  }
}



/**
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SensorTag_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      // Process GATT message
      SensorTag_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    default:
      // do nothing
      break;
  }
}



/**
 * @brief   Process GATT messages
 *
 * @return  none
 */
static void SensorTag_processGATTMsg(gattMsgEvent_t *pMsg)
{
  GATT_bm_free(&pMsg->msg, pMsg->method);
}



/**
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   serviceID - service identifier
 * @param   paramID - parameter identifier
 *
 * @return  none
 */
static void SensorTag_enqueueMsg(uint8_t event, uint8_t serviceID, uint8_t paramID)
{
  stEvt_t *pMsg;

  // Create dynamic pointer to message.
  if ((pMsg = ICall_malloc(sizeof(stEvt_t))))
  {
    pMsg->event = event;
    pMsg->serviceID = serviceID;
    pMsg->paramID = paramID;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueue, sem, (uint8_t*)pMsg);
  }
}



/**
 * @brief   Update the advertising data with the latest key press status
 *
 * @return  none
 */
void SensorTag_updateAdvertisingData(uint8_t keyStatus)
{
  // Record key state in advertising data
  advertData[KEY_STATE_OFFSET] = keyStatus;
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
}

/** @} **************************************************************************/
/************************* END OF BLE FUNCTIONS *********************************/
/********************************************************************************/





/// @}

/*******************************************************************************
*******************************************************************************/
