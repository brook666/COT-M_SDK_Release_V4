#include <stdio.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "osal.h"
#include "osal_pwrmgr.h"
#include "osal_tick.h"
#include "vfs.h"
#include "spi.h"
#include "gpio.h"
#include "flash.h"
#include "uart.h"
#include "rtc.h"
#include "log.h"
#include "RfParamConfig.h"
#include "cot_mt_led.h"
#include "cot_mt_oled.h"
#include "spi1.h"
#include "sample_sleep_mode.h"

int main( void )
{
   	//
    // Set the clock frequency.
    //
    am_hal_clkgen_sysclk_select(AM_HAL_CLKGEN_SYSCLK_MAX);

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

	spi_init_module();

	gpio_init_module();

    FLASH_InitModule();

	#ifdef GENERAL_MODE
	uart_init_module();

    spi1_init_module();
         
    cot_mt_oled_init();
	
	rtc_init_module();
	#endif
	
	#ifdef RTC_AWAKE_MODE
	rtc_init_module();
	#endif
	
	log_init_module();
	//
    // Enable interrupts to the core.
    //
	am_hal_interrupt_master_enable();
    
	osal_init_system();
	
	osal_pwrmgr_device( PWRMGR_ALWAYS_ON );

	osal_start_system();

	while(1);

}
