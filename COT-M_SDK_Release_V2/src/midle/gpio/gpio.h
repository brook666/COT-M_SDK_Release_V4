#ifndef GPIO_H
#define GPIO_H
/*********************************************************************
    Filename:       gpio.h
    Revised:        $Date: 2017-03-25 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the gpio virtual file system Management API.

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

/*********************************************************************
 * MACROS
 */

/* sx1280 gpio visual file list */
#define PATH_SX1280     ("sx1280")    /*!< sx1280 */

#define SX1280_RESET_PIN        38
#define SX1280_BUSY_PIN         43
#define SX1280_DIO1_PIN         42
#define SX1280_DIO2_PIN         47
#define SX1280_DIO3_PIN         45


#define TEST_PIN                20

#define SX1280_DEVICES_ON_NOT_RESET     0x1
#define SX1280_DEVICES_ON_RESET         0x0



/* gpio device module */
/* (1) the args of open interface is set as baudrate. We may change in future*/
#define REQ_SET_SX1280_DIO1_INT 	(0x01)		/* set interrupt of sx1280 dio1 */
#define SX1280_PIN_DISABLE          (0x02)
#define SX1280_PIN_ENABLE           (0x04)
#define SX1280_DIO1_DISABLE			(0x05)

/* key gpio visual file list */
#define PATH_KEY1     ("KEY1")    /*!< KEY1 */
#define GPIO_KEY1_PIN		1//0

#define PATH_KEY2     ("KEY2")    /*!< KEY2 */
#define GPIO_KEY2_PIN		11//1

#define PATH_KEY3     ("KEY3")    /*!< KEY3 */
#define GPIO_KEY3_PIN		37//2

#define PATH_KEY4     ("KEY4")    /*!< KEY4 */
#define GPIO_KEY4_PIN		0//11

#define PATH_KEY5     ("KEY5")    /*!< KEY5 */
#define GPIO_KEY5_PIN		2//37

#define REQ_SET_GPIO_KEY_INT_ENABLE		(0x01)
#define REQ_SET_GPIO_KEY_INT_DISABLE	(0x02)
#define REQ_SET_GPIO_KEY_RELEASE		(0x03)
#define REQ_SET_GPIO_KEY_ACTIVE			(0x04)

/* led gpio visual file list */
#define LED_ON			0x01
#define LED_OFF			0x00

#define PATH_LED1     ("LED1")    /*!< LED1 */
#define GPIO_LED1_PIN		23

#define PATH_LED2     ("LED2")    /*!< LED2 */
#define GPIO_LED2_PIN		29

#define PATH_LED3     ("LED3")    /*!< LED3 */
#define GPIO_LED3_PIN		30

#define REQ_SET_GPIO_LED_RELEASE		(0x01)
#define REQ_SET_GPIO_LED_ACTIVE			(0x02)

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */

int32_t gpio_init_module(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif




#endif
