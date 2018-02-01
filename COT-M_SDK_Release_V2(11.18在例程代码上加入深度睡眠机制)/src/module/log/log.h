#ifndef LOG_H
#define LOG_H
/*********************************************************************
    Filename:       log.h
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



/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */

void log_init_module(void);

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
uint32_t log_printf(const char *pui8Fmt, ...);

void log_terminal_clear(void);

void print_array(uint8_t *arr_name,uint32_t frame_len );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif




#endif
