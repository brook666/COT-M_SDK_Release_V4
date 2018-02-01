#ifndef SPI_H
#define SPI_H

/*********************************************************************
    Filename:       UART.h
    Revised:        $Date: 2017-03-14 14:57:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the uart virtual file system Management API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/

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
#define PATH_SPI0     ("SPI0")    /*!< spi */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/* SPI0 pins define */
#define SX1280_MISO_PIN         6
#define SX1280_MISO             AM_HAL_PIN_6_M0MISO
#define SX1280_MOSI_PIN         7
#define SX1280_MOSI             AM_HAL_PIN_7_M0MOSI
#define SX1280_SCK_PIN          5
#define SX1280_SPI_SCK          AM_HAL_PIN_5_M0SCK  //(AM_HAL_PIN_5_M0SCK | AM_HAL_GPIO_HIGH_DRIVE)
#define SX1280_NSS_PIN          44
#define SX1280_SPI_NSS         	AM_HAL_PIN_44_M0nCE2 //M0nCE2 -- select spi master 0 the 2nd slave device, so SX1280_CHIP_SELECT=2

#define SX1280_IOM_MODULE       0
#define SX1280_CHIP_SELECT      2


#define SPI0_MISO_PIN           SX1280_MISO_PIN
#define SPI0_MISO               SX1280_MISO
#define SPI0_MOSI_PIN           SX1280_MOSI_PIN
#define SPI0_MOSI               SX1280_MOSI
#define SPI0_SCK_PIN            SX1280_SCK_PIN
#define SPI0_SCK                SX1280_SPI_SCK
#define SPI0_NSS_PIN            SX1280_NSS_PIN
#define SPI0_NSS                SX1280_SPI_NSS
#define SPI0_IOM_MODULE         SX1280_IOM_MODULE
#define SPI0_CHIP_SELECT        SX1280_CHIP_SELECT


#define SPI1_IOM_MODULE         1


#define CLOCK_FREQUENCY                         (0x00000001)
#define SET_SPI_OPTION                          (0x00000002)
#define SET_SPI_WRITE_FINAL_FRAME_NSS_STATUS    (0x00000004)
#define SET_SPI_DISABLE                         (0x00000008)
#define SET_SPI_ENABLE                          (0x00000010)

#define SET_NSS_HIGH    1
#define SET_NSS_LOW     2


#define SPI_READ_ADDER      (1)
#define SPI_WRITE_ADDER     (2)

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * FUNCTIONS
 */

/* spi device module */
/* (1) the args of open interface is set as baudrate. We may change in future*/
int32_t spi_init_module(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif
