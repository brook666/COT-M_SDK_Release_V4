/**
* @file         sample_adc.c
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

/*********************************************************************
 * INCLUDE
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "osal.h"
#include "osal_time.h"
#include "osal_tasks.h"
#include "osal_pwrmgr.h"
#include "vfs.h"
#include "spi1.h"
#include "error.h"
#include "log.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "adc.h"
#include "cot_mt_adc.h"
#include "cot_mt_oled.h"
#include "sample_adc.h"


/*********************************************************************
 * MACROS
 */
#define MSG(FORMAT, ARG...) /* message that is destined to the user */
#define MSG_DEBUG(FLAG, fmt, ...)                                                                         \
            do  {                                                                                         \
                if (FLAG)                                                                                 \
                {\
                    log_printf("%s:%d:%s(): " ,__FILE__, __LINE__, __FUNCTION__);\
					log_printf(fmt,##__VA_ARGS__);\
                }\
            } while (0)

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
static uint16_t s_sample_adc_id = 0; /*<  sample application id for osal task id >*/


/*********************************************************************
 * EXTERNAL FUNCTIONS
 */



/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * PROFILE CALLBACKS
 */



/*********************************************************************
 * PUBLIC FUNCTIONS
 */


                
/**
* @name: sample_adc_init
* This funtion deal with the sample app init
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
void sample_adc_init( uint8_t taskId )
{
    s_sample_adc_id = taskId ;
    osal_set_event(s_sample_adc_id, SMAPLE_ADC_INIT_EVENT);
    osal_start_timerEx(taskId, SMAPLE_ADC_TEMP_EVENT, 2);
}


/**
 * @name: sample_adc
 * This function provide simple useing of the adc apis.
 * @param[in]   task_id -- the osal task id.
 * @param[out]  events -- the event of the task_id.
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by wende.wu on 2017-06-19
 */
uint16_t sample_adc(uint8_t task_id, uint16_t events)
{
    uint8_t *samle_msg_ptr = NULL;
    
    if ( events & SYS_EVENT_MSG )
    {
        while ((samle_msg_ptr = osal_msg_receive( task_id )) != NULL)
        {
            // Release the OSAL message
            osal_msg_deallocate((uint8_t *)samle_msg_ptr);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    
    if(events & SMAPLE_ADC_INIT_EVENT)
    {
        cot_mt_adc_init();
        osal_set_event(task_id, SMAPLE_ADC_TRIGGER_EVENT);
        // return unprocessed events
        return (events ^ SMAPLE_ADC_INIT_EVENT);
    }

    if(events & SMAPLE_ADC_TRIGGER_EVENT)
    {
        cot_mt_adc_trigger();
        osal_start_timerEx(task_id, SMAPLE_ADC_RX_EVENT, 100); // Wait here for the ISR to grab a buffer of samples.
        // return unprocessed events
        return (events ^ SMAPLE_ADC_TRIGGER_EVENT);
    }

    

    if(events & SMAPLE_ADC_RX_EVENT)
    {
        uint8_t i =0;
        uint8_t showstring[30] ={0};
        float fTempF = 0;
        //int32_t i32BaseLevelCount;
        float fVBATT;
        float fADCTempVolts;
        float fADCTempDegreesC;
        
        // ADC code for ADC_ext from ADC ISR to base level.
        uint16_t ui16ADCEXT_code[3] = {0};
        
        // ADC code for  temperature sensor from ADC ISR to base level.
        uint16_t ui16ADCTEMP_code = 0;

        cot_mt_adc_read(ui16ADCEXT_code,&ui16ADCTEMP_code);
        
        for(i =0 ; i< 3; i++)
        {
            fTempF = ui16ADCEXT_code[i];
            fVBATT = ((float)fTempF) * (float)fReferenceVoltage / 1024.0f / 64.0f;

            log_printf("\nVBATT[%d] = <%.3f>",i,fVBATT);
            log_printf(" (0x%4X) ", ui16ADCEXT_code[i]);
            sprintf((char *)showstring + 5 *i,"%.3f",fVBATT);
        }
        
        //
        // Convert and scale the temperature.
        // Temperatures are in Fahrenheit range -40 to 225 degrees.
        // Voltage range is 0.825V to 1.283V
        // First get the ADC voltage corresponding to temperature.
        //
        fTempF = ui16ADCTEMP_code;
        fADCTempVolts = ((float)fTempF) * (float)fReferenceVoltage / 1024.0f / 64.0f;

        //
        // Now call the HAL routine to convert volts to degrees Celsius.
        //
        fADCTempDegreesC = am_hal_adc_volts_to_celsius(fADCTempVolts);

        //
        // print the temperature value in Celsius.
        //
        log_printf("TEMP = %.2f C ",fADCTempDegreesC);
        log_printf("(0x%4X) ",ui16ADCTEMP_code);
        //
        // Print the temperature value in Fahrenheit.
        //
        fTempF = (fADCTempDegreesC * (9.0f / 5.0f)) + 32.0f;
        log_printf(" %.2f F", fTempF);


        sprintf((char *)showstring + 15,"%.3f",fADCTempDegreesC);
        showstring[4] = ' ';showstring[9] = ' ';showstring[14] = ' ';
        OLED_ShowStr6x8(OLED_SHOWDARK,0,0,showstring);


        osal_start_timerEx(task_id,SMAPLE_ADC_TRIGGER_EVENT, 3000); //
        return (events ^ SMAPLE_ADC_RX_EVENT);
    }


    if(events & SMAPLE_ADC_TEMP_EVENT)  // ÁÙÊ±°´¼üºÍµÆ
    {
        
        #if 1
            {
                static uint8_t init_c = 0;
                static uint8_t out_N = 0x01;
                static uint8_t out_O = 0x00;
                if(init_c == 0)
                {
                    init_c++;
                    am_hal_gpio_pin_config(0,AM_HAL_GPIO_INPUT | AM_HAL_GPIO_PULL6K); //  button
                    am_hal_gpio_pin_config(1,AM_HAL_GPIO_INPUT | AM_HAL_GPIO_PULL6K); // button
                    am_hal_gpio_pin_config(2,AM_HAL_GPIO_INPUT | AM_HAL_GPIO_PULL6K); //
                    am_hal_gpio_pin_config(11,AM_HAL_GPIO_INPUT | AM_HAL_GPIO_PULL6K); //
                    am_hal_gpio_pin_config(37,AM_HAL_GPIO_INPUT | AM_HAL_GPIO_PULL6K); //
                
                    am_hal_gpio_pin_config(23,AM_HAL_GPIO_OUTPUT); // led
                    am_hal_gpio_pin_config(29,AM_HAL_GPIO_OUTPUT); //
                    am_hal_gpio_pin_config(30,AM_HAL_GPIO_OUTPUT); //
                    
                    am_hal_gpio_out_bit_set(23);
                    am_hal_gpio_out_bit_set(29);
                    am_hal_gpio_out_bit_set(30);
                }
                //while(1)
                {
                    if(am_hal_gpio_input_bit_read(0) == 0) 
                    {am_util_delay_ms(20);if(am_hal_gpio_input_bit_read(0) == 0) out_N++; }
            
                    if(am_hal_gpio_input_bit_read(1) == 0) 
                    {am_util_delay_ms(20);if(am_hal_gpio_input_bit_read(1) == 0) out_N++; }
            
                    if(am_hal_gpio_input_bit_read(2) == 0) 
                    {am_util_delay_ms(20);if(am_hal_gpio_input_bit_read(2) == 0) out_N++; }
            
                    if(am_hal_gpio_input_bit_read(11) == 0)
                    {am_util_delay_ms(20);if(am_hal_gpio_input_bit_read(11) == 0) out_N++; }
            
                    if(am_hal_gpio_input_bit_read(37) == 0)
                    {am_util_delay_ms(20);if(am_hal_gpio_input_bit_read(37) == 0) out_N++; }
            
                    if(out_O != out_N)
                    {
                        out_O = out_N;
                        if(out_O == 0x01)
                        {
                            am_hal_gpio_out_bit_set(23);
                            am_hal_gpio_out_bit_clear(29);
                            am_hal_gpio_out_bit_clear(30);
                        }
                        else if(out_O == 0x02)
                        {
                            am_hal_gpio_out_bit_clear(23);
                            am_hal_gpio_out_bit_set(29);
                            am_hal_gpio_out_bit_clear(30);
                        }
                        else
                        {
                            out_O = 0;
                            out_N = 0;
                            am_hal_gpio_out_bit_clear(23);
                            am_hal_gpio_out_bit_clear(29);
                            am_hal_gpio_out_bit_set(30);
                        }
                        //am_util_delay_ms(200);
                        osal_start_timerEx(task_id, SMAPLE_ADC_TEMP_EVENT, 300); // 
                        return (events ^ SMAPLE_ADC_TEMP_EVENT);
                    } 
                }
            }
        #endif
        
        osal_start_timerEx(task_id, SMAPLE_ADC_TEMP_EVENT, 20); // Wait here for the ISR to grab a buffer of samples.
        // return unprocessed events
        return (events ^ SMAPLE_ADC_TEMP_EVENT);
    }
    
    return 0;
}

