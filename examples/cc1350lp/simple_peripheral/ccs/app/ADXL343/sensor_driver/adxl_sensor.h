/*
 * adxl_sensor.h
 *
 *  Created on: Feb 16, 2022
 *      Author: rjyar
 */

#ifndef ADXL343_DRIVER_ADXL_SENSOR_H_
#define ADXL343_DRIVER_ADXL_SENSOR_H_

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>

/* Board Header files */
#include "Board.h"

#include "I2C/i2c_interface.h"


/* ========== FROM ADXL ============*/
/*** Defining Registers Addresses ***/

/* Chip ID */
#define ADXL_CHIP_ID        UINT8_C(0x00)

/* Tap threshold */
#define THRESH_TAP          UINT8_C(0x1D)

/* X-axis offset */
#define OFFSET_X            UINT8_C(0x1E)

/* Y-axis offset */
#define OFFSET_Y            UINT8_C(0x1F)

/* Z-axis offset */
#define OFFSET_Z            UINT8_C(0x20)

/* Tap duration */
#define TAP_DURATION        UINT8_C(0x21)

/* Tap latency */
#define TAP_LATENCY         UINT8_C(0x22)

/* Tap window */
#define TAP_WINDOW          UINT8_C(Ox23)

/* Activity threshold */
#define THRESH_ACT          UINT8_C(0x24)

/* Inactivity threshold */
#define THRESH_INACT        UINT8_C(0x25)

/* Inactivity time */
#define TIME_INACT          UINT8_C(0x26)

/* Axis enable control for activity and inactivity detection */
#define ACT_INACT_CTL       UINT8_C(0x27)

/* Free-fall threshold */
#define THRESH_FF           UINT8_C(0x28)

/* Free-fall time  */
#define TIME_FF             UINT8_C(0x29)

/* Axis control for single tap/double tap */
#define TAP_AXES            UINT8_C(0x2A)

/* Source of single tap/double tap */
#define ACT_TAP_STATUS      UINT8_C(0x2B)

/* Data rate and power mode control */
#define BW_RATE             UINT8_C(0x2C)

/* Power-saving features control */
#define POWER_CTL           UINT8_C(0x2D)

/* Interrupt enable control */
#define INT_ENABLE          UINT8_C(0x2E)

/* Interrupt mapping control */
#define INT_MAP             UINT8_C(0x2F)

/* Sources Interrupt  */
#define INT_SOURCE          UINT8_C(0x30)

/* Data Format control */
#define DATA_FORMAT         UINT8_C(0x31)

/* X-axis Data 0 */
#define DATA_X0             UINT8_C(0x32)

/* X-axis Data 1 */
#define DATA_X1             UINT8_C(0x33)

/* Y-axis Data 0 */
#define DATA_Y0             UINT8_C(0x34)

/* Y-axis Data 1 */
#define DATA_Y1             UINT8_C(0x35)

/* Z-axis Data 0 */
#define DATA_Z0             UINT8_C(0x36)

/* Z-axis Data 1 */
#define DATA_Z1             UINT8_C(0x37)

/* FIFO Control */
#define FIFO_CTL            UINT8_C(0x38)

/* FIFO Status */
#define FIFO_STATUS         UINT8_C(0x39)

/* Defining Power Control Status */

/* Auto sleep mode */
#define ADXL_AUTO_SLEEP     UINT8_C(0x10)

/* Measure mode */
#define ADXL_MEASURE        UINT8_C(0x08)

/* Sleep mode */
#define ADXL_SLEEP          UINT8_C(0X04)

/* Wake up bits, these bits control the frequency of readings in sleep mode*/
#define ADXL_WAKE_UP_8HZ    UINT8_C(0x00)

#define ADXL_WAKE_UP_4HZ    UINT8_C(0x01)

#define ADXL_WAKE_UP_2HZ    UINT8_C(0x02)

#define ADXL_WAKE_UP_1HZ    UINT8_C(0x03)


/* Defining data format registers */

/* Self test bit */
#define SELF_TEST_BIT       UINT8_C(0x80)

/* Int Invert Bit */
#define INT_INVERT_BIT      UINT8_C(0x20)

/* Full resolution bit */
#define FULL_RES_BIT        UINT8_C(0x08)

/* Justify Bit */
#define JUST_BIT            UINT8_C(0X04)

/** Range Bits **/

/* +- 2g        */
#define RANGE_2G            UINT8_C(0x00)

/* +- 4g        */
#define RANGE_4G            UINT8_C(0x01)

/* +- 8g        */
#define RANGE_8G            UINT8_C(0x02)

/* +- 16g        */
#define RANGE_16G           UINT8_C(0x8B)


/* Macro to combine two 8 bit data's to form a 16 bit data */
#define ADXL_CONCAT_BYTES(msb, lsb)             (((uint16_t)msb << 8) | (uint16_t)lsb)

/*============= END FROM ADXL =============== */

#define ADXL_SLAVE_ADDR     0x53        ///I2C Address of the ADXL device

/** Change this to adjust how many samples to take from ADXL before releasing **/
#define ADXL_TRANSACTIONS   3

void ADXL_read(uint8_t* d);


#endif /* ADXL343_DRIVER_ADXL_SENSOR_H_ */
