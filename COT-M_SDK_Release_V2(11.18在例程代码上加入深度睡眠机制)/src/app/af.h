/**
* @file         af.h
* @brief        This file contains the Application Framework definitions.
* @details      This is the detail description.
* @author       zhangjh
* @date         2017-06-04
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef AF_H
#define AF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*!
 * cot phy header field definition (PHDR field)
 *
 * COTWAN Specification VX.X.X, chapter X.X
 */
typedef union uRFHDR
{
    /*!
     * Byte-access to the bits
     */
    uint8_t value;
    /*!
     * Structure containing single access to header bits
     */
    struct sRfHdrBits
    {
        /*!
         * tx power
         */
        uint8_t tx_power            : 5;

        /*!
         * Message type
         */
        uint8_t sf                  : 3;
    }Bits;
}cot_rf_hdr_t;


typedef struct
{
    osal_event_hdr_t    hdr;     /* OSAL Message header */
    cot_rf_hdr_t        rf_hdr;
    uint8_t             cmd_h;
    uint8_t             cmd_l;
    uint32_t            src_dev_addr;/* src device ID - 0 if not set */
    uint32_t            dst_dev_addr;/* dst device ID */
    uint8_t             data_len;
    uint8_t             data[256];
} af_serial_incoming_msg_packet_t;




#ifdef __cplusplus
}
#endif

#endif
