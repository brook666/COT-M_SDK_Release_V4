/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: cot_mt_adc.c
*
* Author: wende.wu
*
* Version: 1.0
*
* Date: 2017-05-29
*
* Description: the functions of spi virtual file system
*
* Function List:
*
* History:
*--------------------------------------------------------------------------*/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "OSAL.h"
#include "OSAL_Memory.h"
#include "vfs.h"
#include "error.h"
#include "log.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "adc.h"
#include "cot_mt_adc.h"
#include "cot_mt_oled.h"

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
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */


/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/**
* @name: cot_mt_adc_init
* This funtion deal with the adc init
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
void cot_mt_adc_init( )
{
    int32_t adc_fd = 0;  

    adc_init_module(); 
    adc_fd = open(PATH_ADC0,0);
    if( adc_fd <= 0 )   return ;
    ioctl(adc_fd,SET_ADC0_CONFIG,ADC_IN1 | ADC_IN2 | ADC_IN3 | ADC_SLOT_TEMP);
    ioctl(adc_fd,SET_ADC0_ENABLE,TRUE);

    close(adc_fd);
}



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
void cot_mt_adc_trigger( )
{
    int32_t adc_fd = 0;  
    
    adc_fd = open(PATH_ADC0,0);
    if( adc_fd <= 0 )   return ;

    ioctl(adc_fd,SET_ADC0_TRIGGER,TRUE);

    close(adc_fd);
}


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
void cot_mt_adc_read(uint16_t ui16ADCEXT[],uint16_t *ui16ADCTEMP)
{
    int32_t adc_fd = 0;  
        
    // ADC code for ADC_ext from ADC ISR to base level.
    uint16_t ui16ADCEXT_code[3] = {0};
    
    // ADC code for  temperature sensor from ADC ISR to base level.
    uint16_t ui16ADCTEMP_code = 0;

    
    adc_fd = open(PATH_ADC0,0);
    if( adc_fd <= 0 )   return ;

     // Wait here for the ISR to grab a buffer of samples.
    lseek(adc_fd,ADC_IN1,SEEK_SET);
    read(adc_fd,&(ui16ADCEXT_code[0]),1);

    lseek(adc_fd,ADC_IN2,SEEK_SET);
    read(adc_fd,&(ui16ADCEXT_code[1]),1);

    lseek(adc_fd,ADC_IN3,SEEK_SET);
    read(adc_fd,&(ui16ADCEXT_code[2]),1);

    lseek(adc_fd,ADC_SLOT_TEMP,SEEK_SET);
    read(adc_fd,&ui16ADCTEMP_code,1);

    ui16ADCEXT[0] = ui16ADCEXT_code[0];
    ui16ADCEXT[1] = ui16ADCEXT_code[1];
    ui16ADCEXT[2] = ui16ADCEXT_code[2];
    *ui16ADCTEMP = ui16ADCTEMP_code;
    
    close(adc_fd);
}



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
void cot_mt_adc_disable( )
{
    int32_t adc_fd = 0;  
    
    adc_fd = open(PATH_ADC0,0);
    if( adc_fd <= 0 )   return ;

    ioctl(adc_fd,SET_ADC0_DISABLE,TRUE);

    close(adc_fd);
}


