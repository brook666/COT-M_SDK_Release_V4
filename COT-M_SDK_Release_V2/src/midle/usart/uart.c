/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: uart.c
*
* Author: zhangjh
*
* Version: 1.0
*
* Date: 2017-03-14
*
* Description: the functions of serial virtual file system
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
#include "am_reg_uart.h"
#include "am_hal_uart.h"
#include "am_bsp.h"

#include "vfs.h"
#include "uart.h"


/*********************************************************************
 * MACROS
 */
#define UART_RX_BUF_SIZE    1040
#define UART_TX_BUF_SIZE    1040

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/* This global variable sets the UART configuration.
 */
am_hal_uart_config_t g_sUartConfig =
{
    .ui32BaudRate = 115200,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .bTwoStopBits = false,
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32FlowCtrl = AM_HAL_UART_FLOW_CTRL_NONE
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
uint8_t g_uart_rx_fifo[UART_RX_BUF_SIZE];
uint8_t g_uart_tx_fifo[UART_TX_BUF_SIZE];
/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/
static void uart_enable(void);
static void uart_disable(void);
static void init_config_usart1(void);


/* file interface: open
   when args is not 0, it is baudrate config. Default baudrate is setting when args is 0. */
static int32_t USART_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_USART1,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: release */
static int32_t USART_Release( struct File* file )
{

    if( strncmp(file->path,PATH_USART1,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: read */
static int32_t USART_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint32_t len = 0 ;
    uint32_t readed_count = 0 ;
    uint8_t* data = (uint8_t*)buf;

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_USART1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }
    len = count < UART_TX_BUF_SIZE ? count : UART_TX_BUF_SIZE ;// len = count > UART_TX_BUF_SIZE ? count : UART_TX_BUF_SIZE ;
    //am_hal_interrupt_disable(AM_HAL_INTERRUPT_UART);
    readed_count = am_hal_uart_char_receive_buffered(0,(char *)data, len);
    //am_hal_interrupt_enable(AM_HAL_INTERRUPT_UART);


	return readed_count;
}

/* file ops: write */
static int32_t USART_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint32_t i;
    uint8_t* data = (uint8_t*)buf;

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_USART1,4) != 0 )
    {
        return ERR_PARAMETER ;
    }
	for( i = 0; i < count; i++ )
	{
	    am_hal_interrupt_disable(AM_HAL_INTERRUPT_UART);
		/* send data and wait util it's done */
		am_hal_uart_char_transmit_buffered(0,data[i]);
        am_hal_interrupt_enable(AM_HAL_INTERRUPT_UART);
	}
	//am_hal_uart_string_transmit_polled(0,(char *)data);
	return i;
}

/* file ops: ioctl */
static int32_t  USART_Ioctl( struct File* file, int32_t request, uint32_t args )
{

	int8_t ret = -1;	/* ret value */

    if( NULL == file || strncmp(file->path,PATH_USART1,4) != 0 )
        return ERR_PARAMETER ;

    if(request & REQ_SET_BUADRATE)
    {
        g_sUartConfig.ui32BaudRate = args ;
        am_hal_uart_disable(0);
        am_hal_uart_config(0,&g_sUartConfig);
        am_hal_uart_enable(0);
        ret = ERR_SUCCESS;
    }

    if(request & REQ_IT_RXNE)
    {
        am_hal_uart_int_enable(0,AM_HAL_UART_INT_RX);
        ret = ERR_SUCCESS;
    }

    if(request & REQ_IT_TXE)
    {
        am_hal_uart_int_enable(0,AM_HAL_UART_INT_TX);
        ret = ERR_SUCCESS;
    }

    if(request & REQ_IT_RX_TMOUT)
    {
        am_hal_uart_int_enable(0,AM_HAL_UART_INT_RX_TMOUT);
        ret = ERR_SUCCESS;
    }

    if(request & REQ_IT_UART)
    {
        am_hal_interrupt_enable(AM_HAL_INTERRUPT_UART);
        ret = ERR_SUCCESS;
    }

    if(request & REQ_UART_DISABLE)
    {
        uart_disable();
        ret = ERR_SUCCESS;
    }
    
    if(request & REQ_UART_ENABLE)
    {
        am_hal_uart_disable(0);
        uart_enable();
        ret = ERR_SUCCESS;
    }
    
	return ret;

}

/* file operation */
struct FileOps usart_ops =
{
   USART_Open,		    /* open */
   USART_Release,	 	/* release */
   USART_Read,		    /* read */
   USART_Write,	   		/* write */
   NULL,				/* lseek */
   USART_Ioctl,			/* ioctl */
   NULL,                /* irq handler regiser */
};

/* usart1 file descriptor */
struct File file_uart1 =
{
	PATH_USART1,  	    /* path */
    0,				    /* pos */
    &usart_ops,	   	    /* ops */
    0,                  /* private */
    0,	  			    /* count */
};


/*Enable Buffered UART Communication.*/
static void uart_enable(void)
{

    //
    // Enable the necessary pins for talking to the PC via the UART.
    //
    am_bsp_pin_enable(COM_UART_TX);
    am_bsp_pin_enable(COM_UART_RX);
    //am_bsp_pin_enable(COM_UART_RTS);
    //am_bsp_pin_enable(COM_UART_CTS);

    //
    // Enable the clock to the UART.
    //
    am_hal_uart_clock_enable(0);
	//
    // Disable and configure the UART.
    //
    am_hal_uart_disable(0);
    am_hal_uart_config(0,&g_sUartConfig);

    //触发串口深度
    am_hal_uart_fifo_config(0,AM_HAL_UART_TX_FIFO_1_2 | AM_HAL_UART_RX_FIFO_1_2);

    am_hal_uart_enable(0);

    am_hal_uart_int_enable(0,AM_HAL_UART_INT_TX);
    am_hal_uart_int_enable(0,AM_HAL_UART_INT_RX);
    am_hal_uart_int_enable(0,AM_HAL_UART_INT_RX_TMOUT);
	am_hal_interrupt_enable(AM_HAL_INTERRUPT_UART);

}



/* Disable the Buffered UART.*/
#if 1
static void uart_disable(void)
{

    //
    // Wait for the UART lines to stop toggling, and make sure we have
    // absolutely no interrupts pending.
    //
    while (am_hal_uart_flags_get(0) & AM_REG_UART_FR_BUSY_BUSY);
    am_hal_uart_int_clear(0,0xFFFFFFFF);

    //
    // Disable the UART pins.
    //
    am_bsp_pin_disable(COM_UART_TX);
    am_bsp_pin_disable(COM_UART_RX);
    //am_bsp_pin_disable(COM_UART_RTS);
    //am_bsp_pin_disable(COM_UART_CTS);

    //
    // Disable the clock to the UART.
    //
    am_hal_uart_disable(0);
    am_hal_uart_clock_disable(0);
}
#endif

/*Interrupt handler for the UART*/
void am_uart_isr(void)
{
    uint32_t ui32Status;

    //
    // Read the masked interrupt status from the UART.
    //
    ui32Status = am_hal_uart_int_status_get(0,true);

    //
    // Service the buffered UART.
    //
    am_hal_uart_service_buffered(0,ui32Status);

    //
    // Clear the UART interrupts.
    //
    am_hal_uart_int_clear(0,ui32Status);

}


/* init usart1 config */
static void init_config_usart1(void)
{

    /* init usart1: register device file */
    register_dev( &file_uart1 );

    //
    // Initialize and Enable the UART.
    uart_enable();

	//
    // Now crank up the ring buffered interface to the UART.
    //
    am_hal_uart_init_buffered(0,g_uart_rx_fifo,UART_RX_BUF_SIZE, g_uart_tx_fifo, UART_TX_BUF_SIZE);

}

/* init usart character driver */
int32_t uart_init_module(void)
{
    /* init hardware setting of usart1 */
    init_config_usart1();

	return 0;
}


