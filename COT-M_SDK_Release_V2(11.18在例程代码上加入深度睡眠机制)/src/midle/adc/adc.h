/**
* @file         adc.h
* @brief        This header describes the functions that handle the serial port.
* @details      This is the detail description.
* @author       wende.wu
* @date         2017-06-09
* @version      V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 *                                               INCLUDES
 ***************************************************************************************************/


/***************************************************************************************************
 * CONST
 ***************************************************************************************************/

/* visual file list */
#define PATH_ADC0                   ("ADC0")    /*!< adc */

#define ADC0_EXT0_PIN_12            12
#define ADC0_EXT1_PIN_13            13
#define ADC0_EXT3_PIN_15            15
#define ADC0_ADCREF_PIN_16          16


#define ADC_SLOT_EXT0               (0x00000001)        
#define ADC_SLOT_EXT1               (0x00000001 << 1)
#define ADC_SLOT_EXT3               (0x00000001 << 2)
#define ADC_SLOT_TEMP               (0x00000001 << 8)


#define fReferenceVoltage           (3.3)   // ²Î¿¼µçÑ¹

#define SET_ADC0_ENABLE             (0x00000001)
#define SET_ADC0_DISABLE            (0x00000002) 
#define SET_ADC0_TRIGGER            (0x00000004)
#define SET_ADC0_CONFIG             (0x00000008)



/***************************************************************************************************
 * EXTERNALS
 ***************************************************************************************************/

/***************************************************************************************************
 * FUNCTIONS
 ***************************************************************************************************/


int32_t adc_init_module(void);


#ifdef __cplusplus
}
#endif

#endif  /* COT_MT_ADC_H */ 
