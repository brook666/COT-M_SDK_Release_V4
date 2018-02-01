/**
* @file         cot_mt_adc.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef COT_MT_ADC_H
#define COT_MT_ADC_H


/*********************************************************************
 * INCLUDE
 */
#include <stdint.h>
#include "adc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * CONSTANTS
 */
#define ADC_IN1     ADC_SLOT_EXT0           
#define ADC_IN2     ADC_SLOT_EXT1   
#define ADC_IN3     ADC_SLOT_EXT3    


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
* @name: cot_mt_adc_init
* This funtion deal with the sample app init
* @param[in]   none.
* @param[out]  none.
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
void cot_mt_adc_init(void);



/**
* @name: cot_mt_adc_trigger
* This funtion deal with the adc trigger
* @param[in]   none.
* @param[out]  none.
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
void cot_mt_adc_trigger(void);



/**
* @name: cot_mt_adc_read
* This funtion deal with the adc read
* @param[in]   none.
* @param[out]  none.
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
void cot_mt_adc_read(uint16_t ui16ADCEXT[],uint16_t *ui16ADCTEMP);



/**
* @name: cot_mt_adc_disable
* This funtion deal with the adc disable
* @param[in]   none.
* @param[out]  none.
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-19
*/
void cot_mt_adc_disable(void);


#ifdef __cplusplus
}
#endif


#endif

