/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       CC1350_LAUNCHXL.h
 *
 *  @brief      CC1350 LaunchPad Board Specific header file.
 *
 *  NB! This is the board file for CC1350 LaunchPad PCB version 1.0
 *
 *  ============================================================================
 */
#ifndef __CC1350_4XS_BOARD_H__
#define __CC1350_4XS_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 *  Includes
 *  ==========================================================================*/
#include <ti/drivers/PIN.h>
#include <driverlib/ioc.h>

/** ============================================================================
 *  Externs
 *  ==========================================================================*/
extern const PIN_Config BoardGpioInitTable[];

/** ============================================================================
 *  Defines
 *  ==========================================================================*/
#define CC1350LP_7XD

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>        <pin mapping>   <comments>
 */

/* Discrete outputs */
#define Board_RLED                  IOID_9
#define Board_LED_ON                1
#define Board_LED_OFF               0

/* Discrete inputs */
#define Board_BTN1                  PIN_UNASSIGNED
#define Board_BTN2                  PIN_UNASSIGNED

/* UART Board */
#define Board_UART_RX               PIN_UNASSIGNED        /* RXD  */
#define Board_UART_TX               PIN_UNASSIGNED         /* TXD  */
#define Board_UART_CTS              PIN_UNASSIGNED         /* CTS  */
#define Board_UART_RTS              PIN_UNASSIGNED         /* RTS */

/* SPI Board */
#define Board_SPI0_MISO             PIN_UNASSIGNED         /* RF1.20 */
#define Board_SPI0_MOSI             PIN_UNASSIGNED          /* RF1.18 */
#define Board_SPI0_CLK              PIN_UNASSIGNED        /* RF1.16 */
#define Board_SPI0_CSN              PIN_UNASSIGNED
#define Board_SPI1_MISO             PIN_UNASSIGNED
#define Board_SPI1_MOSI             PIN_UNASSIGNED
#define Board_SPI1_CLK              PIN_UNASSIGNED
#define Board_SPI1_CSN              PIN_UNASSIGNED

/* SPI */
#define Board_SPI_FLASH_CS          PIN_UNASSIGNED
#define Board_FLASH_CS_ON           0
#define Board_FLASH_CS_OFF          1

/* Booster pack generic */
#define Board_DIO0                  PIN_UNASSIGNED
#define Board_DIO1_RFSW             IOID_1
#define Board_DIO12                 PIN_UNASSIGNED
#define Board_DIO15                 PIN_UNASSIGNED
#define Board_DIO16_TDO             PIN_UNASSIGNED
#define Board_DIO17_TDI             PIN_UNASSIGNED
#define Board_DIO21                 PIN_UNASSIGNED
#define Board_DIO22                 PIN_UNASSIGNED
#define Board_DIO30_SWPWR           IOID_30

#define Board_DIO23_ANALOG          PIN_UNASSIGNED
#define Board_DIO24_ANALOG          PIN_UNASSIGNED
#define Board_DIO25_ANALOG          PIN_UNASSIGNED
#define Board_DIO26_ANALOG          PIN_UNASSIGNED
#define Board_DIO27_ANALOG          PIN_UNASSIGNED
#define Board_DIO28_ANALOG          PIN_UNASSIGNED
#define Board_DIO29_ANALOG          PIN_UNASSIGNED
#define Board_DIO30_ANALOG          PIN_UNASSIGNED

/* I2C */
#define Board_I2C1_SCL0             IOID_2
#define Board_I2C1_SDA0             IOID_1

#define Board_I2C0_SCL0             IOID_8
#define Board_I2C0_SDA0             IOID_7


/* Booster pack LCD (430BOOST - Sharp96 Rev 1.1) */
#define Board_LCD_CS                PIN_UNASSIGNED // SPI chip select
#define Board_LCD_EXTCOMIN          PIN_UNASSIGNED // External COM inversion
#define Board_LCD_ENABLE            PIN_UNASSIGNED // LCD enable
#define Board_LCD_POWER             PIN_UNASSIGNED // LCD power control
#define Board_LCD_CS_ON             1
#define Board_LCD_CS_OFF            0


/* PWM outputs */
#define Board_PWMPIN0                       Board_RLED
#define Board_PWMPIN1                       PIN_UNASSIGNED
#define Board_PWMPIN2                       PIN_UNASSIGNED
#define Board_PWMPIN3                       PIN_UNASSIGNED
#define Board_PWMPIN4                       PIN_UNASSIGNED
#define Board_PWMPIN5                       PIN_UNASSIGNED
#define Board_PWMPIN6                       PIN_UNASSIGNED
#define Board_PWMPIN7                       PIN_UNASSIGNED

/** ============================================================================
 *  Instance identifiers
 *  ==========================================================================*/
/* Generic SPI instance identifiers */
#define Board_SPI0                  CC1350_LAUNCHXL_SPI0
#define Board_SPI1                  CC1350_LAUNCHXL_SPI1
/* Generic UART instance identifiers */
#define Board_UART                  CC1350_LAUNCHXL_UART0
/* Generic I2C instance identifiers */
#define Board_I2C0                   CC1350_LAUNCHXL_I2C0
#define Board_I2C1                   CC1350_LAUNCHXL_I2C1
/* Generic Crypto instance identifiers */
#define Board_CRYPTO                CC1350_LAUNCHXL_CRYPTO0

#define Board_TRNG                  CC1350_LAUNCHXL_TRNG0
/* Generic GPTimer instance identifiers */
#define Board_GPTIMER0A             CC1350_LAUNCHXL_GPTIMER0A
#define Board_GPTIMER0B             CC1350_LAUNCHXL_GPTIMER0B
#define Board_GPTIMER1A             CC1350_LAUNCHXL_GPTIMER1A
#define Board_GPTIMER1B             CC1350_LAUNCHXL_GPTIMER1B
#define Board_GPTIMER2A             CC1350_LAUNCHXL_GPTIMER2A
#define Board_GPTIMER2B             CC1350_LAUNCHXL_GPTIMER2B
#define Board_GPTIMER3A             CC1350_LAUNCHXL_GPTIMER3A
#define Board_GPTIMER3B             CC1350_LAUNCHXL_GPTIMER3B
/* Generic PWM instance identifiers */
#define Board_PWM0                  CC1350_LAUNCHXL_PWM0
#define Board_PWM1                  CC1350_LAUNCHXL_PWM1
#define Board_PWM2                  CC1350_LAUNCHXL_PWM2
#define Board_PWM3                  CC1350_LAUNCHXL_PWM3
#define Board_PWM4                  CC1350_LAUNCHXL_PWM4
#define Board_PWM5                  CC1350_LAUNCHXL_PWM5
#define Board_PWM6                  CC1350_LAUNCHXL_PWM6
#define Board_PWM7                  CC1350_LAUNCHXL_PWM7

/** ============================================================================
 *  Number of peripherals and their names
 *  ==========================================================================*/

/*!
 *  @def    CC1350_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names on the CC2650 dev board
 */
typedef enum CC1350_LAUNCHXL_I2CName {
    CC1350_LAUNCHXL_I2C0 = 0,
    CC1350_LAUNCHXL_I2C1,

    CC1350_LAUNCHXL_I2CCOUNT
} CC1350_LAUNCHXL_I2CName;

/*!
 *  @def    CC1350_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names on the CC2650 dev board
 */
typedef enum CC1350_LAUNCHXL_CryptoName {
    CC1350_LAUNCHXL_CRYPTO0 = 0,

    CC1350_LAUNCHXL_CRYPTOCOUNT
} CC1350_LAUNCHXL_CryptoName;


/*!
 *  @def    CC1350_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names on the CC2650 dev board
 */
typedef enum CC1350_LAUNCHXL_SPIName {
    CC1350_LAUNCHXL_SPI0 = 0,
    CC1350_LAUNCHXL_SPI1,

    CC1350_LAUNCHXL_SPICOUNT
} CC1350_LAUNCHXL_SPIName;

/*!
 *  @def    CC1350_LAUNCHXL_TRNGName
 *  @brief  Enum of TRNG names on the board
 */
typedef enum CC1350_LAUNCHXL_TRNGName {
    CC1350_LAUNCHXL_TRNG0 = 0,
    CC1350_LAUNCHXL_TRNGCOUNT
} CC1350_LAUNCHXL_TRNGName;

/*!
 *  @def    CC1350_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs on the CC2650 dev board
 */
typedef enum CC1350_LAUNCHXL_UARTName {
    CC1350_LAUNCHXL_UART0 = 0,

    CC1350_LAUNCHXL_UARTCOUNT
} CC1350_LAUNCHXL_UARTName;

/*!
 *  @def    CC1350_LAUNCHXL_UdmaName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC1350_LAUNCHXL_UdmaName {
    CC1350_LAUNCHXL_UDMA0 = 0,

    CC1350_LAUNCHXL_UDMACOUNT
} CC1350_LAUNCHXL_UdmaName;

/*!
 *  @def    CC1350_LAUNCHXL_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum CC1350_LAUNCHXL_GPTimerName
{
    CC1350_LAUNCHXL_GPTIMER0A = 0,
    CC1350_LAUNCHXL_GPTIMER0B,
    CC1350_LAUNCHXL_GPTIMER1A,
    CC1350_LAUNCHXL_GPTIMER1B,
    CC1350_LAUNCHXL_GPTIMER2A,
    CC1350_LAUNCHXL_GPTIMER2B,
    CC1350_LAUNCHXL_GPTIMER3A,
    CC1350_LAUNCHXL_GPTIMER3B,
    CC1350_LAUNCHXL_GPTIMERPARTSCOUNT
} CC1350_LAUNCHXL_GPTimerName;

/*!
 *  @def    CC1350_LAUNCHXL_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum CC1350_LAUNCHXL_GPTimers
{
    CC1350_LAUNCHXL_GPTIMER0 = 0,
    CC1350_LAUNCHXL_GPTIMER1,
    CC1350_LAUNCHXL_GPTIMER2,
    CC1350_LAUNCHXL_GPTIMER3,
    CC1350_LAUNCHXL_GPTIMERCOUNT
} CC1350_LAUNCHXL_GPTimers;

/*!
 *  @def    CC1350_LAUNCHXL_PWM
 *  @brief  Enum of PWM outputs on the board
 */
typedef enum CC1350_LAUNCHXL_PWM
{
    CC1350_LAUNCHXL_PWM0 = 0,
    CC1350_LAUNCHXL_PWM1,
    CC1350_LAUNCHXL_PWM2,
    CC1350_LAUNCHXL_PWM3,
    CC1350_LAUNCHXL_PWM4,
    CC1350_LAUNCHXL_PWM5,
    CC1350_LAUNCHXL_PWM6,
    CC1350_LAUNCHXL_PWM7,
    CC1350_LAUNCHXL_PWMCOUNT
} CC1350_LAUNCHXL_PWM;

/*!
 *  @def    CC1350_LAUNCHXL_ADCBufName
 *  @brief  Enum of ADCBufs
 */
typedef enum CC1350_LAUNCHXL_ADCBufName {
    CC1350_LAUNCHXL_ADCBuf0 = 0,
    CC1350_LAUNCHXL_ADCBufCOUNT
} CC1350_LAUNCHXL_ADCBufName;


/*!
 *  @def    CC1350_LAUNCHXL_ADCName
 *  @brief  Enum of ADCs
 */
typedef enum CC1350_LAUNCHXL_ADCName {
    CC1350_LAUNCHXL_ADC0 = 0,
    CC1350_LAUNCHXL_ADC1,
    CC1350_LAUNCHXL_ADC2,
    CC1350_LAUNCHXL_ADC3,
    CC1350_LAUNCHXL_ADC4,
    CC1350_LAUNCHXL_ADC5,
    CC1350_LAUNCHXL_ADC6,
    CC1350_LAUNCHXL_ADC7,
    CC1350_LAUNCHXL_ADCDCOUPL,
    CC1350_LAUNCHXL_ADCVSS,
    CC1350_LAUNCHXL_ADCVDDS,
    CC1350_LAUNCHXL_ADCCOUNT
} CC1350_LAUNCHXL_ADCName;


#ifdef __cplusplus
}
#endif

#endif /* __CC1350_LAUNCHXL_BOARD_H__ */
