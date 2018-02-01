#ifndef COT_MT_LED_H
#define COT_MT_LED_H
/*********************************************************************
    Filename:       cot_mt_led.h
    Revised:        $Date: 2017-06-26 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the cot led API.

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

/**************************************************************************************************
 *                                             INCLUDES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* LEDS - The LED number is the same as the bit position */
#define COT_MT_LED_1     0x01
#define COT_MT_LED_2     0x02
#define COT_MT_LED_3     0x04
#define COT_MT_LED_ALL   (COT_MT_LED_1 | COT_MT_LED_2 | COT_MT_LED_3 )

/* Modes */
#define COT_MT_LED_MODE_OFF     0x00
#define COT_MT_LED_MODE_ON      0x01
#define COT_MT_LED_MODE_BLINK   0x02
#define COT_MT_LED_MODE_FLASH   0x04
#define COT_MT_LED_MODE_TOGGLE  0x08

/* Defaults */
#define COT_MT_LED_DEFAULT_MAX_LEDS      3
#define COT_MT_LED_DEFAULT_DUTY_CYCLE    5
#define COT_MT_LED_DEFAULT_FLASH_COUNT   50
#define COT_MT_LED_DEFAULT_FLASH_TIME    1000

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize LED Service.
 */
extern void cot_mt_led_init ( uint8_t task_id );

/*
 * Set the LED ON/OFF/TOGGLE.
 */
extern uint8_t cot_mt_led_set( uint8_t led, uint8_t mode );

/*
 * Blink the LED.
 */
extern void cot_mt_led_blink( uint8_t leds, uint8_t cnt, uint8_t duty, uint16_t time );

/*
 * Put LEDs in sleep state - store current values
 */
extern void cot_mt_led_enter_sleep( void );

/*
 * Retore LEDs from sleep state
 */
extern void cot_mt_led_exit_sleep( void );

/*
 * Return LED state
 */
extern uint8_t cot_mt_led_get_state ( void );

extern void cot_mt_led_update (void);

extern void cot_led_on_off (uint8_t leds, uint8_t mode);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif


#endif
