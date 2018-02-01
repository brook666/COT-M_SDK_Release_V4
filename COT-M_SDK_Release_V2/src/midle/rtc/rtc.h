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

#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */


/*********************************************************************
 * MACROS
 */
 /* visual file list */
#define PATH_RTC0     ("RTC0")    /*!< RTC 0 */

 /*********************************************************************
 * CONSTANTS
 */
#define RTC_SET_ALARM_TIME  0
#define RTC_ENABLE          1
#define RTC_DISABLE         2


//*****************************************************************************
//
//! @name RTC Alarm Repeat Interval.
//! @brief Macro definitions for the RTC alarm repeat interval.
//!
//! These macros correspond to the RPT bits in the RTCCTL register.
//! They may be used with the \e am_hal_rtc_alarm_interval_set() and
//! \e am_hal_rtc_alarm_set() functions.
//!
//! @{
//
//*****************************************************************************
#define RTC_ALM_RPT_DIS              0x0
#define RTC_ALM_RPT_YR               0x1
#define RTC_ALM_RPT_MTH              0x2
#define RTC_ALM_RPT_WK               0x3
#define RTC_ALM_RPT_DAY              0x4
#define RTC_ALM_RPT_HR               0x5
#define RTC_ALM_RPT_MIN              0x6
#define RTC_ALM_RPT_SEC              0x7
#define RTC_ALM_RPT_10TH             0x8
#define RTC_ALM_RPT_EVERY_100TH      0x9

 /*********************************************************************
 * TYPEDEFS
 */
//*****************************************************************************
//
//! @brief The basic time structure used by the HAL for RTC interaction.
//!
//! All values are positive whole numbers. The HAL routines convert back and
//! forth to BCD.
//
//*****************************************************************************
typedef struct rtc_time_struct
{
    uint32_t ui32ReadError;
    uint32_t ui32CenturyEnable;
    uint32_t ui32Weekday;
    uint32_t ui32Century;
    uint32_t ui32Year;
    uint32_t ui32Month;
    uint32_t ui32DayOfMonth;
    uint32_t ui32Hour;
    uint32_t ui32Minute;
    uint32_t ui32Second;
    uint32_t ui32Hundredths;
}rtc_time_t;

 /*********************************************************************
 * GLOBAL VARIABLES
 */


 /*********************************************************************
 * EXTERNAL VARIABLES
 */


 /*********************************************************************
 * PUBLIC FUNCTION
 */
int32_t rtc_init_module(void);


#ifdef __cplusplus
}
#endif

#endif
