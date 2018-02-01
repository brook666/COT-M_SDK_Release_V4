/**
* @file         cot_mt_uart.h
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       zhangjh
* @date         2017-06-01
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef COT_MT_UART_H
#define COT_MT_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 *                                               INCLUDES
 ***************************************************************************************************/


/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/



/*
 * Process Rx uart Data
 */
void cot_mt_uart_data_process ( uint8_t task_id, uint16_t event );


void cot_mt_uart_transport_out( mt_osal_msg_data_t *msg_ptr );



/** 
* @name: cot_mt_uart_disable 
* This function deal with the uart disable.
* @param[in]   none.
* @param[out]  none. 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void cot_mt_uart_disable(void);


/** 
* @name: cot_mt_uart_enable 
* This function deal with the uart enable.
* @param[in]   none
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-15
*/
void cot_mt_uart_enable(void);



#endif  /* COT_MT_UART_H */
