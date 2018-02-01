
/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "osal_time.h"
#include "osal_memory.h"
#include "osal_tick.h"
/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * CONSTANTS
 * the unit is chosen such that the 320us tick equivalent can fit in
 * 32 bits.
 */
#define OSAL_TIMERS_MAX_TIMEOUT 0x28f5c28e /* unit is ms*/

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  void *next;
  uint32_t timeout;
  uint16_t event_flag;
  uint8_t task_id;
} osalTimerRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

osalTimerRec_t *timerHead;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Milliseconds since last reboot
static uint32_t  osal_systemClock;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
osalTimerRec_t  *osalAddTimer( uint8_t task_id, uint16_t event_flag, uint16_t timeout );
osalTimerRec_t *osalFindTimer( uint8_t task_id, uint16_t event_flag );
void osalDeleteTimer( osalTimerRec_t *rmTimer );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalTimerInit
 *
 * @brief   Initialization for the OSAL Timer System.
 *
 * @param   none
 *
 * @return
 */
void osalTimerInit( void )
{
  osal_systemClock = 0;
  osal_systick_configuration();// need to do get systick count
}

/*********************************************************************
 * @fn      osalAddTimer
 *
 * @brief   Add a timer to the timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 * @param   timeout
 *
 * @return  osalTimerRec_t * - pointer to newly created timer
 */
osalTimerRec_t * osalAddTimer( uint8_t task_id, uint16_t event_flag, uint16_t timeout )
{
  osalTimerRec_t *newTimer;
  osalTimerRec_t *srchTimer;

  // Look for an existing timer first
  newTimer = osalFindTimer( task_id, event_flag );
  if ( newTimer )
  {
    // Timer is found - update it.
    newTimer->timeout = timeout;

    return ( newTimer );
  }
  else
  {
    // New Timer
    newTimer = osal_mem_alloc( sizeof( osalTimerRec_t ) );

    if ( newTimer )
    {
      // Fill in new timer
      newTimer->task_id = task_id;
      newTimer->event_flag = event_flag;
      newTimer->timeout = timeout;
      newTimer->next = (void *)NULL;

      // Does the timer list already exist
      if ( timerHead == NULL )
      {
        // Start task list
        timerHead = newTimer;
      }
      else
      {
        // Add it to the end of the timer list
        srchTimer = timerHead;

        // Stop at the last record
        while ( srchTimer->next )
          srchTimer = srchTimer->next;

        // Add to the list
        srchTimer->next = newTimer;
      }

      return ( newTimer );
    }
    else
      return ( (osalTimerRec_t *)NULL );
  }
}

/*********************************************************************
 * @fn      osalFindTimer
 *
 * @brief   Find a timer in a timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 *
 * @return  osalTimerRec_t *
 */
osalTimerRec_t *osalFindTimer( uint8_t task_id, uint16_t event_flag )
{
  osalTimerRec_t *srchTimer;

  // Head of the timer list
  srchTimer = timerHead;

  // Stop when found or at the end
  while ( srchTimer )
  {
    if ( srchTimer->event_flag == event_flag &&
         srchTimer->task_id == task_id )
      break;

    // Not this one, check another
    srchTimer = srchTimer->next;
  }

  return ( srchTimer );
}

/*********************************************************************
 * @fn      osalDeleteTimer
 *
 * @brief   Delete a timer from a timer list.
 *
 * @param   table
 * @param   rmTimer
 *
 * @return  none
 */
void osalDeleteTimer( osalTimerRec_t *rmTimer )
{
  // Does the timer list really exist
  if ( rmTimer )
  {
    // Clear the event flag and osalTimerUpdate() will delete
    // the timer from the list.
    rmTimer->event_flag = 0;
  }
}

/*********************************************************************
 * @fn      osal_start_timerEx
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event.
 *
 * @param   u8 taskID - task id to set timer for
 * @param   u16 event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
uint8_t osal_start_timerEx( uint8_t taskID, uint16_t event_id, uint16_t timeout_value )
{
  osalTimerRec_t *newTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  // Add timer
  newTimer = osalAddTimer( taskID, event_id, timeout_value );

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      osal_stop_timerEx
 *
 * @brief
 *
 *   This function is called to stop a timer that has already been started.
 *   If ZSUCCESS, the function will cancel the timer and prevent the event
 *   associated with the timer from being set for the calling task.
 *
 * @param   u8 task_id - task id of timer to stop
 * @param   u16 event_id - identifier of the timer that is to be stopped
 *
 * @return  SUCCESS or INVALID_EVENT_ID
 */
uint8_t osal_stop_timerEx( uint8_t task_id, uint16_t event_id )
{
  osalTimerRec_t *foundTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  // Find the timer to stop
  foundTimer = osalFindTimer( task_id, event_id );
  if ( foundTimer )
  {
    osalDeleteTimer( foundTimer );
  }

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.


  return ( (foundTimer != NULL) ? SUCCESS : INVALID_EVENT_ID );
}

/*********************************************************************
 * @fn      osal_get_timeoutEx
 *
 * @brief
 *
 * @param   u8 task_id - task id of timer to check
 * @param   u16 event_id - identifier of timer to be checked
 *
 * @return  Return the timer's tick count if found, zero otherwise.
 */
uint16_t osal_get_timeoutEx( uint8_t task_id, uint16_t event_id )
{
    uint16_t rtrn = 0;
    osalTimerRec_t *tmr;

    HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

    tmr = osalFindTimer( task_id, event_id );

    if ( tmr )
    {
        rtrn = tmr->timeout;
    }

    HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

    return rtrn;
}

/*********************************************************************
 * @fn      osal_timer_num_active
 *
 * @brief
 *
 *   This function counts the number of active timers.
 *
 * @return  u8 - number of timers
 */
uint8_t osal_timer_num_active( void )
{
    uint8_t num_timers = 0;
    osalTimerRec_t *srchTimer;

    HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

    // Head of the timer list
    srchTimer = timerHead;

    // Count timers in the list
    while ( srchTimer != NULL )
    {
        num_timers++;
        srchTimer = srchTimer->next;
    }

    HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

    return num_timers;
}

/*********************************************************************
 * @fn      osalTimerUpdate
 *
 * @brief   Update the timer structures for a timer tick.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osalTimerUpdate( uint32_t updateTime )
{
    osalTimerRec_t *srchTimer;
    osalTimerRec_t *prevTimer;

    HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.
    // Update the system time
    osal_systemClock += updateTime;
    HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

    // Look for open timer slot
    if ( timerHead != NULL )
    {
        // Add it to the end of the timer list
        srchTimer = timerHead;
        prevTimer = (void *)NULL;

        // Look for open timer slot
        while ( srchTimer )
        {
            osalTimerRec_t *freeTimer = NULL;

            HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

            if (srchTimer->timeout <= updateTime)
            {
                srchTimer->timeout = 0;
            }
            else
            {
                srchTimer->timeout = srchTimer->timeout - updateTime;
            }

            // When timeout or delete (event_flag == 0)
            if ( srchTimer->timeout == 0 || srchTimer->event_flag == 0 )
            {
                // Take out of list
                if ( prevTimer == NULL )
                    timerHead = srchTimer->next;
                else
                    prevTimer->next = srchTimer->next;

                // Setup to free memory
                freeTimer = srchTimer;

                // Next
                srchTimer = srchTimer->next;
            }
            else
            {
                // Get next
                prevTimer = srchTimer;
                srchTimer = srchTimer->next;
            }

            HAL_EXIT_CRITICAL_SECTION( );   // Re-enable interrupts.

            if ( freeTimer )
            {
                if ( freeTimer->timeout == 0 )
                {
                    osal_set_event( freeTimer->task_id, freeTimer->event_flag );
                }
                osal_mem_free( freeTimer );
            }
        }
    }
}

#ifdef POWER_SAVING
/*********************************************************************
 * @fn      osal_adjust_timers
 *
 * @brief   Update the timer structures for elapsed ticks.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osal_adjust_timers( void )
{
#if 0
	uint32_t eTime;

	if ( timerHead != NULL )
	{
		// Compute elapsed time (msec)
		eTime = TimerElapsed() / TICK_COUNT;
		if ( eTime )
		{
			osalTimerUpdate( eTime );
		}
	}
#endif

}
#endif /* POWER_SAVING */

#ifdef POWER_SAVING
/*********************************************************************
 * @fn      osal_next_timeout
 *
 * @brief
 *
 *   Search timer table to return the lowest timeout value. If the
 *   timer list is empty, then the returned timeout will be zero.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
uint32_t osal_next_timeout( void )
{
    uint32_t nextTimeout;
    osalTimerRec_t *srchTimer;

    if ( timerHead != NULL )
    {
        // Head of the timer list
        srchTimer = timerHead;
        nextTimeout = OSAL_TIMERS_MAX_TIMEOUT;

        // Look for the next timeout timer
        while ( srchTimer != NULL )
        {
            if (srchTimer->timeout < nextTimeout)
            {
                nextTimeout = srchTimer->timeout;
            }
            // Check next timer
            srchTimer = srchTimer->next;
        }
    }
    else
    {
        // No timers
        nextTimeout = 0;
    }
    return ( nextTimeout );
}
#endif // POWER_SAVING


/*********************************************************************
 * @fn      osal_GetSystemClock()
 *
 * @brief   Read the local system clock.
 *
 * @param   none
 *
 * @return  local clock in milliseconds
 */
uint32_t osal_GetSystemClock( void )
{
  return ( osal_systemClock );
}

/*********************************************************************
*********************************************************************/
