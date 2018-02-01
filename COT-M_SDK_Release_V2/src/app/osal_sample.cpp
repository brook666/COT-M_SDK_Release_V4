/*********************************************************************
    Filename:       osal_sample.cpp
    Revised:        $Date: 2017-04-29 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

      .

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/

/*********************************************************************
* INCLUDES
*/
#include <stdio.h>
#include <stdint.h>
#include "osal.h"
#include "osal_tasks.h"
#include "osal_time.h"
#include "osal_pwrmgr.h"
#include "osal_memory.h"
#include "cot_ranging.h"
#include "cot_mac.h"
#include "sample_app.h"
#include "cot_mt_task.h"

/*********************************************************************
* MACROS
*/


/*********************************************************************
* CONSTANTS
*/

/*********************************************************************
* TYPEDEFS
*/

/*********************************************************************
* GLOBAL VARIABLES
*/
// The order in this table must be identical to the task initialization calls below in osalInitTask.

const pTaskEventHandlerFn tasksArr[] =
{
    cot_mac_layer_processevent,
    sample_app_processevent,
    cot_mt_process_event,
};

const uint8_t tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
//uint16_t myTasksEvents[tasksCnt];
//uint16_t *tasksEvents = myTasksEvents;
uint16_t *tasksEvents = NULL ;


/*********************************************************************
* FUNCTIONS
*/


/**********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
    uint8_t taskID = 0;

    tasksEvents = (uint16_t *)osal_mem_alloc( sizeof( uint16_t ) * tasksCnt);
    osal_memset( tasksEvents, 0, (sizeof( uint16_t ) * tasksCnt));

    /* phy layer Task */
    phy_layer_init(taskID++);

    /* sample application Task */
    sample_app_init(taskID++);

    /* mt task Task */
    mt_task_init(taskID++);

}






