
#ifndef _DATAPACKAGE_H
#define _DATAPACKAGE_H


/*********************************************************************
    Filename:       data_package.h
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
    typedef struct
    {
        uint16_t    cmd;
        uint16_t    data_len;
        uint8_t    *data;
    } packagedata_t;


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */
    uint8_t makeDataPackage(packagedata_t send_data,uint8_t *outdata,uint16_t *outdata_len);
    uint8_t ReceiveHandle(const uint8_t *redata,const uint16_t redata_len, packagedata_t *Receive_data);
    uint16_t check_receivedata(const uint8_t *redata,const uint16_t redata_len, uint16_t *count);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif


