/**
* @file         iic.h
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: V0.1,wende.wu, 2017-05-24
*/

#ifndef IIC_H
#define IIC_H

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
#define PATH_IIC0     ("IIC0")    /*!< IIC 0 */

#define IIC0_SCL_PIN           31           // IIC Ê±ÖÓ
#define IIC0_SCL               AM_HAL_GPIO_OUTPUT
 
#define IIC0_SDA_PIN           32           // IIC Êý¾Ý
#define IIC0_SDA               AM_HAL_GPIO_OUTPUT





 /*********************************************************************
 * CONSTANTS
 */
    
#define IIC_SLAVE_ADDR      0           // I2C_SLAVE ADDR
#define IIC_ENABLE          1
#define IIC_DISABLE         2
#define IIC_TIMEOUT         3

//*****************************************************************************
//
//! @{
//
//*****************************************************************************


 /*********************************************************************
 * TYPEDEFS
 */
//*****************************************************************************


 /*********************************************************************
 * GLOBAL VARIABLES
 */


 /*********************************************************************
 * EXTERNAL VARIABLES
 */


 /*********************************************************************
 * PUBLIC FUNCTION
 */


int32_t iic_init_module(void);



#ifdef __cplusplus
}
#endif

#endif



