/**
* @file         cot_mt_uart.c
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       zhangjh
* @date         2017-06-01
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/


/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <string.h>
#include <stdint.h>
#include "OSAL.h"
#include "OSAL_Memory.h"
#include "vfs.h"
#include "uart.h"
#include "base64.h"
#include "cot_mt.h"
#include "cot_mt_uart.h"


/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
/* State values for ZTool protocal */
#define STX_STATE       0x00
#define DATA_LEN_STATE  0x01
#define DATA_STATE      0x02
#define ETX_STATE       0x03


/*frame extra len contain stx,payload len,etx*/
#define FRAME_EXTRA_LEN     4

/* Start-of-frame delimiter for UART transport */
#define STX     0x02

/* End-of-frame delimiter for UART transport */
#define ETX     0x03


/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/
/* cot application protocal parameters */
static uint8_t  s_state;
static uint16_t s_index;
static uint8_t  s_uart_buf[288];

mt_osal_msg_data_t  *g_pMsg;



/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/
static uint8_t calculate_lrc( uint8_t *src_data , uint16_t len );


static uint8_t calculate_lrc( uint8_t *src_data , uint16_t len )
{
	uint16_t i = 0;
	uint8_t lrc = 0 ;

	for(i=0; i<len; i++)
	{
		lrc ^= src_data[i];
	}

	return lrc ;
}

/***************************************************************************************************
 *                                          PUBLIC FUNCTIONS
 ***************************************************************************************************/
void cot_mt_uart_init( void )
{
    s_state = STX_STATE;
    s_index = 0;
    memset(s_uart_buf,0x0,sizeof(s_uart_buf));
}

/***************************************************************************************************
 * @fn      cot_mt_uart_data_process
 *
 * @brief   | STX | Data Length  | RF_HDR |  CMD   |   Data    |  LRC  |  ETX |
 *          |  1  |     2        | 	1	  |   2    |   0-Len   |   1   |   1  |
 *
 *          Parses the data and determine either is  just simply serial data
 *          then send the data to correct place (MT or APP); Data Length contain RF_HDR + CMD + Data;
 *			LRC is the xor of the (RF_HDR,CMD and Data)
 *
 * @param   task_id     - osal task id
 *          event    - Event that causes the callback
 *
 *
 * @return  None
 ***************************************************************************************************/
void cot_mt_uart_data_process ( uint8_t task_id, uint16_t event )
{
    uint8_t     ch=0;
    int32_t     uart_fd;
    uint8_t     temp[344];

    (void)event;  // Intentionally unreferenced parameter

    uart_fd = open(PATH_USART1,0);
    if( uart_fd <= 0 )
        return ;

    while ( read(uart_fd,&ch,1)==1 )
    {
        switch (s_state)
        {
            case STX_STATE:
                if (ch == STX)
                {
                    s_state = DATA_STATE;
                    s_index++;
                    memset(temp,0x0,sizeof(temp));
                }
                break;
            case DATA_STATE:
				if( ch != ETX )
				{
					temp[s_index-1] = ch;
					s_index++;
				}
				else
				{
					s_index++;
					memset(s_uart_buf,0x0,sizeof(s_uart_buf));
					uint16_t data_len = base64Decode(temp,s_index-2,s_uart_buf);
                    if( data_len <=0 )
                    {
                        s_index = 0;
						s_state = STX_STATE;
						close(uart_fd);
						return;
                    }

					uint8_t	lrc = 0 ;
					uint8_t read_lrc = s_uart_buf[data_len-1];
					lrc = calculate_lrc(s_uart_buf+2,data_len-3);////2 replace data len(2byte);1--replace lrc(1byte)
					if( lrc != read_lrc )
					{
                        s_index = 0;
						s_state = STX_STATE;
						close(uart_fd);
						return;
					}

					g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+data_len-3);////3 replace 2byte data len and 1byte lrc
					if (g_pMsg)
					{
						/* Fill up what we can */
						g_pMsg->hdr.event = CMD_SERIAL_MSG;
                        g_pMsg->event_cmd = SERIAL_EVENT_RECV_CMD;
						g_pMsg->msg = (uint8_t*)(g_pMsg+1);
						g_pMsg->msg_len = data_len-3;////3 replace 2byte data len and 1byte lrc
						memcpy(g_pMsg->msg,s_uart_buf+2,g_pMsg->msg_len);////2 replace 2byte data_len
						osal_msg_send(task_id,(uint8_t *)g_pMsg);
                        s_index = 0;
                        s_state = STX_STATE;
					}
					else
					{
                        s_index = 0;
						s_state = STX_STATE;
						close(uart_fd);
						return;
					}

				}
				break;

          default:
                break;
        }
    }

	close(uart_fd);
}


/***************************************************************************************************
 * @fn      cot_mt_uart_data_process
 *
 * @brief   | STX | Data Length  | RF_HDR |  CMD   |   Data    |  LRC  |  ETX |
 *          |  1  |     2        | 	1	  |   2    |   0-Len   |   1   |   1  |
 *
 *          Parses the data and determine either is  just simply serial data
 *          then send the data to correct place (MT or APP); Data Length contain RF_HDR + CMD + Data;
 *			LRC is the xor of the (RF_HDR,CMD and Data)
 *
 * @param   task_id     - osal task id
 *          event    - Event that causes the callback
 *
 *
 * @return  None
 ***************************************************************************************************/
void cot_mt_uart_transport_out( mt_osal_msg_data_t *msg_ptr )
{
    uint8_t     temp[344];
    uint8_t     lrc = 0;
    uint16_t    index = 0;
    uint16_t    len = 0;
    int32_t     uart_fd;

    memset(s_uart_buf,0x0,sizeof(s_uart_buf));

    s_uart_buf[index++] = (msg_ptr->msg_len&0xFF00) >> 8;
    s_uart_buf[index++] = msg_ptr->msg_len & 0xFF;
    memcpy(s_uart_buf+index,msg_ptr->msg,msg_ptr->msg_len);
    index += msg_ptr->msg_len;

    lrc = calculate_lrc(msg_ptr->msg,msg_ptr->msg_len);
    s_uart_buf[index++] = lrc;

    memset(temp,0x0,sizeof(temp));
    temp[0] = STX;
    len = base64Encode(s_uart_buf, index, temp+1);
    temp[len+1] = ETX;

    uart_fd = open(PATH_USART1,0);
    if( uart_fd > 0 )
        write(uart_fd, temp, len+2);
    close(uart_fd);

}



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
void cot_mt_uart_disable( void )
{
    int32_t uart_fd = 0;
    
    uart_fd = open(PATH_USART1,0);
    if( uart_fd > 0 )
    {
        ioctl(uart_fd,REQ_UART_DISABLE,0);
        close(uart_fd);
    }
}


/** 
* @name: cot_mt_uart_enable 
* This function deal with the uart enable.
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
void cot_mt_uart_enable( void )
{
    int32_t uart_fd = 0;
    
    uart_fd = open(PATH_USART1,0);
    if( uart_fd > 0 )
    {
        ioctl(uart_fd,REQ_UART_ENABLE,0);
        close(uart_fd);
    }
}


