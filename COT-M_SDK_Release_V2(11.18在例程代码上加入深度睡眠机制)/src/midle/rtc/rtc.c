/**
* @file         rtc.c
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: V0.1,zhangjh, 2017-05-24
*/

/*********************************************************************
 * INCLUDE
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "am_reg_clkgen.h"
#include "am_hal_clkgen.h"
#include "am_reg_rtc.h"
#include "am_hal_rtc.h"
#include "am_hal_interrupt.h"

#include "error.h"
#include "vfs.h"
#include "rtc.h"

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
 * EXTERNAL VARIABLES
 */


 /*********************************************************************
 * LOCAL VARIABLES
 */

static uint32_t s_alram_time ;

 /*********************************************************************
 * LOCAL FUNCTIONS
 */
static void rtc0_init(void);
static void rtc0_enable(void);
static void rtc0_disable(void);
static void rtc0_config(void);

/* file interface: open */
static int32_t RTC0_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_RTC0,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

    return ERR_FAILUER;
}

/* file ops: release */
static int32_t RTC0_Release( struct File* file )
{
    if( strncmp(file->path,PATH_RTC0,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

    return ERR_FAILUER;

}

/* file ops: read */
static int32_t RTC0_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_RTC0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    am_hal_rtc_time_t *rtc_time = (am_hal_rtc_time_t*)buf;

    am_hal_rtc_time_get(rtc_time);

    return count;

}


/* file ops: write */
static int32_t RTC0_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_RTC0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    am_hal_rtc_time_t *rtc_time = (am_hal_rtc_time_t*)buf;

    am_hal_rtc_time_set(rtc_time);

    return count;

}


static int32_t RTC0_Ioctl( struct File* file, int32_t request, uint32_t args )
{

    if( NULL == file || strncmp(file->path,PATH_RTC0,4) != 0 )
        return ERR_PARAMETER;

    switch(request)
    {
        case RTC_SET_ALARM_TIME:
            s_alram_time = args ;//s_alram_time = args & 0xff;
            break;
        case RTC_ENABLE:
            rtc0_enable();
            break;
        case RTC_DISABLE:
            rtc0_disable();
            break;
        default:
            break;
    }

    return 0;
}


/* file ops: lseek */
static int32_t RTC0_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    if( NULL == file || strncmp(file->path,PATH_RTC0,4) != 0 )
        return ERR_PARAMETER ;

    return file->pos;
}

/* file operation */
struct FileOps rtc0_ops =
{
   RTC0_Open,            /* open */
   RTC0_Release,         /* release */
   RTC0_Read,            /* read */
   RTC0_Write,           /* write */
   RTC0_Lseek,           /* lseek */
   RTC0_Ioctl,           /* ioctl */
   NULL,                /* irq handler regiser */
};

/* usart1 file descriptor */
struct File file_rtc0 =
{
    PATH_RTC0,              /* path */
    0,                  /* pos */
    &rtc0_ops,           /* ops */
    0,                  /* private */
    0,                  /* count */
};

void rtc0_disable()
{
	//am_hal_clkgen_osc_stop(AM_HAL_CLKGEN_OSC_XT);
	am_hal_rtc_int_disable(AM_HAL_RTC_INT_ALM);
	am_hal_interrupt_disable(AM_HAL_INTERRUPT_CLKGEN);
	//am_hal_rtc_osc_disable();
	//am_hal_interrupt_master_disable();
}


static void rtc_alram_time(am_hal_rtc_time_t *pTime,uint32_t u32millisecond)
{
    pTime->ui32Hundredths += u32millisecond / 10 + 1;
    if(pTime->ui32Hundredths >= 100)
    {
        pTime->ui32Second += pTime->ui32Hundredths / 100;
        pTime->ui32Hundredths %= 100;
        if(pTime->ui32Second >= 60)
        {
            pTime->ui32Minute += pTime->ui32Second / 60;
            pTime->ui32Second %= 60;
            if(pTime->ui32Minute >= 60)
            {
                pTime->ui32Hour += pTime->ui32Minute /60;
                pTime->ui32Minute %= 60;
                if(pTime->ui32Hour >= 24)
                {
                    pTime->ui32Weekday += pTime->ui32Hour /24;
                    pTime->ui32Hour %= 24;
                    if(pTime->ui32Weekday > 6)
                    {
                        pTime->ui32Weekday /= 7;
                    }
                }
            }
        }
    }

}


void rtc0_enable()
{
    //
	// Enable the XTC for the RTC.
	//
	//am_hal_clkgen_osc_start(AM_HAL_CLKGEN_OSC_XT);
	//am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);

	//
	// Enable the RTC.
	//
	//am_hal_rtc_osc_enable();

	//
    // Set the alarm repeat interval to be every second.
    //
    //am_hal_rtc_alarm_interval_set(s_alram_time);

    am_hal_rtc_time_t pTime;
    #if 0
    {
        pTime.ui32Hundredths = 10;
        pTime.ui32Second = 50;
        pTime.ui32Minute = 59;
        pTime.ui32Hour = 23;
        pTime.ui32Weekday = 6;
        pTime.ui32DayOfMonth = 1;
        pTime.ui32Month = 1;
        am_hal_rtc_time_set(&pTime);
    }
    {
        pTime.ui32Hundredths = 10;
        pTime.ui32Second = 0;
        pTime.ui32Minute = 0;
        pTime.ui32Hour = 0;
        pTime.ui32Weekday = 2;
        pTime.ui32DayOfMonth = 5;
        pTime.ui32Month = 8;
    }
    #else
        am_hal_rtc_time_get(&pTime);
        rtc_alram_time(&pTime,s_alram_time);
    #endif
    am_hal_rtc_alarm_set(&pTime,AM_HAL_RTC_ALM_RPT_WK);

    
    //
    // Clear the RTC alarm interrupt.
    //
    am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

    //
    // Enable the RTC alarm interrupt.
    //
    am_hal_rtc_int_enable(AM_HAL_RTC_INT_ALM);

	//
    // Enable GPIO interrupts to the NVIC.
    //
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_CLKGEN);

	//
    // Enable interrupts to the core.
    //
    am_hal_interrupt_master_enable();

}

void rtc0_init()
{
    //
	// Enable the XTC for the RTC.
	//
	am_hal_clkgen_osc_start(AM_HAL_CLKGEN_OSC_XT);
	am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);

	//
	// Enable the RTC.
	//
	am_hal_rtc_osc_enable();

	//
    // Set the alarm repeat interval to be every second.
    //
    //am_hal_rtc_alarm_interval_set(AM_HAL_RTC_ALM_RPT_SEC);

    //
    // Clear the RTC alarm interrupt.
    //
    //am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

    //
    // Enable the RTC alarm interrupt.
    //
    //am_hal_rtc_int_enable(AM_HAL_RTC_INT_ALM);
	//
    // Enable GPIO interrupts to the NVIC.
    //
    //am_hal_interrupt_enable(AM_HAL_INTERRUPT_CLKGEN);
    //
    // Enable interrupts to the core.
    //
    //am_hal_interrupt_master_enable();

}

static void rtc0_config(void)
{
    /* init spi: register device file */
    register_dev(&file_rtc0);

    // Initialize and Enable the rtc.
    rtc0_init();

}

/* init rtc0 character driver */
int32_t rtc_init_module(void)
{
    rtc0_config();
    return 0;
}


void am_clkgen_isr(void)
{
    //
    // Clear the RTC alarm interrupt.
    //
    am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);
}


