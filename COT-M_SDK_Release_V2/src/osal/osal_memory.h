/**************************************************************************************************
* Filename:             osal_Memory.h
* Revised:        
* Revision:       
* Description:    
**************************************************************************************************/
#ifndef osal_Memory_H
#define osal_Memory_H

#ifdef __cplusplus

extern "C"
{
  
#endif
/*********************************************************************
* INCLUDES
*/
#include <stdint.h>
	
	
/*********************************************************************
* MACROS
*/

/*********************************************************************
* CONSTANTS
*/

/*********************************************************************
* TYPEDEFS
*/
#define OSALMEM_METRICS 1

/*********************************************************************
* GLOBAL VARIABLES
*/

/*********************************************************************
* FUNCTIONS
*/
 /*
  * Allocate a block of memory.
  */
void osal_mem_init( void );
void osal_mem_kick( void );
extern void *osal_mem_alloc( uint16_t size );
void osal_mem_free( void *ptr );
uint16_t osal_heap_block_max( void );
uint16_t osal_heap_block_cnt( void );
uint16_t osal_heap_block_free( void );
uint16_t osal_heap_mem_used( void );
uint16_t osal_heap_mem_max( void );
/*********************************************************************
*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif 
