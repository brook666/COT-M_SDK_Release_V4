/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: adc.c
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
#include "spi1.h"
#include "error.h"
#include "log.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "adc.h"

/*********************************************************************
 * MACROS
 */
#define ADC_SLOT_CONFIG_EXT0        0
#define ADC_SLOT_CONFIG_EXT1        1
#define ADC_SLOT_CONFIG_EXT3        2
#define ADC_SLOT_CONFIG_TEMP        3
    


/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
    
// ADC code for ADC_ext from ADC ISR to base level.
uint16_t g_ui16ADCEXT_code[3];

// ADC code for  temperature sensor from ADC ISR to base level.
uint16_t g_ui16ADCTEMP_code;

float fTemp, fVoltage, fOffset;

//*****************************************************************************
//
// ADC Configuration
//
//*****************************************************************************
am_hal_adc_config_t g_sADC_CfgA =
{
    // Select the ADC Clock source using one of the clock source macros.
    AM_HAL_ADC_CLOCK_1_5MHZ,

    // Select the ADC trigger source using a trigger source macro.
    AM_HAL_ADC_TRIGGER_SOFT,

    // Use a macro to select the ADC reference voltage.
    AM_HAL_ADC_REF_VDD,//AM_HAL_ADC_REF_EXT,

    // Use a macro to choose a maximum sample rate setting.
    AM_HAL_ADC_MODE_1MSPS,

    // Use a macro to choose the power mode for the ADC's idle state.
    AM_HAL_ADC_LPMODE_2,

    // Use the Repeat macro to enable repeating samples using Timer3A.
    AM_HAL_ADC_NO_REPEAT,//AM_HAL_ADC_REPEAT,

    // Power Off the temp sensor.
    AM_HAL_ADC_PON_TEMP,

    // Set the ADC window limits using the window limit macro.
    AM_HAL_ADC_WINDOW(768, 256) // arbitrary window setting, not used here.
};
    

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
static void adc_disable(void);
static void adc_enable(void);
static void init_config_adc(void);
static void configure_adc_slot(uint32_t slot);



//*****************************************************************************
//
// ADC Interrupt Service Routine (ISR)
//
//*****************************************************************************
void am_adc_isr(void)
{
    am_hal_adc_fifo_read_t fifo_info;

    //
    // Clear ADC Interrupt (write to clear).
    //
    AM_REGn(CTIMER, 0, INTCLR) = AM_REG_CTIMER_INTCLR_CTMRA0INT_M;

    //
    // Keep grabbing value from the ADC FIFO until it goes empty.
    //
    while (am_hal_adc_fifo_read(&fifo_info))
    {
        switch(fifo_info.ui8Slot)
        {
            case ADC_SLOT_CONFIG_EXT0:
                g_ui16ADCEXT_code[0] = fifo_info.ui16Data;
            break;
            case ADC_SLOT_CONFIG_EXT1:
                g_ui16ADCEXT_code[1] = fifo_info.ui16Data;
            break;
            case ADC_SLOT_CONFIG_EXT3:
                g_ui16ADCEXT_code[2] = fifo_info.ui16Data;
            break;
            case ADC_SLOT_CONFIG_TEMP:
                // Just grab the ADC code for the temperature sensor.
                g_ui16ADCTEMP_code = fifo_info.ui16Data;
            break;
            default:
		    break;
        }
    }

    //
    // Signal interrupt arrival to base level.
    //
    //g_ui32SampleCount++;
}


/* file interface: open */
static int32_t ADC_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_ADC0,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: release */
static int32_t ADC_Release( struct File* file )
{
    if( strncmp(file->path,PATH_ADC0,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }
	return ERR_FAILUER;
}

/* file ops: read */
static int32_t ADC_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{
    uint16_t* ui16Dest = (uint16_t *)buf;

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_ADC0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch(*pos)
    {
        case ADC_SLOT_EXT0:
            *ui16Dest = g_ui16ADCEXT_code[0];
        break;
        case ADC_SLOT_EXT1:
            *ui16Dest = g_ui16ADCEXT_code[1];
        break;
        case ADC_SLOT_EXT3:
            *ui16Dest = g_ui16ADCEXT_code[2];
        break;
        case ADC_SLOT_TEMP:
            *ui16Dest = g_ui16ADCTEMP_code;
        break;
        default:
			break;
    }

    return count;
}



static int32_t ADC_Ioctl( struct File* file, int32_t request, uint32_t args )
{

    if( NULL == file || strncmp(file->path,PATH_ADC0,4) != 0 )
        return ERR_PARAMETER ;

	switch(request)
	{
	    case SET_ADC0_TRIGGER:
            //
            // This function issues the software trigger to the ADC.
            //
            am_hal_adc_trigger();
		break;
        case SET_ADC0_ENABLE:
            adc_enable();            
		break;
        case SET_ADC0_DISABLE:
            adc_disable(); 
		break;
        case SET_ADC0_CONFIG:
            am_hal_interrupt_disable(AM_HAL_INTERRUPT_ADC); 
            configure_adc_slot(args);
            //am_hal_interrupt_enable(AM_HAL_INTERRUPT_ADC);
		break;
		default:
			break;
	}
	return 0;
}


/* file ops: lseek */
static int32_t ADC_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    if( NULL == file || strncmp(file->path,PATH_ADC0,4) != 0 )
        return ERR_PARAMETER ;

    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
        case SEEK_CUR:
            break;
        case SEEK_END:
            break;
		default:
			break;
    }
    return file->pos;
}


/* file operation */
struct FileOps adc_ops =
{
   ADC_Open,		    /* open */
   ADC_Release,	 	    /* release */
   ADC_Read,		    /* read */
   NULL,	   		    /* write */
   ADC_Lseek,			/* lseek */
   ADC_Ioctl,           /* ioctl */
   NULL,                /* irq handler regiser */
};

/* ADC file descriptor */
struct File file_adc =
{
	PATH_ADC0,  	    /* path */
    0,				    /* pos */
    &adc_ops,	   	    /* ops */
    0,                  /* private */
    0,	  			    /* count */
};

//*****************************************************************************
//
// Configure adc slot
//
//*****************************************************************************
static void configure_adc_slot(uint32_t slot)
{
    //
    // Initialize the slot control registers.
    // 
    if(ADC_SLOT_EXT0)
    {
        am_hal_adc_slot_config(ADC_SLOT_CONFIG_EXT0, (AM_HAL_ADC_SLOT_AVG_1 |
                                        AM_HAL_ADC_SLOT_CHSEL_EXT0 |
                                        AM_HAL_ADC_SLOT_WINDOW_EN |
                                        AM_HAL_ADC_SLOT_ENABLE));
    }
    if(ADC_SLOT_EXT1)
    {
        am_hal_adc_slot_config(ADC_SLOT_CONFIG_EXT1, (AM_HAL_ADC_SLOT_AVG_1 |
                                        AM_HAL_ADC_SLOT_CHSEL_EXT1 |
                                        AM_HAL_ADC_SLOT_WINDOW_EN |
                                        AM_HAL_ADC_SLOT_ENABLE));
    }
    if(ADC_SLOT_EXT3)
    {
        am_hal_adc_slot_config(ADC_SLOT_CONFIG_EXT3, (AM_HAL_ADC_SLOT_AVG_1 |
                                       AM_HAL_ADC_SLOT_CHSEL_EXT3 |
                                       AM_HAL_ADC_SLOT_WINDOW_EN |
                                       AM_HAL_ADC_SLOT_ENABLE));
    }
    if(ADC_SLOT_TEMP)
    {
        am_hal_adc_slot_config(ADC_SLOT_CONFIG_TEMP, (AM_HAL_ADC_SLOT_AVG_1 |
                                       AM_HAL_ADC_SLOT_CHSEL_TEMP |
                                       AM_HAL_ADC_SLOT_WINDOW_EN |
                                       AM_HAL_ADC_SLOT_ENABLE));
    }
    
    am_hal_adc_slot_config(4, 0); // unused slot
    am_hal_adc_slot_config(5, 0); // unused slot

    am_hal_adc_slot_config(6, 0); // unused slot
    am_hal_adc_slot_config(7, 0); // unused slot
            
}
    


/* enable the adc.*/
static void adc_enable(void)
{
    //    
    am_hal_gpio_pin_config(ADC0_EXT0_PIN_12, AM_HAL_PIN_12_ADC0);
    am_hal_gpio_pin_config(ADC0_EXT1_PIN_13, AM_HAL_PIN_13_ADC1); //
    am_hal_gpio_pin_config(ADC0_EXT3_PIN_15, AM_HAL_PIN_15_ADC3); //
    am_hal_gpio_pin_config(ADC0_ADCREF_PIN_16, AM_HAL_PIN_16_ADCREF); //
    
    //
    // We MUST turn on band gap to use the Temp Sensor.
    // The ADC hardware in mode 2 will cycle the power to the bandgap
    // automatically.
    //
    am_hal_mcuctrl_bandgap_enable();

    //
    // Configure the ADC.
    //
    am_hal_adc_config(&g_sADC_CfgA);

    //
    // Print out the temperature trim values as recorded in OTP.
    //
    //am_hal_adc_temp_trims_get(&fTemp, &fVoltage, &fOffset);
    //log_printf("TRIMMED TEMP    = %.3f\n", fTemp);
    //log_printf("TRIMMED VOLTAGE = %.3f\n", fVoltage);
    //log_printf("TRIMMED Offset  = %.3f\n", fOffset);


    //
    // Enable the ADC interrupt in the NVIC.
    //
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_ADC);

    //
    // Enable the ADC interrupts in the ADC.
    //
    am_hal_adc_int_enable(AM_HAL_ADC_INTEN_WCINC |
                          AM_HAL_ADC_INTEN_WCEXC |
                          AM_HAL_ADC_INTEN_FIFOOVR2 |
                          AM_HAL_ADC_INTEN_FIFOOVR1 |
                          AM_HAL_ADC_INTEN_SCNCMP |
                          AM_HAL_ADC_INTEN_CNVCMP);
    //
    // Enable the ADC.
    //
    am_hal_adc_enable();
}



/* Disable the adc.*/
static void adc_disable(void)
{
    //    
    am_hal_gpio_pin_config(ADC0_EXT0_PIN_12, AM_HAL_GPIO_DISABLE);
    am_hal_gpio_pin_config(ADC0_EXT1_PIN_13, AM_HAL_GPIO_DISABLE); //
    am_hal_gpio_pin_config(ADC0_EXT3_PIN_15, AM_HAL_GPIO_DISABLE); //
    am_hal_gpio_pin_config(ADC0_ADCREF_PIN_16, AM_HAL_GPIO_DISABLE); //
    
	//
    // Disable the ADC.
    //
    am_hal_adc_disable();
}



/* init adc config */
static void init_config_adc(void)
{

    /* init adc: register device file */
    register_dev(&file_adc);
    
    //
    // Initialize the adc GPIO.
    configure_adc_slot(ADC_SLOT_EXT0 | ADC_SLOT_EXT1 | ADC_SLOT_EXT3 | ADC_SLOT_TEMP);

    //
    // Enable the adc.
    //adc_enable();
}

/* init adc character driver */
int32_t adc_init_module(void)
{
    /* init hardware setting of adc */
    init_config_adc();

	return 0;
}

