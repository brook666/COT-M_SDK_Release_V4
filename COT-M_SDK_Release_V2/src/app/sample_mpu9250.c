/**
* @file         sample_mpu9250.c
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
#include "cot_mt_mpu9250.h"
#include "cot_mt_oled.h"
#include "sample_mpu9250.h"


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
static uint16_t s_sample_mpu9250_id = 0; /*<  sample application id for osal task id >*/


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
void sample_mpu9250_init( uint8_t taskId )
{
    s_sample_mpu9250_id = taskId ;
    osal_set_event(s_sample_mpu9250_id, SMAPLE_MPU9250_INIT_EVENT);
}



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
uint16_t sample_mpu9250(uint8_t task_id, uint16_t events)
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
    
    if(events & SMAPLE_MPU9250_INIT_EVENT)
    {
        cot_mt_mpu9250_init();
        osal_start_timerEx(task_id, SMAPLE_MPU9250_RX_EVENT, 300); 
        // return unprocessed events
        return (events ^ SMAPLE_MPU9250_INIT_EVENT);
    }
    

    if(events & SMAPLE_MPU9250_RX_EVENT)
    {
        int32_t ret = 0;
        float temp = 0;
        int16_t gyr[3], acc[3], mag[3];
        uint8_t i,showstring[25] = {0};
        
        ret = cot_mt_mpu9250_GetRaw(gyr,acc,mag);
        cot_mt_mpu9250_GetTemp(&temp);

        if(ret == ERR_SUCCESS)
        {
            sprintf((char *)showstring ,"%d",gyr[0]); showstring[6] = ' ';
            sprintf((char *)showstring + 7,"%d",gyr[1]);showstring[13] = ' ';
            sprintf((char *)showstring + 14,"%d",gyr[2]);
            for(i =0; i< 15;i++)
                if(showstring[i] == 0) showstring[i] = ' ';
            showstring[21] = 0 ;
            OLED_ShowStr6x8(OLED_SHOWDARK,0,2,showstring);

            memset(showstring,0,sizeof(showstring));
            sprintf((char *)showstring ,"%d",acc[0]);showstring[6] = ' ';
            sprintf((char *)showstring + 7,"%d",acc[1]);showstring[13] = ' ';
            sprintf((char *)showstring + 14,"%d",acc[2]);
            for(i =0; i< 15;i++)
                if(showstring[i] == 0) showstring[i] = ' ';
            showstring[21] = 0 ;
            OLED_ShowStr6x8(OLED_SHOWDARK,0,3,showstring);

            memset(showstring,0,sizeof(showstring));
            sprintf((char *)showstring ,"%d",mag[0]);showstring[6] = ' ';
            sprintf((char *)showstring + 7,"%d",mag[1]);showstring[13] = ' ';
            sprintf((char *)showstring + 14,"%d",mag[2]);
            for(i =0; i< 15;i++)
                if(showstring[i] == 0) showstring[i] = ' ';
            showstring[21] = 0 ;
            OLED_ShowStr6x8(OLED_SHOWDARK,0,4,showstring);
        }
        memset(showstring,0,sizeof(showstring));
        sprintf((char *)showstring ,"%.3f",temp);
        for(i =0; i< 15;i++)
            if(showstring[i] == 0 ) showstring[i] = ' ';
        showstring[16] = 0 ;
        OLED_ShowStr6x8(OLED_SHOWDARK,0,5,showstring);
        
        
        osal_start_timerEx(task_id, SMAPLE_MPU9250_RX_EVENT, 3000); 
        // return unprocessed events
        return (events ^ SMAPLE_MPU9250_RX_EVENT);
    }
    return 0;
}


