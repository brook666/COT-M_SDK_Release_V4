/**
* @file         sample_app.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef SAMPLE_APP_H
#define SAMPLE_APP_H


/*********************************************************************
 * INCLUDE
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
	
#include "sample_sleep_mode.h"
	
/*********************************************************************
 * CONSTANTS
 */

#define SMAPLE_MSG_RX_EVENT                        				0x0001


#ifdef RTC_AWAKE_MODE
	#define SMAPLE_MSG_WAKE_EVENT								0x2000
#endif

#define	SOFT_VERSION											{3,0,0,0}
#define HARD_WARE_VERSION										{1,0}
/*!
 * \brief Represents the possible packet type (i.e. modem) used
 */
typedef enum
{
    MASTER_TX                        = 0x00,
    SLAVE_RX,
    MASTER_MOVE,
    SLAVE_ANCHOR,
}FunctionalMode;

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
* @name: sample_app_init
* This funtion deal with the sample app init
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-04-29
*/
void sample_app_init( uint8_t taskId );

uint8_t get_sample_task_id( void );

/**
 * @name: sample_app_processevent
 * This function provide simple useing of the rf lib apis.
 * @param[in]   task_id -- the osal task id.
 * @param[out]  events -- the event of the task_id.
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by zhangjh on 2017-04-29
 */
uint16_t sample_app_processevent(uint8_t task_id, uint16_t events);

#ifdef __cplusplus
}
#endif


#endif

