/******************************************************************************

 @file  ccfg_app_ble.c

 @brief Customer Configuration CC26xx PG2 device family.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2014-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_01_18
 Release Date: 2016-10-26 15:20:04
 *****************************************************************************/

//
//       ===> READ THIS BEFORE MODIFYING THIS FILE
//
//
//       ===> READ THIS BEFORE MODIFYING THIS FILE
//
//
//       ===> READ THIS BEFORE MODIFYING THIS FILE
//

// The customer configuration area (ccfg section) is located at the end of the 
// flash and reflect the hw configuration of the device. it is very important 
// that it remains align with the version of driverlib you are using.
// all BLE project except sensor tag use the same configuration.
// Keeping the "#include <startup_files/ccfg.c>" guarantee that your project using 
// driverlib and the ccfg area will be align.

// you can modify it if you want, the recommend way will be to remove the 
// bellow include, copy the content of the <startup_files/ccfg.c> file in this
// file and rebuild.

// ==> KEEP IN MIND that if you do so, be sure that any further update of the 
// driverlib must be align with your modified version of ccfg area.

// Note: SNP_SBL_ENABLE should be defined to make Simple NP OAD upgradeable.
#ifdef SNP_SBL_ENABLE
#include "board.h"

#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0xC5       // Enable
#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xC5       // Enabled

#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x0        // Active low

#if defined (CC2650DK_7ID)    || \
    defined (CC2650_LAUNCHXL) || \
    defined (BOOSTXL_CC2650MA)
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                Board_MRDY // BL backdoor pin, Re-use MRDY
#else
#error "Board not recognized! must define CC2650DK_7ID, CC2650_LAUNCHXL or \
        BOOSTXL_CC2650MA."
#endif //CC2650DK_7ID || CC2650_LAUNCHXL || BOOSTXL_CC2650MA
#endif //SNP_SBL_ENABLE

#include <startup_files/ccfg.c>
