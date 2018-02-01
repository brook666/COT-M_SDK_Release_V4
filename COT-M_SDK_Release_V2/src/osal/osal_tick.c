/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include <stdio.h>
#include "am_hal_systick.h"
#include "hal_timers.h"
#include "osal_tick.h"
/**************************************************************************************************
 *                                            MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/
//vu16 osal_tick = 0;



/**************************************************************************************************
*
* @fn            SysTick_Configuration
*
* @brief
*
* @param
*
* @return
*
**************************************************************************************************/
void osal_systick_configuration(void)
{
    am_hal_systick_load(24000);
	am_hal_systick_int_enable();
    am_hal_systick_start();
	//timerA0_init();
}

uint32_t osal_get_systick( void )
{
    return g_tick ;
	//return g_osal_tick ;
}

void osal_systick_reset( void )
{
	//g_osal_tick = 0 ;
    g_tick = 0 ;
}

void osal_systick_stop(void)
{
    am_hal_systick_stop();
}

uint32_t osal_get_system_tick(void)
{
	return g_system_tick;
}

