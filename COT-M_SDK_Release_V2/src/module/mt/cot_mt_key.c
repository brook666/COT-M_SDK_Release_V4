/*********************************************************************
    Filename:       cot_mt_key.c
    Revised:        $Date: 2017-03-25 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the cot key API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/

/**************************************************************************************************
 *                                             INCLUDES
 **************************************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "osal.h"
#include "vfs.h"
#include "gpio.h"
#include "log.h"
#include "cot_mt.h"
#include "cot_mt_key.h"


/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/
#define HAL_KEY_INTERRUPT_DISABLE		0x00
#define HAL_KEY_INTERRUPT_ENABLE 		0x01


/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/
#define HAL_KEY_DEBOUNCE_VALUE  25



/**************************************************************************************************
 *                                            TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/
static halKeyCBack_t pHalKeyProcessFunction;
static uint8_t halKeySavedKeys;     /* used to store previous key state in polling mode */
static uint8_t HalKeyConfigured;
static bool cot_mt_key_int_enable;            /* interrupt enable/disable flag */

static uint8_t s_cot_mt_task_id = 0;
static uint8_t s_press_keys = 0;
/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
uint8_t cot_mt_key_send_keys( uint8_t keys, uint8_t state );

/*
 * Configure the Key Service
 */
static void cot_mt_key_config( uint8_t keys, bool interruptEnable, const halKeyCBack_t cback);

void cot_mt_key_callback ( uint8_t keys, uint8_t state );

void halProcessKeyInterrupt(void);

void cot_key1_isr(void);
void cot_key2_isr(void);
void cot_key3_isr(void);
void cot_key4_isr(void);
void cot_key5_isr(void);


/**************************************************************************************************
 * @fn      cot_key_init
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void cot_mt_key_init( uint8_t task_id )
{
	/* Initialize previous key to 0 */
	halKeySavedKeys = 0;

	/* Initialize callback function */
	pHalKeyProcessFunction  = NULL;

	/* Start with key is not configured */
	HalKeyConfigured = false;
	
	s_press_keys = 0;
	
	s_cot_mt_task_id = task_id;
	
	int32_t fd = 0;
	fd = open(PATH_KEY1,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		irq_handler_register(fd,0,GPIO_KEY1_PIN,cot_key1_isr);	
		close(fd);
	}

	fd = open(PATH_KEY2,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		irq_handler_register(fd,0,GPIO_KEY2_PIN,cot_key2_isr);	
		close(fd);
	}
	
	fd = open(PATH_KEY3,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		irq_handler_register(fd,0,GPIO_KEY3_PIN,cot_key3_isr);	
		close(fd);
	}
	
	fd = open(PATH_KEY4,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		irq_handler_register(fd,0,GPIO_KEY4_PIN,cot_key4_isr);	
		close(fd);
	}
	
	fd = open(PATH_KEY5,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		irq_handler_register(fd,0,GPIO_KEY5_PIN,cot_key5_isr);	
		close(fd);
	}
	
	cot_mt_key_config(0,HAL_KEY_INTERRUPT_ENABLE,cot_mt_key_callback);
		
}


bool get_cot_mt_key_int_enadble_status( void )
{
	return cot_mt_key_int_enable;
}

/**************************************************************************************************
 * @fn      cot_mt_key_config
 *
 * @brief   Configure the Key serivce
 *
 * @param   interruptEnable - TRUE/FALSE, enable/disable interrupt
 *          cback - pointer to the CallBack function
 *
 * @return  None
 **************************************************************************************************/
static void cot_mt_key_config (uint8_t keys, bool interruptEnable, halKeyCBack_t cback)
{
	/* Enable/Disable Interrupt or */
	cot_mt_key_int_enable = interruptEnable;

	/* Register the callback fucntion */
	pHalKeyProcessFunction = cback;

	/* Determine if interrupt is enable or not */
	if (cot_mt_key_int_enable)
	{		
		/* Do this only after the hal_key is configured - to work with sleep stuff */
		if (HalKeyConfigured == TRUE)
		{
			//uint8_t effect_key = 0;
			int32_t fd = 0;
			
			if(s_press_keys & COT_MT_KEY_SW_1)
			{
				fd = open(PATH_KEY1,0);
				if( fd > 0 )
				{	
					s_press_keys &= (~COT_MT_KEY_SW_1);
					ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);	
					close(fd);
				}
			}

			if(s_press_keys & COT_MT_KEY_SW_2)
			{
				fd = open(PATH_KEY2,0);
				if( fd > 0 )	
				{	
					s_press_keys &= (~COT_MT_KEY_SW_2);
					ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
					close(fd);
				}
			}
			
			if(s_press_keys & COT_MT_KEY_SW_3)
			{
				fd = open(PATH_KEY3,0);
				if( fd > 0 )	
				{	
					s_press_keys &= (~COT_MT_KEY_SW_3);
					ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
					close(fd);
				}
			}

			if(s_press_keys & COT_MT_KEY_SW_4)
			{
				fd = open(PATH_KEY4,0);
				if( fd > 0 )	
				{	
					s_press_keys &= (~COT_MT_KEY_SW_4);
					ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
					close(fd);
				}
			}
			
			if(s_press_keys & COT_MT_KEY_SW_5)
			{
				fd = open(PATH_KEY5,0);
				if( fd > 0 )	
				{	
					s_press_keys &= (~COT_MT_KEY_SW_5);
					ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
					close(fd);
				}
			}
			
			/*all key up*/ 
			if( s_press_keys == 0 )
			{
				/* Cancel polling if active */
				mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+2);
				if (g_pMsg)
				{
					/* Fill up what we can */
					g_pMsg->hdr.event = CMD_KEY_MSG;
					g_pMsg->event_cmd = KEY_EVENT_DETECT_INT_ENABLE_CMD;
					g_pMsg->msg = (uint8_t*)(g_pMsg+1);
					g_pMsg->msg_len = 0;
					osal_msg_send(s_cot_mt_task_id,(uint8_t *)g_pMsg);
				}
			}
		}
	}
	else/* Interrupts NOT enabled */
	{
		/* don't generate interrupt */
		/* Clear interrupt enable bit */
		int32_t fd = 0;
		
		if(keys & COT_MT_KEY_SW_1)
		{
			fd = open(PATH_KEY1,0);
			if( fd > 0 )
			{	
				ioctl(fd,REQ_SET_GPIO_KEY_INT_DISABLE,0);	
				close(fd);
			}
		}

		if(keys & COT_MT_KEY_SW_2)
		{
			fd = open(PATH_KEY2,0);
			if( fd > 0 )	
			{	
				ioctl(fd,REQ_SET_GPIO_KEY_INT_DISABLE,0);
				close(fd);
			}
		}
		
		if(keys & COT_MT_KEY_SW_3)
		{
			fd = open(PATH_KEY3,0);
			if( fd > 0 )	
			{	
				ioctl(fd,REQ_SET_GPIO_KEY_INT_DISABLE,0);
				close(fd);
			}
		}

		if(keys & COT_MT_KEY_SW_4)
		{
			fd = open(PATH_KEY4,0);
			if( fd > 0 )	
			{	
				ioctl(fd,REQ_SET_GPIO_KEY_INT_DISABLE,0);
				close(fd);
			}
		}
		
		if(keys & COT_MT_KEY_SW_5)
		{
			fd = open(PATH_KEY5,0);
			if( fd > 0 )	
			{	
				ioctl(fd,REQ_SET_GPIO_KEY_INT_DISABLE,0);
				close(fd);
			}
		}
	}

	/* Key now is configured */
	HalKeyConfigured = TRUE;
}

/*********************************************************************
 * @fn      cot_mt_key_callback
 *
 * @brief   Callback service for keys
 *
 * @param   keys  - keys that were pressed
 *          state - shifted
 *
 * @return  void
 *********************************************************************/
void cot_mt_key_callback ( uint8_t keys, uint8_t state )
{
	uint8_t shift;
	(void)state;
	
	// shift key (S1) is used to generate key interrupt
	// applications should not use S1 when key interrupt is enabled
	shift = (cot_mt_key_int_enable == HAL_KEY_INTERRUPT_ENABLE) ? false : ((keys & COT_MT_KEY_SW_1) ? true : false);
	if ( cot_mt_key_send_keys( keys, shift ) != SUCCESS )//
	{
		//TODO send key error......
	}
	
	/* If any key is currently pressed down and interrupt
	is still enabled, disable interrupt and switch to polling */
	if( keys != 0 )
	{
		/*first key press instantly */
		if( cot_mt_key_int_enable == HAL_KEY_INTERRUPT_ENABLE )
		{
			cot_mt_key_int_enable = HAL_KEY_INTERRUPT_DISABLE;
			cot_mt_key_config(keys,cot_mt_key_int_enable, cot_mt_key_callback);
		}
	}
	/* If no key is currently pressed down and interrupt
	is disabled, enable interrupt and turn off polling */
	else
	{
		if( cot_mt_key_int_enable == HAL_KEY_INTERRUPT_DISABLE )
		{
			/*all key up*/
			cot_mt_key_int_enable = HAL_KEY_INTERRUPT_ENABLE;
			cot_mt_key_config( keys,cot_mt_key_int_enable, cot_mt_key_callback);
		}
	}
}

/**************************************************************************************************
 * @fn      cot_mt_key_poll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void cot_mt_key_poll (void)
{
	uint8_t keys = 0;
	uint8_t notify = 0;
	
	if( s_press_keys & COT_MT_KEY_SW_1 )
	{
		int32_t fd1 = 0;
		fd1 = open(PATH_KEY1,0);
		if( fd1 > 0 )
		{
			uint8_t key_value1 = 1;
			lseek(fd1,GPIO_KEY1_PIN,SEEK_SET);
			read( fd1,&key_value1,1);
			close(fd1);
			
			if(!key_value1)
			{
				keys |= COT_MT_KEY_SW_1;
			}
		}
	}
	
	if( s_press_keys & COT_MT_KEY_SW_2 )
	{
		int32_t fd2 = 0;
		fd2 = open(PATH_KEY2,0);
		if( fd2 > 0 )
		{
			uint8_t key_value2 = 1;
			lseek(fd2,GPIO_KEY2_PIN,SEEK_SET);
			read( fd2,&key_value2,1);
			close(fd2);
			
			if(!key_value2)
			{
				keys |= COT_MT_KEY_SW_2;
			}
		}
	}
	
	if( s_press_keys & COT_MT_KEY_SW_3 )
	{
		int32_t fd3 = 0;
		fd3 = open(PATH_KEY3,0);
		if( fd3 > 0 )
		{
			uint8_t key_value3 = 1;
			lseek(fd3,GPIO_KEY3_PIN,SEEK_SET);
			read( fd3,&key_value3,1);
			close(fd3);
			
			if(!key_value3)
			{
				keys |= COT_MT_KEY_SW_3;
			}
		}
	}
	
	if( s_press_keys & COT_MT_KEY_SW_4 )
	{
		int32_t fd4 = 0;
		fd4 = open(PATH_KEY4,0);
		if( fd4 > 0 )
		{
			uint8_t key_value4 = 1;
			lseek(fd4,GPIO_KEY4_PIN,SEEK_SET);
			read( fd4,&key_value4,1);
			close(fd4);
			
			if(!key_value4)
			{
				keys |= COT_MT_KEY_SW_4;
			}
		}
	}
	
	if( s_press_keys & COT_MT_KEY_SW_5 )
	{
		int32_t fd5 = 0;
		fd5 = open(PATH_KEY5,0);
		if( fd5 > 0 )
		{
			uint8_t key_value5 = 1;
			lseek(fd5,GPIO_KEY5_PIN,SEEK_SET);
			read( fd5,&key_value5,1);
			close(fd5);
			
			if(!key_value5)
			{
				keys |= COT_MT_KEY_SW_5;
			}
		}
	}
	
	/* If interrupts are not enabled, previous key status and current key status
	* are compared to find out if a key has changed status.
	*/
	if (!cot_mt_key_int_enable)
	{
		if (keys == halKeySavedKeys)
		{
			/* Exit - since no keys have changed */
			return;
		}
		/* Store the current keys for comparation next time */
		notify = 1;
	}
	else
	{
		/* Key interrupt handled here */
		notify = 1;
	}
	
	/* Store the current keys for comparation next time */
	halKeySavedKeys = keys;
	
	/* Invoke Callback if new keys were depressed */
	if (notify && (pHalKeyProcessFunction))
	{
		(pHalKeyProcessFunction) (keys, HAL_KEY_STATE_NORMAL);
	}
}


/**************************************************************************************************
 * @fn      cot_key_enter_sleep
 *
 * @brief  - Get called to enter sleep mode
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void cot_key_enter_sleep ( void )
{
	int32_t fd = 0;
	
	fd = open(PATH_KEY1,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_RELEASE,0);	
		close(fd);
	}

	fd = open(PATH_KEY2,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_RELEASE,0);
		close(fd);
	}
	
	fd = open(PATH_KEY3,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_RELEASE,0);
		close(fd);
	}
	
	fd = open(PATH_KEY4,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_RELEASE,0);
		close(fd);
	}
	
	fd = open(PATH_KEY5,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_RELEASE,0);
		close(fd);
	}
}

/**************************************************************************************************
 * @fn      cot_key_exit_sleep
 *
 * @brief   - Get called when sleep is over
 *
 * @param
 *
 * @return  - return saved keys
 **************************************************************************************************/
uint8_t cot_key_exit_sleep ( void )
{
	/* Wake up and read keys */
	int32_t fd = 0;
	
	fd = open(PATH_KEY1,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_ACTIVE,0);
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);		
		close(fd);
	}

	fd = open(PATH_KEY2,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_ACTIVE,0);
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		close(fd);
	}
	
	fd = open(PATH_KEY3,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_ACTIVE,0);
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		close(fd);
	}
	
	fd = open(PATH_KEY4,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_ACTIVE,0);
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		close(fd);
	}
	
	fd = open(PATH_KEY5,0);
	if( fd > 0 )	
	{	
		ioctl(fd,REQ_SET_GPIO_KEY_ACTIVE,0);
		ioctl(fd,REQ_SET_GPIO_KEY_INT_ENABLE,0);
		close(fd);
	}
	
	return 0;
}


/*********************************************************************
 * @fn      cot_mt_key_send_keys
 *
 * @brief   Send "Key Pressed" message to application.
 *
 * @param   keys  - keys that were pressed
 *          state - shifted
 *
 * @return  status
 *********************************************************************/
uint8_t cot_mt_key_send_keys( uint8_t keys, uint8_t state )
{
	mt_osal_msg_data_t *msgPtr;

    // Send the address to the task
    msgPtr = (mt_osal_msg_data_t *)osal_msg_allocate( sizeof(mt_osal_msg_data_t)+2);
    if ( msgPtr )
    {
		uint8_t index = 0;
		msgPtr->hdr.event = CMD_KEY_MSG;
		msgPtr->event_cmd = KEY_EVENT_PRESSED_CMD;
		msgPtr->msg = (uint8_t*)(msgPtr+1);
		msgPtr->msg[index++] = state;
		msgPtr->msg[index++] = keys;		
		msgPtr->msg_len = index;
		
		osal_msg_send(s_cot_mt_task_id,(uint8_t *)msgPtr);
    }
	
    return ( SUCCESS );

}

/**************************************************************************************************
 * @fn      cot_key1_isr
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void cot_key1_isr(void)
{
	int32_t fd = 0;
	fd = open(PATH_KEY1,0);
	if( fd <=0 )
		return;
	
	uint8_t key_value = 0;
	lseek(fd,GPIO_KEY1_PIN,SEEK_SET);
	read( fd,&key_value,1);
	close(fd);
	
	if(!key_value)
	{
		s_press_keys |= COT_MT_KEY_SW_1;
		mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+2);
		if (g_pMsg)
		{
			/* Fill up what we can */
			g_pMsg->hdr.event = CMD_KEY_MSG;
			g_pMsg->event_cmd = KEY_EVENT_PRESS_DEBOUNCE_CMD;
			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
			g_pMsg->msg[0] = COT_MT_KEY_SW_1;
			g_pMsg->msg[1] = HAL_KEY_DEBOUNCE_VALUE;
			g_pMsg->msg_len = 2;
			osal_msg_send(s_cot_mt_task_id,(uint8_t *)g_pMsg);
		}
	}
}

/**************************************************************************************************
 * @fn      cot_key2_isr
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void cot_key2_isr(void)
{
	int32_t fd = 0;
	fd = open(PATH_KEY2,0);
	if( fd <=0 )
		return;
	
	uint8_t key_value = 0;
	lseek(fd,GPIO_KEY2_PIN,SEEK_SET);
	read( fd,&key_value,1);
	close(fd);
	
	if(!key_value)
	{
		s_press_keys |= COT_MT_KEY_SW_2;
		mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+2);
		if (g_pMsg)
		{
			/* Fill up what we can */
			g_pMsg->hdr.event = CMD_KEY_MSG;
			g_pMsg->event_cmd = KEY_EVENT_PRESS_DEBOUNCE_CMD;
			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
			g_pMsg->msg[0] = COT_MT_KEY_SW_2;
			g_pMsg->msg[1] = HAL_KEY_DEBOUNCE_VALUE;
			g_pMsg->msg_len = 2;
			osal_msg_send(s_cot_mt_task_id,(uint8_t *)g_pMsg);
		}
	}
}

/**************************************************************************************************
 * @fn      cot_key3_isr
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void cot_key3_isr(void)
{
	int32_t fd = 0;
	fd = open(PATH_KEY3,0);
	if( fd <=0 )
		return;
	
	uint8_t key_value = 0;
	lseek(fd,GPIO_KEY3_PIN,SEEK_SET);
	read( fd,&key_value,1);
	close(fd);
	
	if(!key_value)
	{
		s_press_keys |= COT_MT_KEY_SW_3;
		mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+2);
		if (g_pMsg)
		{
			/* Fill up what we can */
			g_pMsg->hdr.event = CMD_KEY_MSG;
			g_pMsg->event_cmd = KEY_EVENT_PRESS_DEBOUNCE_CMD;
			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
			g_pMsg->msg[0] = COT_MT_KEY_SW_3;
			g_pMsg->msg[1] = HAL_KEY_DEBOUNCE_VALUE;
			g_pMsg->msg_len = 2;
			osal_msg_send(s_cot_mt_task_id,(uint8_t *)g_pMsg);
		}
	}
}

/**************************************************************************************************
 * @fn      cot_key4_isr
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void cot_key4_isr(void)
{
	int32_t fd = 0;
	fd = open(PATH_KEY4,0);
	if( fd <=0 )
		return;
	
	uint8_t key_value = 0;
	lseek(fd,GPIO_KEY4_PIN,SEEK_SET);
	read( fd,&key_value,1);
	close(fd);
	
	if(!key_value)
	{
		s_press_keys |= COT_MT_KEY_SW_4;
		mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+2);
		if (g_pMsg)
		{
			/* Fill up what we can */
			g_pMsg->hdr.event = CMD_KEY_MSG;
			g_pMsg->event_cmd = KEY_EVENT_PRESS_DEBOUNCE_CMD;
			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
			g_pMsg->msg[0] = COT_MT_KEY_SW_4;
			g_pMsg->msg[1] = HAL_KEY_DEBOUNCE_VALUE;
			g_pMsg->msg_len = 2;
			osal_msg_send(s_cot_mt_task_id,(uint8_t *)g_pMsg);
		}
	}
}

/**************************************************************************************************
 * @fn      cot_key5_isr
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void cot_key5_isr(void)
{
	int32_t fd = 0;
	fd = open(PATH_KEY5,0);
	if( fd <=0 )
		return;
	
	uint8_t key_value = 0;
	lseek(fd,GPIO_KEY5_PIN,SEEK_SET);
	read( fd,&key_value,1);
	close(fd);
	
	if(!key_value)
	{
		s_press_keys |= COT_MT_KEY_SW_5;
		mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+2);
		if (g_pMsg)
		{
			/* Fill up what we can */
			g_pMsg->hdr.event = CMD_KEY_MSG;
			g_pMsg->event_cmd = KEY_EVENT_PRESS_DEBOUNCE_CMD;
			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
			g_pMsg->msg[0] = COT_MT_KEY_SW_5;
			g_pMsg->msg[1] = HAL_KEY_DEBOUNCE_VALUE;
			g_pMsg->msg_len = 2;
			osal_msg_send(s_cot_mt_task_id,(uint8_t *)g_pMsg);
		}
	}
}




