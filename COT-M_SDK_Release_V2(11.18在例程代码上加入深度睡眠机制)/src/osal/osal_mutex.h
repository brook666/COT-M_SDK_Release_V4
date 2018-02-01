/**************************************************************************************************
* Filename:             osal_mutex.h
* Revised:        
* Revision:       
* Description:    
**************************************************************************************************/
#ifndef OSAL_MUTEX_H
#define OSAL_MUTEX_H

#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************
* INCLUDES
*/
 #include <stdint.h>
//#include "stm32f10x.h"
//#include "stm32f4xx.h"
//#include "stm32l0xx.h"

typedef struct mutex_struct
{
 uint32_t mutex_value;
 struct mutex_struct *next_mutex;
}osal_mutex_t;

osal_mutex_t* osal_mutex_create(void);
void osal_mutex_delete(osal_mutex_t** mutex);
void osal_mutex_take(osal_mutex_t** mutex,uint32_t mutex_overtime);
uint32_t osal_mutex_check(osal_mutex_t* mutex);
void osal_mutex_release(osal_mutex_t** mutex);
void osal_mutex_updata(void);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 


