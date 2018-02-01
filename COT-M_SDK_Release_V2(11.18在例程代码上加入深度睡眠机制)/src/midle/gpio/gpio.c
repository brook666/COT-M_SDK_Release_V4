/*********************************************************************
    Filename:       gpio.cpp
    Revised:        $Date: 2017-03-25 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the gpio virtual file system Management API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/


/*********************************************************************
 * INCLUDES
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "am_reg_base_addresses.h"
#include "am_reg_macros.h"
#include "am_reg_gpio.h"
#include "am_hal_gpio.h"
#include "am_hal_interrupt.h"

#include "error.h"
#include "vfs.h"
#include "gpio.h"



/*********************************************************************
 * MACROS
 */



/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */
static void init_config_sx1280_gpio(void);


/*********************************************************************
*********************************************************************/

/* file interface: open */
static int32_t SX1280_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_SX1280,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t SX1280_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_SX1280,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: read */
static int32_t SX1280_GPIO_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
    uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_SX1280,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case SX1280_RESET_PIN:
            *data = am_hal_gpio_input_bit_read(SX1280_RESET_PIN);
            break;
        case SX1280_BUSY_PIN:
            *data = am_hal_gpio_input_bit_read(SX1280_BUSY_PIN);
			break;
		case SX1280_DIO1_PIN:
			*data = am_hal_gpio_input_bit_read(SX1280_DIO1_PIN);
            break;
        default:
            break;
    }

	return 1;
}

/* file ops: write */
static int32_t SX1280_GPIO_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
    uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */


    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_SX1280,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case SX1280_RESET_PIN:
            if( SX1280_DEVICES_ON_NOT_RESET == *data )
            {
                am_hal_gpio_out_bit_set(SX1280_RESET_PIN);
            }
            else
            {
                am_hal_gpio_out_bit_clear(SX1280_RESET_PIN);
            }
            break;

        default:
            break;
    }

	return 1;

}

static int32_t SX1280_GPIO_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
        case SEEK_CUR:
            //file->pos += offset;
            break;
        case SEEK_END:
            //file->pos = SIZE + offset;
            break;
    }

    return file->pos;
}


/* file ops: ioctl */
static int32_t SX1280_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
    if( request & REQ_SET_SX1280_DIO1_INT )
    {
        //
        // Configure the GPIO/button interrupt polarity.
        //
        am_hal_gpio_int_polarity_bit_set(SX1280_DIO1_PIN, AM_HAL_GPIO_RISING);

        //
        // Clear the GPIO Interrupt (write to clear).
        //
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));

        //
        // Enable the GPIO/button interrupt.
        //
        am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));

        //
        // Enable GPIO interrupts to the NVIC.
        //
        am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);

    }

    if( request & SX1280_PIN_DISABLE)
    {
        //am_hal_gpio_pin_config(SX1280_RESET_PIN, AM_HAL_GPIO_DISABLE);
        //
        // Clear the GPIO Interrupt (write to clear).
        //
        //am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));

        //
        // Enable the GPIO/button interrupt.
        //
        //am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));

        am_hal_gpio_pin_config(SX1280_BUSY_PIN,AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pin_config(SX1280_DIO1_PIN,AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pin_config(SX1280_DIO3_PIN,AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pin_config(SX1280_DIO2_PIN,AM_HAL_GPIO_DISABLE);
    }

    if( request & SX1280_PIN_ENABLE)
    {
        init_config_sx1280_gpio( );
    }
    if( request & SX1280_DIO1_DISABLE )
	{
		//am_hal_gpio_pin_config(SX1280_DIO1_PIN,AM_HAL_GPIO_DISABLE);
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));
	}

	return ERR_SUCCESS;
}

static int32_t SX1280_GPIO_IRQ_Register(  struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback  )
{
    int32_t ret = ERR_UNDEFINE;

    if( NULL == file     ||
        NULL == callback ||
        strncmp(file->path,PATH_SX1280,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    if( SX1280_DIO1_PIN == args )
    {
        am_hal_gpio_int_register(SX1280_DIO1_PIN, callback);
        ret = ERR_SUCCESS ;
    }

    return ret ;
}

/* file operation */
struct FileOps sx1280_gpio_ops =
{
    SX1280_GPIO_Open,		    /* open */
    SX1280_GPIO_Release,	 	/* release */
    SX1280_GPIO_Read,		    /* read */
    SX1280_GPIO_Write,	   		/* write */
    SX1280_GPIO_Lseek,			/* lseek */
    SX1280_GPIO_Ioctl,			/* ioctl */
    SX1280_GPIO_IRQ_Register,   /* irq_handler_register */
};

/* usart1 file descriptor */
struct File file_sx1280 =
{
	PATH_SX1280,  	    /* path */
    0,				    /* pos */
    &sx1280_gpio_ops,	/* ops */
    0,                  /* private */
    0,	  			    /* count */
};

/* init sx1280 config */
static void init_config_sx1280_gpio(void)
{

    /* init usart1: register device file */
    register_dev( &file_sx1280 );

    //am_hal_gpio_pin_config(SX1280_RESET_PIN, AM_HAL_GPIO_DISABLE);

    am_hal_gpio_pin_config(SX1280_RESET_PIN, AM_HAL_GPIO_OUTPUT);
	am_hal_gpio_out_bit_set(SX1280_RESET_PIN);

    am_hal_gpio_pin_config(SX1280_BUSY_PIN, AM_HAL_GPIO_INPUT);
    am_hal_gpio_pin_config(SX1280_DIO1_PIN,AM_HAL_GPIO_INPUT);
    am_hal_gpio_pin_config(SX1280_DIO2_PIN, AM_HAL_GPIO_DISABLE);
    am_hal_gpio_pin_config(SX1280_DIO3_PIN,AM_HAL_GPIO_DISABLE);
    //am_hal_gpio_pin_config(SX1280_DIO2_PIN, AM_HAL_GPIO_INPUT);
    //am_hal_gpio_pin_config(SX1280_DIO3_PIN,AM_HAL_GPIO_INPUT);

    //
    // Initialize sx1280 dio1 pin interrupt .
    //
    // Configure the GPIO/button interrupt polarity.
    //
    am_hal_gpio_int_polarity_bit_set(SX1280_DIO1_PIN, AM_HAL_GPIO_RISING);

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));
    //
    // Enable the GPIO/button interrupt.
    //
    am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));

	#if 0
    //
    // Clear the GPIO Interrupt (write to clear).
    //
    am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO2_PIN));
    //
    // Enable the GPIO/button interrupt.
    //
    am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(SX1280_DIO2_PIN));

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO3_PIN));
    //
    // Enable the GPIO/button interrupt.
    //
    am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(SX1280_DIO3_PIN));
	#endif
    //
    // Enable GPIO interrupts to the NVIC.
    //
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);


}



/**
** key
**
*/
/* file interface: open */
static int32_t KEY1_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_KEY1,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t KEY1_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_KEY1,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}
/* file ops: read */
static int32_t KEY1_GPIO_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */


    if( NULL == file ||
        NULL == buf  ||
        NULL == pos  ||
        strncmp(file->path,PATH_KEY1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case GPIO_KEY1_PIN:
           *data = am_hal_gpio_input_bit_read(GPIO_KEY1_PIN);
        default:
            break;
    }

	return ERR_SUCCESS;
}

static int32_t KEY1_GPIO_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
		default:
			break;
    }

    return file->pos;
}

/* file ops: ioctl */
static int32_t KEY1_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_KEY1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_KEY_INT_ENABLE)
	{
		am_hal_gpio_int_polarity_bit_set(GPIO_KEY1_PIN, AM_HAL_GPIO_FALLING);

		am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY1_PIN));
		//
		// Enable the GPIO/button interrupt.
		//
		am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(GPIO_KEY1_PIN));

		am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);
	}
	else if( request & REQ_SET_GPIO_KEY_INT_DISABLE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY1_PIN));
	}
	else if( request & REQ_SET_GPIO_KEY_RELEASE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY1_PIN));
		am_hal_gpio_pin_config(GPIO_KEY1_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_KEY_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_KEY1_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
	}
	
	return ERR_SUCCESS;
}

static int32_t KEY1_GPIO_IRQ_Register(  struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback  )
{
    int32_t ret = ERR_UNDEFINE;

    if( NULL == file     ||
        NULL == callback ||
        strncmp(file->path,PATH_KEY1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    if( GPIO_KEY1_PIN  == args )
    {
        am_hal_gpio_int_register(GPIO_KEY1_PIN, callback);
        ret = ERR_SUCCESS ;
    }

    return ret ;
}

/* file operation */
struct FileOps key1_gpio_ops =
{
    KEY1_GPIO_Open,		        /* open */
    KEY1_GPIO_Release,	 	    /* release */
    KEY1_GPIO_Read,		        /* read */
    NULL,	   		            /* write */
    KEY1_GPIO_Lseek,			/* lseek */
    KEY1_GPIO_Ioctl,			/* ioctl */
    KEY1_GPIO_IRQ_Register,     /* irq_handler_register */
};

/* key1 file descriptor */
struct File file_key1 =
{
	PATH_KEY1,  	    /* path */
    0,				    /* pos */
    &key1_gpio_ops,	    /* ops */
    0,                  /* private */
    0,	  			    /* count */
};

static void init_config_key1_gpio(void)
{
	register_dev( &file_key1 );
	am_hal_gpio_pin_config(GPIO_KEY1_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
}


//KEY2
/* file interface: open */
static int32_t KEY2_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_KEY2,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t KEY2_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_KEY2,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: read */
static int32_t KEY2_GPIO_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */


    if( NULL == file ||
        NULL == buf  ||
        NULL == pos  ||
        strncmp(file->path,PATH_KEY2,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case GPIO_KEY2_PIN:
           *data = am_hal_gpio_input_bit_read(GPIO_KEY2_PIN);
        default:
            break;
    }

	return ERR_SUCCESS;
}

static int32_t KEY2_GPIO_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
		default:
			break;
    }

    return file->pos;
}

/* file ops: ioctl */
static int32_t KEY2_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_KEY2,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_KEY_INT_ENABLE)
	{
		am_hal_gpio_int_polarity_bit_set(GPIO_KEY2_PIN, AM_HAL_GPIO_FALLING);

		am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY2_PIN));
		//
		// Enable the GPIO/button interrupt.
		//
		am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(GPIO_KEY2_PIN));

		am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);
	}
	else if( request & REQ_SET_GPIO_KEY_INT_DISABLE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY2_PIN));
	}
	else if( request & REQ_SET_GPIO_KEY_RELEASE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY2_PIN));
		am_hal_gpio_pin_config(GPIO_KEY2_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_KEY_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_KEY2_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
	}
	
	return ERR_SUCCESS;
}

static int32_t KEY2_GPIO_IRQ_Register(  struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback  )
{
    int32_t ret = ERR_UNDEFINE;

    if( NULL == file     ||
        NULL == callback ||
        strncmp(file->path,PATH_KEY2,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    if( GPIO_KEY2_PIN  == args )
    {
        am_hal_gpio_int_register(GPIO_KEY2_PIN, callback);
        ret = ERR_SUCCESS ;
    }

    return ret ;
}

/* file operation */
struct FileOps key2_gpio_ops =
{
    KEY2_GPIO_Open,		    /* open */
    KEY2_GPIO_Release,	 	/* release */
    KEY2_GPIO_Read,		    /* read */
    NULL,	   		/* write */
    KEY2_GPIO_Lseek,			/* lseek */
    KEY2_GPIO_Ioctl,			/* ioctl */
    KEY2_GPIO_IRQ_Register,   /* irq_handler_register */
};

/* key2 file descriptor */
struct File file_key2 =
{
	PATH_KEY2,  	    /* path */
    0,				    /* pos */
    &key2_gpio_ops,	/* ops */
    0,                  /* private */
    0,	  			    /* count */
};
static void init_config_key2_gpio(void)
{
	register_dev( &file_key2 );
	am_hal_gpio_pin_config(GPIO_KEY2_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
}

//key3
/* file interface: open */
static int32_t KEY3_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_KEY3,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t KEY3_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_KEY3,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: read */
static int32_t KEY3_GPIO_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */


    if( NULL == file ||
        NULL == buf  ||
        NULL == pos  ||
        strncmp(file->path,PATH_KEY3,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case GPIO_KEY3_PIN:
           *data = am_hal_gpio_input_bit_read(GPIO_KEY3_PIN);
        default:
            break;
    }

	return ERR_SUCCESS;
}
static int32_t KEY3_GPIO_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
		default:
			break;
    }

    return file->pos;
}
/* file ops: ioctl */
static int32_t KEY3_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_KEY3,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_KEY_INT_ENABLE)
	{
		am_hal_gpio_int_polarity_bit_set(GPIO_KEY3_PIN, AM_HAL_GPIO_FALLING);

		am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY3_PIN));
		//
		// Enable the GPIO/button interrupt.
		//
		am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(GPIO_KEY3_PIN));

		am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);
	}
	else if( request & REQ_SET_GPIO_KEY_INT_DISABLE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY3_PIN));
	}
	else if( request & REQ_SET_GPIO_KEY_RELEASE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY3_PIN));
		am_hal_gpio_pin_config(GPIO_KEY3_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_KEY_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_KEY3_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
	}
	
	return ERR_SUCCESS;
}

static int32_t KEY3_GPIO_IRQ_Register(  struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback  )
{
    int32_t ret = ERR_UNDEFINE;

    if( NULL == file     ||
        NULL == callback ||
        strncmp(file->path,PATH_KEY3,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    if( GPIO_KEY3_PIN  == args )
    {
        am_hal_gpio_int_register(GPIO_KEY3_PIN, callback);
        ret = ERR_SUCCESS ;
    }

    return ret ;
}

/* file operation */
struct FileOps key3_gpio_ops =
{
    KEY3_GPIO_Open,		    /* open */
    KEY3_GPIO_Release,	 	/* release */
    KEY3_GPIO_Read,		    /* read */
    NULL,	   		/* write */
    KEY3_GPIO_Lseek,			/* lseek */
    KEY3_GPIO_Ioctl,			/* ioctl */
    KEY3_GPIO_IRQ_Register,   /* irq_handler_register */
};

/* key3 file descriptor */
struct File file_key3 =
{
	PATH_KEY3,  	    /* path */
    0,				    /* pos */
    &key3_gpio_ops,	/* ops */
    0,                  /* private */
    0,	  			    /* count */
};

static void init_config_key3_gpio(void)
{
	register_dev( &file_key3 );
	am_hal_gpio_pin_config(GPIO_KEY3_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
}

//key4
/* file interface: open */
static int32_t KEY4_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_KEY4,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t KEY4_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_KEY4,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}
/* file ops: read */
static int32_t KEY4_GPIO_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */


    if( NULL == file ||
        NULL == buf  ||
        NULL == pos  ||
        strncmp(file->path,PATH_KEY4,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case GPIO_KEY4_PIN:
           *data = am_hal_gpio_input_bit_read(GPIO_KEY4_PIN);
        default:
            break;
    }

	return ERR_SUCCESS;
}
static int32_t KEY4_GPIO_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
		default:
			break;
    }

    return file->pos;
}
/* file ops: ioctl */
static int32_t KEY4_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_KEY4,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_KEY_INT_ENABLE)
	{
		am_hal_gpio_int_polarity_bit_set(GPIO_KEY4_PIN, AM_HAL_GPIO_FALLING);

		am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY4_PIN));
		//
		// Enable the GPIO/button interrupt.
		//
		am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(GPIO_KEY4_PIN));

		am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);
	}
	else if( request & REQ_SET_GPIO_KEY_INT_DISABLE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY4_PIN));
	}
	else if( request & REQ_SET_GPIO_KEY_RELEASE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY4_PIN));
		am_hal_gpio_pin_config(GPIO_KEY4_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_KEY_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_KEY4_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
	}
	
	return ERR_SUCCESS;
}

static int32_t KEY4_GPIO_IRQ_Register(  struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback  )
{
    int32_t ret = ERR_UNDEFINE;

    if( NULL == file     ||
        NULL == callback ||
        strncmp(file->path,PATH_KEY4,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    if( GPIO_KEY4_PIN  == args )
    {
        am_hal_gpio_int_register(GPIO_KEY4_PIN, callback);
        ret = ERR_SUCCESS ;
    }

    return ret ;
}

/* file operation */
struct FileOps key4_gpio_ops =
{
    KEY4_GPIO_Open,		    /* open */
    KEY4_GPIO_Release,	 	/* release */
    KEY4_GPIO_Read,		    /* read */
    NULL,	   		/* write */
    KEY4_GPIO_Lseek,			/* lseek */
    KEY4_GPIO_Ioctl,			/* ioctl */
    KEY4_GPIO_IRQ_Register,   /* irq_handler_register */
};

/* key4 file descriptor */
struct File file_key4 =
{
	PATH_KEY4,  	    /* path */
    0,				    /* pos */
    &key4_gpio_ops,	/* ops */
    0,                  /* private */
    0,	  			    /* count */
};
static void init_config_key4_gpio(void)
{
	register_dev( &file_key4 );
	am_hal_gpio_pin_config(GPIO_KEY4_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
}

//key5
/* file interface: open */
static int32_t KEY5_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_KEY5,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t KEY5_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_KEY5,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */
		
        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}
/* file ops: read */
static int32_t KEY5_GPIO_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint8_t* data = (uint8_t*)buf;
    uint32_t dw_pin_pos = *pos;        /* new position */


    if( NULL == file ||
        NULL == buf  ||
        NULL == pos  ||
        strncmp(file->path,PATH_KEY5,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    switch( dw_pin_pos )
    {
        case GPIO_KEY5_PIN:
           *data = am_hal_gpio_input_bit_read(GPIO_KEY5_PIN);
        default:
            break;
    }

	return ERR_SUCCESS;
}
static int32_t KEY5_GPIO_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
		default:
			break;
    }

    return file->pos;
}
/* file ops: ioctl */
static int32_t KEY5_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_KEY5,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_KEY_INT_ENABLE)
	{
		am_hal_gpio_int_polarity_bit_set(GPIO_KEY5_PIN, AM_HAL_GPIO_FALLING);

		am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY5_PIN));
		//
		// Enable the GPIO/button interrupt.
		//
		am_hal_gpio_int_enable(AM_HAL_GPIO_BIT(GPIO_KEY5_PIN));

		am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);
	}
	else if( request & REQ_SET_GPIO_KEY_INT_DISABLE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY5_PIN));
	}
	else if( request & REQ_SET_GPIO_KEY_RELEASE )
	{
		am_hal_gpio_int_disable(AM_HAL_GPIO_BIT(GPIO_KEY5_PIN));
		am_hal_gpio_pin_config(GPIO_KEY5_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_KEY_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_KEY5_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
	}
	
	return ERR_SUCCESS;
}

static int32_t KEY5_GPIO_IRQ_Register(  struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback  )
{
    int32_t ret = ERR_UNDEFINE;

    if( NULL == file     ||
        NULL == callback ||
        strncmp(file->path,PATH_KEY5,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    if( GPIO_KEY5_PIN  == args )
    {
        am_hal_gpio_int_register(GPIO_KEY5_PIN, callback);
        ret = ERR_SUCCESS ;
    }

    return ret ;
}

/* file operation */
struct FileOps key5_gpio_ops =
{
    KEY5_GPIO_Open,		    /* open */
    KEY5_GPIO_Release,	 	/* release */
    KEY5_GPIO_Read,		    /* read */
    NULL,	   		/* write */
    KEY5_GPIO_Lseek,			/* lseek */
    KEY5_GPIO_Ioctl,			/* ioctl */
    KEY5_GPIO_IRQ_Register,   /* irq_handler_register */
};

/* key5 file descriptor */
struct File file_key5 =
{
	PATH_KEY5,  	    /* path */
    0,				    /* pos */
    &key5_gpio_ops,	/* ops */
    0,                  /* private */
    0,	  			    /* count */
};
static void init_config_key5_gpio(void)
{
	register_dev( &file_key5 );
	am_hal_gpio_pin_config(GPIO_KEY5_PIN, AM_HAL_GPIO_INPUT|AM_HAL_GPIO_PULL6K);
}




//leds
/* file interface: open */
static int32_t LED1_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_LED1,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t LED1_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_LED1,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: write */
static int32_t LED1_GPIO_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
    uint8_t* data = (uint8_t*)buf;

    if( NULL == file || count<=0 || strncmp(file->path,PATH_LED1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( LED_ON == *data )
	{
		am_hal_gpio_out_bit_set(GPIO_LED1_PIN);
	}
	else
	{
		am_hal_gpio_out_bit_clear(GPIO_LED1_PIN);
	}
	return 1;

}

/* file ops: ioctl */
static int32_t LED1_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_LED1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_LED_RELEASE)
	{
		am_hal_gpio_out_bit_clear(GPIO_LED1_PIN);
		am_hal_gpio_pin_config(GPIO_LED1_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_LED_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_LED1_PIN, AM_HAL_GPIO_OUTPUT);
		am_hal_gpio_out_bit_set(GPIO_LED1_PIN);
	}
	
	return ERR_SUCCESS;
}


/* file operation */
struct FileOps led1_gpio_ops =
{
    LED1_GPIO_Open,		    /* open */
    LED1_GPIO_Release,	 	/* release */
    NULL,		            /* read */
    LED1_GPIO_Write,	   	/* write */
    NULL,			        /* lseek */
    LED1_GPIO_Ioctl,		/* ioctl */
    NULL,                   /* irq_handler_register */
};

/* led1 file descriptor */
struct File file_led1 =
{
	PATH_LED1,  	    /* path */
    0,				    /* pos */
    &led1_gpio_ops,	    /* ops */
    0,                  /* private */
    0,	  			    /* count */
};
static void init_config_led1_gpio(void)
{
	register_dev( &file_led1 );
	am_hal_gpio_pin_config(GPIO_LED1_PIN, AM_HAL_GPIO_OUTPUT);
	am_hal_gpio_out_bit_clear(GPIO_LED1_PIN);
	//am_hal_gpio_out_bit_set(GPIO_LED1_PIN);
}


//led2
/* file interface: open */
static int32_t LED2_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_LED2,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t LED2_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_LED2,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */
		
        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: write */
static int32_t LED2_GPIO_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
    uint8_t* data = (uint8_t*)buf;

    if( NULL == file|| count <= 0|| strncmp(file->path,PATH_LED2,4) != 0 )
    {
        return ERR_PARAMETER ;
    }
	if( LED_ON == *data)
	{
		am_hal_gpio_out_bit_set(GPIO_LED2_PIN);
	}
	else
	{
		am_hal_gpio_out_bit_clear(GPIO_LED2_PIN);
	}
	
	return 1;
}

/* file ops: ioctl */
static int32_t LED2_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_LED2,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_LED_RELEASE)
	{
		am_hal_gpio_out_bit_clear(GPIO_LED2_PIN);
		am_hal_gpio_pin_config(GPIO_LED2_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_LED_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_LED2_PIN, AM_HAL_GPIO_OUTPUT);
		am_hal_gpio_out_bit_set(GPIO_LED2_PIN);
	}
	
	return ERR_SUCCESS;
}

/* file operation */
struct FileOps led2_gpio_ops =
{
    LED2_GPIO_Open,		    /* open */
    LED2_GPIO_Release,	 	/* release */
    NULL,		    		/* read */
    LED2_GPIO_Write,	   	/* write */
    NULL,					/* lseek */
    LED2_GPIO_Ioctl,		/* ioctl */
    NULL,   				/* irq_handler_register */
};

/* led2 file descriptor */
struct File file_led2 =
{
	PATH_LED2,  	    /* path */
    0,				    /* pos */
    &led2_gpio_ops,		/* ops */
    0,                  /* private */
    0,	  			    /* count */
};

static void init_config_led2_gpio(void)
{
	register_dev( &file_led2 );
	am_hal_gpio_pin_config(GPIO_LED2_PIN, AM_HAL_GPIO_OUTPUT);
	am_hal_gpio_out_bit_clear(GPIO_LED2_PIN);
}

//led3
/* file interface: open */
static int32_t LED3_GPIO_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_LED3,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t LED3_GPIO_Release( struct File* file )
{
    if( strncmp(file->path,PATH_LED3,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */
		        
		return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: write */
static int32_t LED3_GPIO_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
    uint8_t* data = (uint8_t*)buf;

    if( NULL == file || count <= 0 || strncmp(file->path,PATH_LED3,4) != 0 )
    {
        return ERR_PARAMETER ;
    }
	if( LED_ON == *data )
	{
		am_hal_gpio_out_bit_set(GPIO_LED3_PIN);	
	}
	else
	{
		am_hal_gpio_out_bit_clear(GPIO_LED3_PIN);
	}
	return 1;

}

/* file ops: ioctl */
static int32_t LED3_GPIO_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	if( NULL == file || strncmp(file->path,PATH_LED3,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	if( request & REQ_SET_GPIO_LED_RELEASE)
	{
		am_hal_gpio_out_bit_clear(GPIO_LED3_PIN);
		am_hal_gpio_pin_config(GPIO_LED3_PIN, AM_HAL_GPIO_DISABLE);
	}
	else if( request & REQ_SET_GPIO_LED_ACTIVE )
	{
		am_hal_gpio_pin_config(GPIO_LED3_PIN, AM_HAL_GPIO_OUTPUT);
		am_hal_gpio_out_bit_set(GPIO_LED3_PIN);
	}
	
	return ERR_SUCCESS;
}


/* file operation */
struct FileOps led3_gpio_ops =
{
    LED3_GPIO_Open,		    /* open */
    LED3_GPIO_Release,	 	/* release */
    NULL,		            /* read */
    LED3_GPIO_Write,	   	/* write */
    NULL,			        /* lseek */
    LED3_GPIO_Ioctl,		/* ioctl */
    NULL,                   /* irq_handler_register */
};

/* led3 file descriptor */
struct File file_led3 =
{
	PATH_LED3,  	    /* path */
    0,				    /* pos */
    &led3_gpio_ops,	/* ops */
    0,                  /* private */
    0,	  			    /* count */
};

static void init_config_led3_gpio(void)
{
	register_dev( &file_led3 );
	am_hal_gpio_pin_config(GPIO_LED3_PIN, AM_HAL_GPIO_OUTPUT);
	am_hal_gpio_out_bit_clear(GPIO_LED3_PIN);
}

/* init usart character driver */
int32_t gpio_init_module(void)
{
    /* init hardware setting of sx1280 gpio */
    init_config_sx1280_gpio();
	/* init hardware setting of KEY1 gpio */
    init_config_key1_gpio();
	/* init hardware setting of KEY2 gpio */
	init_config_key2_gpio();
	/* init hardware setting of KEY3 gpio */
    init_config_key3_gpio();
	/* init hardware setting of KEY4 gpio */
	init_config_key4_gpio();
	/* init hardware setting of KEY5 gpio */
	init_config_key5_gpio();
	/* init hardware setting of led gpio */
	init_config_led1_gpio();
	init_config_led2_gpio();
	init_config_led3_gpio();

	return ERR_SUCCESS;

}



void am_gpio_isr(void)
{
    //
    // Delay for debounce.
    //
    //am_util_delay_ms(200);

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    //am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));
	am_hal_interrupt_disable(AM_HAL_INTERRUPT_GPIO);//1//

    uint64_t ui64Status;
    ui64Status = am_hal_gpio_int_status_get(true);

    am_hal_gpio_int_service(ui64Status);

	//am_hal_gpio_int_clear(ui64Status);
	#if 1
    if((ui64Status>>SX1280_DIO1_PIN)&1)
    {
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(SX1280_DIO1_PIN));
    }
	else if((ui64Status>>GPIO_KEY1_PIN)&1)
    {
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY1_PIN));
    }
	else if((ui64Status>>GPIO_KEY2_PIN)&1)
    {
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY2_PIN));
    }
	else if((ui64Status>>GPIO_KEY3_PIN)&1)
    {
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY3_PIN));
    }
	else if((ui64Status>>GPIO_KEY4_PIN)&1)
    {
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY4_PIN));
    }
	else if((ui64Status>>GPIO_KEY5_PIN)&1)
    {
        am_hal_gpio_int_clear(AM_HAL_GPIO_BIT(GPIO_KEY5_PIN));
    }
    #endif

	am_hal_interrupt_enable(AM_HAL_INTERRUPT_GPIO);
}


