#include "am_hal_sysctrl.h"
#include "am_hal_systick.h"
#include "am_util_delay.h"
#include "osal.h"
#include "vfs.h"
#include "rtc.h"
#include "sleep.h"
#include "sample_app.h"
#include "sample_sleep_mode.h"

#include "cot_mac.h"

/*******************************************************************************
 * @fn          hal_sleep
 *
 * @brief       This function is called from the OSAL task loop using and
 *              existing OSAL interface.  
 *
 * input parameters
 *
 * @param       osal_timeout - Next OSAL timer timeout, in msec.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hal_sleep( uint32_t osal_timeout )
{
    int32_t rtc0_fd = open(PATH_RTC0, 0);
    
    am_hal_systick_stop();
    #ifdef RTC_AWAKE_MODE
        ioctl(rtc0_fd, RTC_SET_ALARM_TIME, 10000);// 10s // millisecond
    	ioctl(rtc0_fd, RTC_ENABLE, 0);
	#endif
	#ifdef DEEP_SLEEP_MODE
    ioctl(rtc0_fd, RTC_DISABLE, 0);
	#endif
	//am_util_delay_ms(100);
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    am_hal_systick_start();

    ioctl(rtc0_fd, RTC_DISABLE, 0);
    close(rtc0_fd);
	
	#ifdef RTC_AWAKE_MODE
	wake_up();
    osal_set_event(get_sample_task_id(), SMAPLE_MSG_WAKE_EVENT );
	#endif
} 

