/**
* @file         cot_ranging.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       zhangjh
* @date     	2017-0429
* @version  	V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef COT_RANGING_H
#define COT_RANGING_H

/*********************************************************************
 * INCLUDE
 */
#include <stdint.h>
#include "osal.h"
#include "cot_mac.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
    osal_event_hdr_t            hdr;                    //osal event head
    uint8_t                     event_cmd;              //event cmd
    uint8_t  					src_task_id;			//the osal task id of the sendder
}cot_phy_ranging_msg_t;


/*!
 * \brief ranging output result structure
 */
#pragma pack(1)
typedef struct
{
    double moy;
    double squre_devi;
    int8_t rssi;
    uint8_t zn;
    double zmoy;
    double fei;
    double distance; //uint : m
    uint8_t cnt_packet_rx_ok;
}cot_ranging_result_t;
#pragma pack()


/*********************************************************************
 * CONSTANTS
 */

/*!
 * \brief Define antenna selection for ranging
 */
#define DEMO_RNG_ANT_1              1
#define DEMO_RNG_ANT_2              2
#define DEMO_RNG_ANT_BOTH           0

/*!
 * \brief Define units for ranging distances
 */
#define DEMO_RNG_UNIT_CONV_M        1.0
#define DEMO_RNG_UNIT_CONV_YD       1.0936
#define DEMO_RNG_UNIT_CONV_MI       6.2137e-4
#define DEMO_RNG_UNIT_SEL_M         0
#define DEMO_RNG_UNIT_SEL_YD        1
#define DEMO_RNG_UNIT_SEL_MI        2

#define RANGING_UNIT_SEL_M          0
#define RANGING_UNIT_SEL_YD         1
#define RANGING_UNIT_SEL_MI         2

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void cot_ranging_send_next_pkg( void );

uint8_t cot_ranging( PHYParamSettings_t *PHYParamSettings, cot_ranging_result_t *cot_ranging_res );

#ifdef __cplusplus
}
#endif


#endif
