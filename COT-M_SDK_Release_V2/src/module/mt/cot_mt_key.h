#ifndef COT_MT_KEY_H
#define COT_MT_KEY_H
/*********************************************************************
    Filename:       cot_mt_key.h
    Revised:        $Date: 2017-03-25 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the cot key API.

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

/**************************************************************************************************
 * MACROS
 **************************************************************************************************/

/* Key state */
#define HAL_KEY_STATE_NORMAL          	0x01


#define COT_MT_KEY_SW_1 0x01  // key1
#define COT_MT_KEY_SW_2 0x02  // key2
#define COT_MT_KEY_SW_3 0x04  // key3
#define COT_MT_KEY_SW_4 0x08  // key4
#define COT_MT_KEY_SW_5 0x10  // key5

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halKeyCBack_t) (uint8_t keys, uint8_t state);
	

/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 *                                             FUNCTIONS - API
 **************************************************************************************************/

/*
 * Initialize the Key Service
 */
extern void cot_mt_key_init( uint8_t task_id );

extern bool get_cot_mt_key_int_enadble_status( void );

/*
 * Enter sleep mode, store important values
 */
extern void cot_key_enter_sleep ( void );

/*
 * Exit sleep mode, retore values
 */
extern uint8_t cot_key_exit_sleep ( void );

/*
 * This is for internal used by hal_driver
 */
extern void cot_mt_key_poll (void);


/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif


#endif
