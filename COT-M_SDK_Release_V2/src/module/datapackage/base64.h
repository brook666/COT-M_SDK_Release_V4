
#ifndef _BASE64_H
#define _BASE64_H

/*********************************************************************
    Filename:       base64.h
    Revised:        $Date: 2017-03-31 14:57:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       

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
    uint16_t base64Encode( const uint8_t *bindata,uint16_t binlength,uint8_t *base64 );
    uint16_t base64Decode( const uint8_t * base64,uint16_t base64len, uint8_t *bindata);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif


