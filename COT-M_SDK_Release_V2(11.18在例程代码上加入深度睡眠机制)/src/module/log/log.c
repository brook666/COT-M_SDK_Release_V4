/*********************************************************************
    Filename:       log.c
    Revised:        $Date: 2017-04-06 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the logger system Management API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/


/*********************************************************************
 * INCLUDES
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "am_util_stdio.h"
#include "vfs.h"
#include "uart.h"
#include "log.h"


/*********************************************************************
 * MACROS
 */



/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */
static void log_uart_print(char *pcString);



static void log_uart_print(char *pcString)
{
    uint32_t ui32Length = 0;
    int32_t fd = 0 ;

    char *send_buf = pcString ;

    fd = open(PATH_USART1,0);
    if( fd <=0 )
        return;
    //
    // Determine the length of the string.
    //
    while (*(send_buf + ui32Length))
    {
        ui32Length++;
    }

    write(fd,send_buf,ui32Length);

    close(fd);

}


void log_init_module(void)
{
    am_util_stdio_printf_init(log_uart_print);
}

/*********************************************************************
 * @fn      log_printf
 *
 * @brief
 *
 *   printf the log,the data size of log should be smaller than 1024 byte.
 *
 * @param [in]  const char *pui8Fmt - pointer to text string
 *
 * @return  uint32_t - number of print
 */
uint32_t log_printf(const char *pui8Fmt, ...)
{
    return am_util_stdio_printf(pui8Fmt);
}


void log_terminal_clear(void)
{
    am_util_stdio_terminal_clear();
}


void print_array(uint8_t *arr_name,uint32_t frame_len )
{
	uint32_t i = 0;
	
	for(i=0 ; i<frame_len; i++)
		log_printf("%2x,",arr_name[i]);
	
	
}

