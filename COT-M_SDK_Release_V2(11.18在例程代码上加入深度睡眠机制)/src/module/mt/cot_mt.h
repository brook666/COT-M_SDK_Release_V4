/**
* @file         cot_mt.h
* @brief        MonitorTest command and response definitions.
* @details      This is the detail description.
* @author       zhangjh
* @date         2017-06-01
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/



#ifndef MT_H
#define MT_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <stdint.h>
#include "OSAL.h"

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/


//serial event cmd
#define SERIAL_EVENT_INIT_CMD       0x01
#define SERIAL_EVENT_RECV_CMD       0x02
#define SERIAL_EVENT_SEND_CMD       0x03
#define SERIAL_EVENT_RESPONSE_CMD   0x04
#define SERIAL_EVENT_DISABLE_CMD    0x05
#define SERIAL_EVENT_ENABLE_CMD     0x06

//key event cmd
#define KEY_EVENT_PRESS_DEBOUNCE_CMD		0x10
#define KEY_EVENT_PRESSED_CMD				0x11
#define KEY_EVENT_DETECT_INT_ENABLE_CMD		0x12
#define KEY_EVENT_CMD_KEY1_PRESSED			0x13
#define KEY_EVENT_CMD_KEY2_PRESSED			0x14
#define KEY_EVENT_CMD_KEY3_PRESSED			0x15
#define KEY_EVENT_CMD_KEY4_PRESSED			0x16
#define KEY_EVENT_CMD_KEY5_PRESSED			0x17




//led event cmd
#define STOP_COT_MT_LED_BLINK_CMD				0x20
#define START_COT_MT_LED_BLINK_CMD				0x21
#define START_COT_MT_LED_BLINK_IN_TIMEOUT_CMD	0x22



/***************************************************************************************************
 * TYPEDEFS
 ***************************************************************************************************/

typedef struct
{
	osal_event_hdr_t  	hdr;
	uint8_t				task_id;
    uint8_t             event_cmd;
	uint16_t          	msg_len;
	uint8_t           	*msg;
} mt_osal_msg_data_t;


/***************************************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************************************/





#ifdef __cplusplus
}
#endif

#endif
