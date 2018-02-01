/**
* @file         cot_mac_protocol_parse.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       zhangjh
* @date     	2017-08-17
* @version  	V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/


#ifndef COT_MAC_PROTOCOL_PARSE_H
#define COT_MAC_PROTOCOL_PARSE_H

#include <stdint.h>
#include "radio.h"
#include "sx1280-hal.h"
#include <stdlib.h>
#include <string.h>


/*!
 * \brief Define current demo mode
 */
typedef enum
{
    BEACON              = 0x00,
    ACK                 = 0x01,
    COMMAND             = 0x02,
    NORMAL_STREAM_UP    = 0x0c,
    EMERG_STREAM_UP     = 0x0d,
    NORMAL_STREAM_DOWN  = 0x0e,
    EMERG_STREAM_DOWN   = 0x0f,
}mhdr_type_t;


/*!
 * cot mac header field definition (PHDR field)
 *
 * COTWAN Specification VX.X.X, chapter X.X
 */
typedef union uCOTMACHeader
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * Structure containing single access to header bits
     */
    struct sHdrBits
    {
        /*!
         * reserved
         */
        uint8_t reserved            : 2;

        /*!
         * FramePending
         */
        uint8_t frame_pending       : 1;

        /*!
         * FramePending
         */
        uint8_t ack_request         : 1;


        /*!
         * Message type
         */
        uint8_t MType               : 4;

    }Bits;
}cot_mac_header_t;


/*!
 * cot phy header field definition (PHDR field)
 *
 * COTWAN Specification VX.X.X, chapter X.X
 */
typedef struct
{
    cot_mac_header_t    cot_mhdr;
    uint16_t            cot_fcnt;
    uint16_t            cot_id;
    uint32_t            dst_addr;
    uint32_t            src_addr;
    uint8_t             *mac_payload;
    uint32_t            cot_mic;
}cot_phy_payload_t;


#define offscfof(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

#define container_of(ptr,type,member) (type *)((char *)ptr-offscfof(type,member))

typedef struct _student1{
    int num;
    char name[20];
    int pNext;
}Student1;


typedef int8_t (*get_channel_power_callback_t)(void);

typedef void (*send_mac_paylod_callback_t)(uint8_t *src_data, uint32_t data_len, PacketParams_t *packet_param,uint16_t irq_type,TickTime_t time_out );



void cot_mac_protocol_parse_callback_register( get_channel_power_callback_t get_chain_power_callback, send_mac_paylod_callback_t send_mac_paylod_callback );

int32_t send_pkg_by_backoffPeriod( uint8_t *src_dara , uint32_t data_len,
                                   ModulationParams_t *modulation_params,PacketParams_t *packet_param,
                                   uint16_t IrqMask,TickTime_t time_out );

int32_t radio_ram_data_format_to_cot_phy_payload( uint8_t *src_data, uint32_t data_len,
                                                  cot_phy_payload_t * cot_phy_payload, uint16_t *mac_payload_len);

int32_t cot_mac_msg_format_to_cot_phy_payload( uint8_t *src_data, uint16_t data_len , uint32_t dst_addr, uint32_t src_addr,
                                               uint8_t pkg_hdr, cot_phy_payload_t * cot_phy_payload, uint32_t *mac_payload_len );

int32_t cot_phy_payload_format_to_radio_ram_data( cot_phy_payload_t * cot_phy_payload, uint16_t mac_payload_len,
                                                  uint8_t *src_data, uint32_t *data_len );


uint32_t get_symbols_of_ack_in_lr24(RadioLoRaSpreadingFactors_t sf, RadioLoRaBandwidths_t bw);

void struct_offet_calculate_test( void );

void set_rssi_water_leve(int16_t value);



#endif
