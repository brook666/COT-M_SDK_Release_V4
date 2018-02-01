/**
* @file         sample_bmp280.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef SAMPLE_BMP280_H
#define SAMPLE_BMP280_H

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

#define SMAPLE_BMP280_INIT_EVENT                       0x0001
#define SMAPLE_BMP280_RX_EVENT                         0x0002



/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
* @name: sample_bmp280_init
* This funtion deal with the bmp280 init
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
void sample_bmp280_init( uint8_t taskId );


/**
 * @name: sample_adc
 * This function provide simple useing of the bmp280 apis.
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
uint16_t sample_bmp280(uint8_t task_id, uint16_t events);

#ifdef __cplusplus
}
#endif
#endif

