
#include <stdint.h>

#include "am_reg_ctimer.h"
#include "am_hal_ctimer.h"
#include "am_hal_interrupt.h"
#include "am_util_delay.h"

#include "hal_timers.h"


#define TIMER0_PERIOD  33//32768

uint32_t g_osal_tick = 0 ;



//*****************************************************************************
//
// Timer configurations.
//
//*****************************************************************************
am_hal_ctimer_config_t g_sTimer0 =
{
    // Don't link timers.
    0,

    // Set up Timer0A.
    (AM_HAL_CTIMER_FN_REPEAT |
     AM_HAL_CTIMER_INT_ENABLE    |
     AM_HAL_CTIMER_XT_32_768KHZ),

    // No configuration for Timer0B.
    0,
};

//*****************************************************************************
//
// Init function for Timer A0.
//
//*****************************************************************************
void timerA0_init(void)
{
    uint32_t ui32Period;

    //
    // Set up timer A0 to count 3 MHz clocks but don't start it yet
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer0);

    //
    // Set up timerA0 for TICK_MS (33 * TICK_MS clock periods of 32.768kHz ~= 99.3% accurate time base)
    //
    ui32Period = TIMER0_PERIOD;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period, 0);

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);

	am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

	am_hal_interrupt_enable(AM_HAL_INTERRUPT_CTIMER);

	am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
}

void timerA0_start(uint32_t tick_ms )
{
    //
    // Set up timer A0 to count 3 MHz clocks but don't start it yet
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer0);

    //
    // Set up timerA0 for TICK_MS (33 * TICK_MS clock periods of 32.768kHz ~= 99.3% accurate time base)
    //
    uint32_t ui32Period = TIMER0_PERIOD*tick_ms;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period, 0);

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);

	am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

	am_hal_interrupt_enable(AM_HAL_INTERRUPT_CTIMER);

	am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
}

void timerA0_stop(void)
{
    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);

	am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA0);

	am_hal_interrupt_disable(AM_HAL_INTERRUPT_CTIMER);

	am_hal_ctimer_stop(0, AM_HAL_CTIMER_TIMERA);
}

/***************************************************************************************************
 * @fn      HalTimerTick
 *
 * @brief   Check the counter for expired counter.
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void HalTimerTick (void)
{

}



//*****************************************************************************
//
// Timer Interrupt Service Routine (ISR
//
//*****************************************************************************

extern void cot_ranging_send_next_pkg( void );

void am_ctimer_isr(void)
{
	am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
    cot_ranging_send_next_pkg();
}

void wait_ms(uint32_t n_ms)
{
	if(n_ms == 0) return ;
    am_util_delay_ms(n_ms);
}

void wait_us(uint32_t n_us)
{
	if(n_us == 0) return ;
    am_util_delay_us(n_us);
}


