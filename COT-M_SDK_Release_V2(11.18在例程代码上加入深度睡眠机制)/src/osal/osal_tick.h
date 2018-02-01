/**************************************************************************************************
  Filename:       OSAL_Tick.h
  Revised:        $Date: 2005/04/29 01:36:04 $
  Revision:       $Revision$

  Description:

  This file contains the interface to the ADC Service.
  This also contains the Task functions.

  Notes:
**************************************************************************************************/
#ifndef osal_tick_H
#define osal_tick_H
#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdint.h>
#include "osal.h"
/**************************************************************************************************
 * MACROS
 **************************************************************************************************/
#define __I volatile const /*!< defines 'read only' permissions */
#define __O volatile /*!< defines 'write only' permissions */
#define __IO volatile /*!< defines 'read / write' permissions */

/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/
void osal_systick_configuration(void);

uint32_t osal_get_systick( void );

void osal_systick_reset( void );

void osal_systick_stop(void);

uint32_t osal_get_system_tick(void);

#ifdef __cplusplus
}
#endif
#endif
