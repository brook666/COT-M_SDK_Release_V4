/*********************************************************************
    Filename:       spi1.h
    Revised:        $Date: 2017-06-29 14:57:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the spi1 virtual file system Management API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/
#ifndef SPI1_H
#define SPI1_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "am_hal_pin.h"
#include "am_hal_iom.h"

/*********************************************************************
 * MACROS
 */
/* visual file list */
#define PATH_SPI1     ("SPI1")    /*!< spi */


/*********************************************************************
 * GLOBAL VARIABLES
 */
    
#define SPI1_IOM_MODULE             1

/* SPI1 pins define */

#define MPU9250_NSS_PIN             14
#define MPU9250_SPI1_NSS            AM_HAL_PIN_14_M1nCE2 // 
#define SPI1_CHIP_SELECT_MPU9250    2
    
#define BMP280_NSS_PIN              22
#define BMP280_SPI1_NSS             AM_HAL_PIN_22_M1nCE7 //                               
#define SPI1_CHIP_SELECT_BMP280     7

#define SPI1_MISO_PIN               9
#define SPI1_MISO                   AM_HAL_PIN_9_M1MISO
#define SPI1_MOSI_PIN               10
#define SPI1_MOSI                   AM_HAL_PIN_10_M1MOSI
#define SPI1_SCK_PIN                8
#define SPI1_SCK                    AM_HAL_PIN_8_M1SCK

#define SPI1_CLOCK_FREQUENCY                     (0x00000001)
#define SET_SPI1_OPTION                          (0x00000002)
#define SET_SPI1_WRITE_FINAL_FRAME_NSS_STATUS    (0x00000004)
#define SET_SPI1_DISABLE                         (0x00000008)
#define SPI1_CHIPSELECT_BMP280                   (0x00000010)
#define SPI1_CHIPSELECT_MPU9250                  (0x00000020)


#define SPI1_SET_NSS_HIGH           1
#define SPI1_SET_NSS_LOW            2


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */

/* spi1 device module */
/* (1) the args of open interface is set as baudrate. We may change in future*/
int32_t spi1_init_module(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif
#endif

