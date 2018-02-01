
#ifndef HAL_TIMERS_H
#define HAL_TIMERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern uint32_t g_osal_tick ;

void timerA0_init(void);

void timerA0_start(uint32_t tick_ms);

void timerA0_stop(void);

void HalTimerTick (void);

void wait_ms(uint32_t n_ms);

void wait_us(uint32_t n_us);

#ifdef __cplusplus
}
#endif

#endif

