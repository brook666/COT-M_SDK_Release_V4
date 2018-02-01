/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: spi.c
*
* Author: hzx
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
 * INCLUDES
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"

#include "am_reg_base_addresses.h"
#include "am_reg_macros.h"
#include "am_reg_gpio.h"
#include "am_reg_iomstr.h"
#include "am_hal_gpio.h"
#include "am_hal_iom.h"


#include "vfs.h"
#include "spi.h"

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
static int32_t g_spi_final_frame_nss_status = SET_NSS_HIGH ;
//*****************************************************************************
//
//! @brief Configuration structure for the IO master module.
//
//*****************************************************************************
am_hal_iom_config_t g_spi_config = {
	.ui32InterfaceMode = AM_HAL_IOM_SPIMODE,
	.ui32ClockFrequency = AM_HAL_IOM_6MHZ,
	.bSPHA = 0,
	.bSPOL = 0,
	.ui8WriteThreshold = 4,
	.ui8ReadThreshold = 60
};

//*****************************************************************************
//
//! Configuration structure for an individual SPI device.
//
//*****************************************************************************

am_hal_iom_spi_device_t g_spi_device =
{
    .ui32Module = SPI0_IOM_MODULE,
	.ui32ChipSelect = SPI0_CHIP_SELECT,
    .ui32Options = 0
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
static void spi_disable(void);
static void init_config_spi(void);
static void configure_pins(void);

/* file interface: open */
static int32_t SPI_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_SPI0,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: release */
static int32_t SPI_Release( struct File* file )
{
    if( strncmp(file->path,PATH_SPI0,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;

}

/* file ops: read */
static int32_t SPI_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{
	uint32_t i,j,n,m;
    //uint32_t ui32_bytes_remaining;
    uint32_t ui32_transfer_size;
    uint32_t p_ui32_read_buffer[32];

    uint8_t *p_ui8_read_ptr;
    uint8_t* pui8Dest = (uint8_t *)buf;

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_SPI0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    memset(&p_ui32_read_buffer,0x0,sizeof(p_ui32_read_buffer));
    p_ui8_read_ptr = (uint8_t *)(&p_ui32_read_buffer);
    //
    // Set the total number of bytes,and the starting transfer destination.
    //
    #if 0
	n = count/64 ;//64
	m = count%64 ;
	for(i=0 ; i<n ; i++)
	{
        //
        // Prepare for a SPI read command.
        //
        //am_hal_iom_enable(0);
        //am_hal_iom_int_clear(0, AM_HAL_IOM_INT_CMDCMP | AM_HAL_IOM_INT_THR);
		ui32_transfer_size = 64 ;
        am_hal_iom_spi_read(g_spi_device.ui32Module,
                            g_spi_device.ui32ChipSelect,
                            p_ui32_read_buffer,
                            ui32_transfer_size,
                            g_spi_device.ui32Options | AM_HAL_IOM_RAW);

        //
        // Copy the received bytes over to the RxBuffer
        //
        for(j = 0; j < ui32_transfer_size; j++)
        {
            pui8Dest[j] = p_ui8_read_ptr[j];
        }

        //
        // Update the number of bytes remaining and the destination.
        //
        pui8Dest += ui32_transfer_size;
	}

	//
	// Prepare for a SPI read command.
	//
	//am_hal_iom_enable(0);
	//am_hal_iom_int_clear(0, AM_HAL_IOM_INT_CMDCMP | AM_HAL_IOM_INT_THR);
	if( SET_NSS_HIGH == g_spi_final_frame_nss_status )
	{
		g_spi_device.ui32Options &= ( ~(AM_HAL_IOM_CS_LOW) );
	}
	ui32_transfer_size = m ;
	am_hal_iom_spi_read(g_spi_device.ui32Module,
						g_spi_device.ui32ChipSelect,
						p_ui32_read_buffer,
						ui32_transfer_size,
						g_spi_device.ui32Options | AM_HAL_IOM_RAW);

	//
	// Copy the received bytes over to the RxBuffer
	//
	for(j = 0; j < ui32_transfer_size; j++)
	{
		pui8Dest[j] = p_ui8_read_ptr[j];
	}
	//
	// Update the number of bytes remaining and the destination.
	//
	//pui8Dest += ui32_transfer_size;
    #endif
	
	#if 1
	n = count/64 ;//64
	m = count%64 ;
	for(i=0 ; i<n ; i++)
	{
		if( m==0 && i== n-1 )
		{
			if( SET_NSS_HIGH == g_spi_final_frame_nss_status )
			{
				g_spi_device.ui32Options &= ( ~(AM_HAL_IOM_CS_LOW) );
			}
		}
		
        //
        // Prepare for a SPI read command.
        //
        //am_hal_iom_enable(0);
        //am_hal_iom_int_clear(0, AM_HAL_IOM_INT_CMDCMP | AM_HAL_IOM_INT_THR);
		ui32_transfer_size = 64 ;
        am_hal_iom_spi_read(g_spi_device.ui32Module,
                            g_spi_device.ui32ChipSelect,
                            p_ui32_read_buffer,
                            ui32_transfer_size,
                            g_spi_device.ui32Options | AM_HAL_IOM_RAW);

        //
        // Copy the received bytes over to the RxBuffer
        //
        for(j = 0; j < ui32_transfer_size; j++)
        {
            pui8Dest[j] = p_ui8_read_ptr[j];
        }

        //
        // Update the number of bytes remaining and the destination.
        //
        pui8Dest += ui32_transfer_size;
	}

	//
	// Prepare for a SPI read command.
	//
	//am_hal_iom_enable(0);
	//am_hal_iom_int_clear(0, AM_HAL_IOM_INT_CMDCMP | AM_HAL_IOM_INT_THR);
	if( m > 0 )
	{
		if( SET_NSS_HIGH == g_spi_final_frame_nss_status )
		{
			g_spi_device.ui32Options &= ( ~(AM_HAL_IOM_CS_LOW) );
		}
		ui32_transfer_size = m ;
		am_hal_iom_spi_read(g_spi_device.ui32Module,
							g_spi_device.ui32ChipSelect,
							p_ui32_read_buffer,
							ui32_transfer_size,
							g_spi_device.ui32Options | AM_HAL_IOM_RAW);

		//
		// Copy the received bytes over to the RxBuffer
		//
		for(j = 0; j < ui32_transfer_size; j++)
		{
			pui8Dest[j] = p_ui8_read_ptr[j];
		}
	}

	//
	// Update the number of bytes remaining and the destination.
	//
	//pui8Dest += ui32_transfer_size;
    #endif

    return count;
}


/* file ops: write */
static int32_t SPI_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos)
{
	uint8_t* p_ui8_source = (uint8_t*)buf;

    uint32_t i,j,m,n;
    //uint32_t ui32_bytes_remaining;
    uint32_t ui32_transfer_size;

    am_hal_iom_buffer(64) ps_write_data;//96

    memset((uint8_t *)&ps_write_data,0,sizeof(ps_write_data));

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_SPI0,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    #if 0
	n = count/32 ;//32
	m = count%32 ;

	for( i=0 ; i<n ; i++ )
	{
		//
		// Fill the rest of the command buffer with the data that we actually
		// want to write .
		//
		ui32_transfer_size = 32 ;
		for(j = 0; j < ui32_transfer_size; j++)
		{
			ps_write_data.bytes[j] = p_ui8_source[j];
		}

		//
		// Send the write data.
		//
		am_hal_iom_spi_write(g_spi_device.ui32Module,
							 g_spi_device.ui32ChipSelect,
							 ps_write_data.words,
							 ui32_transfer_size,
							 g_spi_device.ui32Options | AM_HAL_IOM_RAW);


		//
		// Update the number of bytes remaining, as well as the source and
		// destination pointers
		//
		p_ui8_source += ui32_transfer_size;
	}

	ui32_transfer_size = m ;
	for(j = 0; j < ui32_transfer_size; j++)
	{
		ps_write_data.bytes[j] = p_ui8_source[j];
	}

	if( SET_NSS_HIGH == g_spi_final_frame_nss_status )
	{
		g_spi_device.ui32Options &= ( ~(AM_HAL_IOM_CS_LOW) );
	}
	//
	// Send the write data.
	//
	am_hal_iom_spi_write(g_spi_device.ui32Module,
						 g_spi_device.ui32ChipSelect,
						 ps_write_data.words,
						 ui32_transfer_size,
						 g_spi_device.ui32Options | AM_HAL_IOM_RAW);


	//
	// Update the number of bytes remaining, as well as the source and
	// destination pointers
	//
	//p_ui8_source += ui32_transfer_size;
    #endif

	#if 1
	n = count/32 ;//32
	m = count%32 ;

	for( i=0 ; i<n ; i++ )
	{
		//
		// Fill the rest of the command buffer with the data that we actually
		// want to write .
		//
		if( m==0 && i==n-1)
		{
			if( SET_NSS_HIGH == g_spi_final_frame_nss_status )
			{
				g_spi_device.ui32Options &= ( ~(AM_HAL_IOM_CS_LOW) );
			}
		}
			
		ui32_transfer_size = 32 ;
		for(j = 0; j < ui32_transfer_size; j++)
		{
			ps_write_data.bytes[j] = p_ui8_source[j];
		}

		//
		// Send the write data.
		//
		am_hal_iom_spi_write(g_spi_device.ui32Module,
							 g_spi_device.ui32ChipSelect,
							 ps_write_data.words,
							 ui32_transfer_size,
							 g_spi_device.ui32Options | AM_HAL_IOM_RAW);


		//
		// Update the number of bytes remaining, as well as the source and
		// destination pointers
		//
		p_ui8_source += ui32_transfer_size;
	}

	if( m >0 )
	{
		ui32_transfer_size = m ;
		for(j = 0; j < ui32_transfer_size; j++)
		{
			ps_write_data.bytes[j] = p_ui8_source[j];
		}

		if( SET_NSS_HIGH == g_spi_final_frame_nss_status )
		{
			g_spi_device.ui32Options &= ( ~(AM_HAL_IOM_CS_LOW) );
		}
		//
		// Send the write data.
		//
		am_hal_iom_spi_write(g_spi_device.ui32Module,
							 g_spi_device.ui32ChipSelect,
							 ps_write_data.words,
							 ui32_transfer_size,
							 g_spi_device.ui32Options | AM_HAL_IOM_RAW);
	}



	//
	// Update the number of bytes remaining, as well as the source and
	// destination pointers
	//
	//p_ui8_source += ui32_transfer_size;
    #endif
	
	return count;

}




static int32_t SPI_Ioctl( struct File* file, int32_t request, uint32_t args )
{

    if( NULL == file || strncmp(file->path,PATH_SPI0,4) != 0 )
        return ERR_PARAMETER ;

	switch(request)
	{
		case CLOCK_FREQUENCY:
			break;

        case SET_SPI_OPTION:
            g_spi_device.ui32Options = args ;
            break;
        case SET_SPI_WRITE_FINAL_FRAME_NSS_STATUS:
            g_spi_final_frame_nss_status = args;
            break;
        case SET_SPI_DISABLE:
            spi_disable(); /* Disable spi */
            break;
        case SET_SPI_ENABLE:
            configure_pins(); /* enable spi */
            break;
		default:
			break;
	}
	return 0;
}

/* file ops: lseek */
static int32_t SPI_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    if( NULL == file || strncmp(file->path,PATH_SPI0,4) != 0 )
        return ERR_PARAMETER ;

    switch( whence )
    {
        case SPI_READ_ADDER:

            break;

        case SPI_WRITE_ADDER:

            break;

        default:
			break;
    }

    return file->pos;
}
/* file operation */
struct FileOps spi_ops =
{
   SPI_Open,		    /* open */
   SPI_Release,	 	    /* release */
   SPI_Read,		    /* read */
   SPI_Write,	   		/* write */
   SPI_Lseek,			/* lseek */
   SPI_Ioctl,			/* ioctl */
   NULL,                /* irq handler regiser */
};

/* usart1 file descriptor */
struct File file_spi =
{
	PATH_SPI0,  	        /* path */
    0,				    /* pos */
    &spi_ops,	   	    /* ops */
    0,                  /* private */
    0,	  			    /* count */
};

//*****************************************************************************
//
// Configure GPIOs for this example
//
//*****************************************************************************
static void configure_pins(void)
{
	//
    // init spi0 pin
    //
    am_hal_gpio_pin_config(SPI0_MISO_PIN,SPI0_MISO);
	am_hal_gpio_pin_config(SPI0_MOSI_PIN,SPI0_MOSI);
	am_hal_gpio_pin_config(SPI0_SCK_PIN,SPI0_SCK);
	am_hal_gpio_pin_config(SPI0_NSS_PIN,SPI0_NSS);

    g_spi_device.ui32Module = SPI0_IOM_MODULE ;

    g_spi_final_frame_nss_status = SET_NSS_LOW ;

    am_hal_iom_config(g_spi_device.ui32Module,&g_spi_config);

    am_hal_iom_enable(g_spi_device.ui32Module);

}

/* Disable the spi.*/
static void spi_disable(void)
{
	am_hal_iom_disable(g_spi_device.ui32Module);

	//
    //  spi0 pin
    //
    am_hal_gpio_pin_config(SPI0_MISO_PIN,AM_HAL_GPIO_DISABLE);
	am_hal_gpio_pin_config(SPI0_MOSI_PIN,AM_HAL_GPIO_DISABLE);
	am_hal_gpio_pin_config(SPI0_SCK_PIN,AM_HAL_GPIO_DISABLE);
	//am_hal_gpio_pin_config(SPI0_NSS_PIN,AM_HAL_GPIO_DISABLE);

    //am_hal_gpio_out_bit_set(SPI0_NSS_PIN);
    
    g_spi_final_frame_nss_status = SET_NSS_HIGH ;
}

/* init spi config */
static void init_config_spi(void)
{

    /* init spi: register device file */
    register_dev(&file_spi);
    //
    // Initialize and Enable the spi.
    configure_pins();

}

/* init usart character driver */
int32_t spi_init_module(void)
{
    /* init hardware setting of usart1 */
    init_config_spi();

	return 0;
}
