/**
* @file         sample_mpu9250.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef SAMPLE_MPU9250_H
#define SAMPLE_MPU9250_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * INCLUDE
 */
#include <stdint.h>

/*********************************************************************
 * CONSTANTS
 */

#define SMAPLE_MPU9250_INIT_EVENT                       0x0001
#define SMAPLE_MPU9250_RX_EVENT                         0x0002



/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
* @name: sample_mpu9250_init
* This funtion deal with the sample mpu9250 init
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
void sample_mpu9250_init( uint8_t taskId );


/**
 * @name: sample_mpu9250
 * This function provide simple useing of the sample mpu9250 apis.
 * @param[in]   task_id -- the osal task id.
 * @param[out]  events -- the event of the task_id.
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by wende.wu on 2017-06-19
 */
uint16_t sample_mpu9250(uint8_t task_id, uint16_t events);

#ifdef __cplusplus
}
#endif


#endif

