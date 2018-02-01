/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: iic.c
*
* Author: wende.wu
*
* Version: 1.0
*
* Date: 2017-03-20
*
* Description: the functions of spi virtual file system
*
* Function List:
*
* History:
*--------------------------------------------------------------------------*/

/*********************************************************************
 * INCLUDE
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "am_hal_gpio.h"

#include "error.h"
#include "vfs.h"
#include "iic.h"

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"


/*********************************************************************
 * MACROS
 */

 
#define IIC_WRITE           0x00        // 地址最低位为 0
#define IIC_READ            0x01        // 地址最低位为 1
 
#define IIC0_SCL_HIGH       am_hal_gpio_out_bit_set(IIC0_SCL_PIN)
#define IIC0_SCL_LOW        am_hal_gpio_out_bit_clear(IIC0_SCL_PIN)
 
#define IIC0_SDA_HIGH       am_hal_gpio_out_bit_set(IIC0_SDA_PIN)
#define IIC0_SDA_LOW        am_hal_gpio_out_bit_clear(IIC0_SDA_PIN)
 

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
 * LOCAL VARIABLES
 */


 /*********************************************************************
 * LOCAL FUNCTIONS
 */
static void iic0_init(void);
static void iic0_disable(void);
static void iic0_config(void);



/** 
* @name: IIC_Start 
* This function starts the IIC transfer.
* @param[in]   none
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-11
*/
static void IIC_Start()
{
    IIC0_SCL_HIGH; // SCL = high;		
    IIC0_SDA_HIGH; //  SDA = high;
    am_util_delay_us(5);//
    IIC0_SDA_LOW; //  SDA = low;
    am_util_delay_us(5);
    IIC0_SCL_LOW; //  SCL = low;
}



/** 
* @name: IIC_Stop 
* This function Stop the IIC transfer.
* @param[in]   none
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-11
*/
static void IIC_Stop()
{
    IIC0_SCL_LOW; //  SCL = low;
    IIC0_SDA_LOW; //  SDA = low;    
    am_util_delay_us(1);
    IIC0_SCL_HIGH; //  SCL = high;
    am_util_delay_us(5);
    IIC0_SDA_HIGH; //  SDA = high;
    am_util_delay_us(10);// 
}


/** 
* @name: IIC_Write_Byte 
* This function IIC bus writes a byte.
* @param[in]   IIC_Byte  writes a byte
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-11
*/
static void IIC_Write_Byte(uint8_t IIC_Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
			IIC0_SDA_HIGH; //SDA=high;
		else
			IIC0_SDA_LOW; //SDA=low;
	    am_util_delay_us(1);
		IIC0_SCL_HIGH; //SCL=high;
		am_util_delay_us(1);
		IIC0_SCL_LOW; //SCL=low;
		IIC_Byte<<=1;
	}
	IIC0_SDA_HIGH; //SDA=1;
	am_util_delay_us(1);
	IIC0_SCL_HIGH; //SCL=1;
	am_util_delay_us(1);
	IIC0_SCL_LOW; //SCL=0;
}


/* file interface: open */
static int32_t IIC0_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_IIC0,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }
    return ERR_FAILUER;
}

/* file ops: release */
static int32_t IIC0_Release( struct File* file )
{
    if( strncmp(file->path,PATH_IIC0,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }
    return ERR_FAILUER;
}


/* file ops: read */
static int32_t IIC0_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{
    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_IIC0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }
    return count;
}


/* file ops: write */
static int32_t IIC0_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{
    uint32_t i;
    uint8_t *data = buf;
    
    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_IIC0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }
    IIC_Start();
    IIC_Write_Byte((uint8_t)*pos & 0xfe);   //Slave address,SA0=0   // IIC写 地址最低位为 0
    for(i =0; i < count;i++)  // write command + data 
    {
        IIC_Write_Byte((uint8_t)data[i]); 
    }
    IIC_Stop();
    
    return count;

}


static int32_t IIC0_Ioctl( struct File* file, int32_t request, uint32_t args )
{

    if( NULL == file || strncmp(file->path,PATH_IIC0,4) != 0 )
        return ERR_PARAMETER;

    switch(request)
    {
        case IIC_SLAVE_ADDR :    // I2C_SLAVE ADDR
            file->pos = args << 1;
        break;
        case IIC_ENABLE:
            iic0_init();
            break;
        case IIC_DISABLE:
            iic0_disable();
            break;
        case IIC_TIMEOUT:
            ;
            break;            
        default:
            break;
    }

    return 0;
}



/* file operation */
struct FileOps iic0_ops =
{
   IIC0_Open,           /* open */
   IIC0_Release,        /* release */
   IIC0_Read,           /* read */
   IIC0_Write,          /* write */
   NULL,                /* lseek */
   IIC0_Ioctl,          /* ioctl */
   NULL,                /* irq handler regiser */
};

/* usart1 file descriptor */
struct File file_iic0 =
{
    PATH_IIC0,          /* path */
    0,                  /* pos */
    &iic0_ops,          /* ops */
    0,                  /* private */
    0,                  /* count */
};


/** 
* @name: iic0_disable 
* This function disables the IIC bus.
* @param[in]   none
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-11
*/
void iic0_disable()
{
    //
    // disable iic0 pin
    //
    am_hal_gpio_out_bit_set(IIC0_SCL_PIN);
    am_hal_gpio_out_bit_set(IIC0_SDA_PIN);
    am_hal_gpio_pin_config(IIC0_SCL_PIN,AM_HAL_GPIO_DISABLE); // IIC 时钟
	am_hal_gpio_pin_config(IIC0_SDA_PIN,AM_HAL_GPIO_DISABLE); // IIC 数据
}


/** 
* @name: iic0_init 
* This function enables the IIC bus.
* @param[in]   none
* @param[out]  none 
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-11
*/
void iic0_init()
{
    //
    // init iic0 pin
    //
    am_hal_gpio_pin_config(IIC0_SCL_PIN,IIC0_SCL); // IIC 时钟
	am_hal_gpio_pin_config(IIC0_SDA_PIN,IIC0_SDA);// IIC 数据
    am_hal_gpio_out_bit_set(IIC0_SCL_PIN);
    am_hal_gpio_out_bit_set(IIC0_SDA_PIN);
      
}

static void iic0_config(void)
{
    /* init iic: register device file */
    register_dev(&file_iic0);
    // Initialize and Enable the iic.
    iic0_init();
}

/* init iic0 character driver */
int32_t iic_init_module(void)
{
    iic0_config();
    return 0;
}

