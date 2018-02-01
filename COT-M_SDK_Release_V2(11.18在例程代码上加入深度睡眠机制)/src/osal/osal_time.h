/**************************************************************************************************
* Filename:             osal_time.h
* Revised:
* Revision:
* Description:
**************************************************************************************************/
#ifndef osal_time_H
#define osal_time_H

#ifdef __cplusplus

extern "C"
{

#endif
/*********************************************************************
* INCLUDES
*/
// #include "stm32l051xx.h"

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

/*********************************************************************
* FUNCTIONS
*/

extern void osalTimerUpdate( uint32_t updateTime );
extern void osalTimerInit( void );
extern uint8_t osal_start_timerEx( uint8_t taskID, uint16_t event_id, uint16_t timeout_value );
extern uint8_t osal_stop_timerEx( uint8_t task_id, uint16_t event_id );
extern uint16_t osal_get_timeoutEx( uint8_t task_id, uint16_t event_id );
extern uint8_t osal_timer_num_active( void );
extern uint32_t osal_GetSystemClock( void );
extern uint32_t osal_next_timeout( void );
/*********************************************************************
*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
