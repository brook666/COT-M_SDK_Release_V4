#ifndef UART_H
#define UART_H

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

/*********************************************************************
 * MACROS
 */
/* visual file list */
#define PATH_USART1     ("com1")    /*!< usart1 */

/* usart device module */
/* (1) the args of open interface is set as baudrate. We may change in future*/
#define REQ_SET_BUADRATE 	(0x01)		/* set baudrate of usart x */
#define REQ_IT_RXNE         (0x02)		/* rxne interrupt of usart x, args can be ENABLE or DISABLE */
#define REQ_IT_TXE			(0x04)
#define REQ_IT_RX_TMOUT     (0x08)
#define REQ_IT_UART         (0x10)
#define REQ_UART_DISABLE    (0x20)
#define REQ_UART_ENABLE     (0x40)



int32_t uart_init_module(void);



/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif
