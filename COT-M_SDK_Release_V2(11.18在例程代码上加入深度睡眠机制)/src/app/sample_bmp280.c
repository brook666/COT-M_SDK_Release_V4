/**
* @file         sample_bmp280.c
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

/*********************************************************************
 * INCLUDE
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "osal.h"
#include "osal_time.h"
#include "osal_tasks.h"
#include "osal_pwrmgr.h"
#include "vfs.h"
#include "error.h"
#include "log.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "spi1.h"
#include "cot_mt_bmp280.h"
#include "cot_mt_oled.h"
#include "sample_bmp280.h"


/*********************************************************************
 * MACROS
 */
#define MSG(FORMAT, ARG...) /* message that is destined to the user */
#define MSG_DEBUG(FLAG, fmt, ...)                                                                         \
            do  {                                                                                         \
                if (FLAG)                                                                                 \
                {\
                    log_printf("%s:%d:%s(): " ,__FILE__, __LINE__, __FUNCTION__);\
					log_printf(fmt,##__VA_ARGS__);\
                }\
            } while (0)

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */



/*********************************************************************
 * EXTERNAL VARIABLES
 */
static uint16_t s_sample_bmp280_id = 0; /*<  sample application id for osal task id >*/


/*********************************************************************
 * EXTERNAL FUNCTIONS
 */



/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * PROFILE CALLBACKS
 */



/*********************************************************************
 * PUBLIC FUNCTIONS
 */


                
/**
* @name: sample_bmp280_init
* This funtion deal with the sample bmp280 init
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-05-29
*/
void sample_bmp280_init( uint8_t taskId )
{
    s_sample_bmp280_id = taskId ;
    osal_start_timerEx(s_sample_bmp280_id, SMAPLE_BMP280_INIT_EVENT, 300); 
}


/**
 * @name: sample_bmp280
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
 *      create by zhangjh on 2017-05-29
 */
uint16_t sample_bmp280(uint8_t task_id, uint16_t events)
{
    uint8_t *samle_msg_ptr = NULL;
    
    if ( events & SYS_EVENT_MSG )
    {
        while ((samle_msg_ptr = osal_msg_receive( task_id )) != NULL)
        {
            // Release the OSAL message
            osal_msg_deallocate((uint8_t *)samle_msg_ptr);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    
    if(events & SMAPLE_BMP280_INIT_EVENT)
    {
        cot_mt_bmp280_init();
        osal_set_event(task_id, SMAPLE_BMP280_RX_EVENT);
        // return unprocessed events
        return (events ^ SMAPLE_BMP280_INIT_EVENT);
    }

    if(events & SMAPLE_BMP280_RX_EVENT)
    {
        float temperature = 0,pressure = 0;
        uint8_t showstring[30] ={0};
        uint8_t i ;
        
        bmp280_read_pressure(&temperature,&pressure);

        sprintf((char *)showstring,"%.3f",pressure);
        sprintf((char *)showstring + 15,"%.3f",temperature);
        for(i =0; i< 15;i++)
            if(showstring[i] == 0 ) showstring[i] = ' ';
        OLED_ShowStr6x8(OLED_SHOWDARK,0,1,showstring);
        
        osal_start_timerEx(task_id, SMAPLE_BMP280_RX_EVENT, 3000); 
        // return unprocessed events
        return (events ^ SMAPLE_BMP280_RX_EVENT);
    }

    return 0;
}

