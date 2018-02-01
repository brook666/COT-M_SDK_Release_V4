/*********************************************************************
    Filename:       cot_mt_led.h
    Revised:        $Date: 2017-06-26 22:05:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the cot led API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/

/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/
#include "osal.h"
#include "osal_tick.h"
#include "vfs.h"
#include "gpio.h"
#include "log.h"
#include "cot_mt.h"
#include "cot_mt_led.h"


/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/


/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/
#define HAL_LED 	TRUE
#define BLINK_LEDS

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/
/* LED control structure */
typedef struct {
  uint8_t 	mode;       /* Operation mode */
  uint8_t 	todo;       /* Blink cycles left */
  uint8_t 	onPct;      /* On cycle percentage */
  uint16_t	time;      	/* On/off cycle time (msec) */
  uint32_t 	next;      	/* Time for next change */
} cot_mt_led_control_t;

typedef struct
{
  cot_mt_led_control_t 	cot_mt_led_control_table[COT_MT_LED_DEFAULT_MAX_LEDS];
  uint8_t           	sleep_active;
} cot_mt_led_status_t;


/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
static uint8_t cot_mt_led_state;              // LED state at last set/clr/blink update

#if HAL_LED == TRUE
static uint8_t HalSleepLedState;         // LED state at last set/clr/blink update
static uint8_t preBlinkState;            // Original State before going to blink mode
                                       // bit 0, 1, 2, 3 represent led 0, 1, 2, 3
#endif

#ifdef BLINK_LEDS
  static cot_mt_led_status_t cot_mt_led_status_control;
#endif

static uint8_t s_cot_mt_task_id;

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/




/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      cot_mt_led_init
 *
 * @brief   Initialize LED Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 ***************************************************************************************************/
void cot_mt_led_init ( uint8_t task_id )
{
	s_cot_mt_task_id = task_id;
	
#if (HAL_LED == TRUE)
	/* Initialize all LEDs to OFF */
	cot_mt_led_set (COT_MT_LED_ALL, COT_MT_LED_MODE_OFF);
#endif /* HAL_LED */

    //cot_mt_led_set (COT_MT_LED_1, COT_MT_LED_MODE_ON); // power led
    
#ifdef BLINK_LEDS
	/* Initialize sleep_active to FALSE */
	cot_mt_led_status_control.sleep_active = false;
#endif
	
}

/***************************************************************************************************
 * @fn      cot_mt_led_set
 *
 * @brief   Tun ON/OFF/TOGGLE given LEDs
 *
 * @param   led - bit mask value of leds to be turned ON/OFF/TOGGLE
 *          mode - BLINK, FLASH, TOGGLE, ON, OFF
 * @return  None
 ***************************************************************************************************/
uint8_t cot_mt_led_set (uint8_t leds, uint8_t mode)
{

#if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
	uint8_t led;
	cot_mt_led_control_t *sts;

	switch (mode)
	{
		case COT_MT_LED_MODE_BLINK:
			/* Default blink, 1 time, D% duty cycle */
			cot_mt_led_blink (leds, 1, COT_MT_LED_DEFAULT_DUTY_CYCLE, COT_MT_LED_DEFAULT_FLASH_TIME);
			break;

		case COT_MT_LED_MODE_FLASH:
			/* Default flash, N times, D% duty cycle */
			cot_mt_led_blink (leds, COT_MT_LED_DEFAULT_FLASH_COUNT, COT_MT_LED_DEFAULT_DUTY_CYCLE, COT_MT_LED_DEFAULT_FLASH_TIME);
			break;

		case COT_MT_LED_MODE_ON:
		case COT_MT_LED_MODE_OFF:
		case COT_MT_LED_MODE_TOGGLE:

			led = COT_MT_LED_1;
			leds &= COT_MT_LED_ALL;
			sts = cot_mt_led_status_control.cot_mt_led_control_table;

			while (leds)
			{
				if (leds & led)
				{
					if (mode != COT_MT_LED_MODE_TOGGLE)
					{
						sts->mode = mode;  /* ON or OFF */
					}
					else
					{
						sts->mode ^= COT_MT_LED_MODE_ON;  /* Toggle */
					}

					cot_led_on_off (led, sts->mode);
					leds ^= led;
				}

				led <<= 1;
				sts++;
			}
			break;

		default:
			break;
	}

#elif (HAL_LED == TRUE)
	cot_led_on_off(leds, mode);
#else
	// HAL LED is disabled, suppress unused argument warnings
	(void) leds;
	(void) mode;
#endif /* BLINK_LEDS && HAL_LED   */

	return ( cot_mt_led_state );

}

/***************************************************************************************************
 * @fn      cot_mt_led_blink
 *
 * @brief   Blink the leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *          percent    - the percentage in each period where the led
 *                       will be on
 *          period     - length of each cycle in milliseconds
 *
 * @return  None
 ***************************************************************************************************/
void cot_mt_led_blink (uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period)
{
#if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
	uint8_t led;
	cot_mt_led_control_t *sts;

	if (leds && percent && period)
	{
		if (percent < 100)
		{
			led = COT_MT_LED_1;
			leds &= COT_MT_LED_ALL;
			sts = cot_mt_led_status_control.cot_mt_led_control_table;

			while (leds)
			{
				if (leds & led)
				{
					/* Store the current state of the led before going to blinking if not already blinking */
					if(sts->mode < COT_MT_LED_MODE_BLINK )
					preBlinkState |= (led & cot_mt_led_state);

					sts->mode  = COT_MT_LED_MODE_OFF;                   	/* Stop previous blink */
					sts->time  = period;                              		/* Time for one on/off cycle */
					sts->onPct = percent;                             		/* % of cycle LED is on */
					sts->todo  = numBlinks;                           		/* Number of blink cycles */
					if (!numBlinks) sts->mode |= COT_MT_LED_MODE_FLASH;  	/* Continuous */
					sts->next = osal_get_system_tick();                		 	/* Start now */
					sts->mode |= COT_MT_LED_MODE_BLINK;                  	/* Enable blinking */
					leds ^= led;
				}
				led <<= 1;
				sts++;
			}
			// Cancel any overlapping timer for blink events
			
			//osal_stop_timerEx(s_cot_mt_task_id, COT_MT_LED_BLINK_EVENT);
			mt_osal_msg_data_t *msgPtr1;
			msgPtr1 = (mt_osal_msg_data_t *)osal_msg_allocate( sizeof(mt_osal_msg_data_t)+2);
			if ( msgPtr1 )
			{
				msgPtr1->hdr.event = CMD_LED_MSG;
				msgPtr1->event_cmd = STOP_COT_MT_LED_BLINK_CMD;
				msgPtr1->msg = (uint8_t*)(msgPtr1+1);
				msgPtr1->msg_len = 0;
				
				osal_msg_send(s_cot_mt_task_id,(uint8_t *)msgPtr1);
			}
			
			#if 0
			//osal_set_event (s_cot_mt_task_id, COT_MT_LED_BLINK_EVENT);
			msgPtr2 = (mt_osal_msg_data_t *)osal_msg_allocate( sizeof(mt_osal_msg_data_t)+2);
			if ( msgPtr2 )
			{
				msgPtr2->hdr.event = CMD_LED_MSG;
				msgPtr2->event_cmd = START_COT_MT_LED_BLINK_CMD;
				msgPtr2->msg = (uint8_t*)(msgPtr2+1);
				msgPtr2->msg_len = 0;
				
				osal_msg_send(s_cot_mt_task_id,(uint8_t *)msgPtr2);
			}
			#endif
		}
		else
		{
			cot_mt_led_set (leds, COT_MT_LED_MODE_ON);                    /* >= 100%, turn on */
		}
	}
	else
	{
		cot_mt_led_set (leds, COT_MT_LED_MODE_OFF);                     /* No on time, turn off */
	}
	
#elif (HAL_LED == TRUE)
	percent = (leds & cot_mt_led_state) ? COT_MT_LED_MODE_OFF : COT_MT_LED_MODE_ON;
	cot_led_on_off (leds, percent);                              /* Toggle */
#else
	// HAL LED is disabled, suppress unused argument warnings
	(void) leds;
	(void) numBlinks;
	(void) percent;
	(void) period;
#endif /* BLINK_LEDS && HAL_LED */

}


/***************************************************************************************************
 * @fn      cot_mt_led_update
 *
 * @brief   Update leds to work with blink
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void cot_mt_led_update (void)
{
	uint8_t led;
	uint8_t pct;
	uint8_t leds;
	cot_mt_led_control_t *sts;
	uint32_t time;
	uint16_t next;
	uint16_t wait;

	next = 0;
	led  = COT_MT_LED_1;
	leds = COT_MT_LED_ALL;
	sts = cot_mt_led_status_control.cot_mt_led_control_table;

	/* Check if sleep is active or not */
	if (!cot_mt_led_status_control.sleep_active)
	{
		while (leds)
		{
			if (leds & led)
			{
				if (sts->mode & COT_MT_LED_MODE_BLINK)
				{
					time = osal_get_system_tick();
					if (time >= sts->next)
					{
						if (sts->mode & COT_MT_LED_MODE_ON)
						{
							pct = 100 - sts->onPct;               /* Percentage of cycle for off */
							sts->mode &= ~COT_MT_LED_MODE_ON;        /* Say it's not on */
							cot_led_on_off (led, COT_MT_LED_MODE_OFF);  /* Turn it off */

							if (!(sts->mode & COT_MT_LED_MODE_FLASH))
							{
								sts->todo--;                        /* Not continuous, reduce count */
							}
						}            
						else if ( (!sts->todo) && !(sts->mode & COT_MT_LED_MODE_FLASH) )
						{
							sts->mode ^= COT_MT_LED_MODE_BLINK;      /* No more blinks */  
						}            
						else
						{
							pct = sts->onPct;                     /* Percentage of cycle for on */
							sts->mode |= COT_MT_LED_MODE_ON;         /* Say it's on */
							cot_led_on_off (led, COT_MT_LED_MODE_ON);   /* Turn it on */
						}
						if (sts->mode & COT_MT_LED_MODE_BLINK)
						{
							wait = (((uint32_t)pct * (uint32_t)sts->time) / 100);
							sts->next = time + wait;
						}
						else
						{
							/* no more blink, no more wait */
							wait = 0;
							/* After blinking, set the LED back to the state before it blinks */
							cot_mt_led_set (led, ((preBlinkState & led)!=0)?COT_MT_LED_MODE_ON:COT_MT_LED_MODE_OFF);
							/* Clear the saved bit */
							preBlinkState &= (led ^ 0xFF);
						}
					}
					else
					{
						wait = sts->next - time;  /* Time left */
					}

					if (!next || ( wait && (wait < next) ))
					{
						next = wait;
					}
				}
				leds ^= led;
			}
			led <<= 1;
			sts++;
		}

		if (next)
		{
			//osal_start_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT, next);   /* Schedule event */
			mt_osal_msg_data_t *msgPtr;
			msgPtr = (mt_osal_msg_data_t *)osal_msg_allocate( sizeof(mt_osal_msg_data_t)+2);
			if ( msgPtr )
			{
				uint8_t index = 0;
				msgPtr->hdr.event = CMD_LED_MSG;
				msgPtr->event_cmd = START_COT_MT_LED_BLINK_IN_TIMEOUT_CMD;
				msgPtr->msg = (uint8_t*)(msgPtr+1);
				msgPtr->msg[index++] = (next&0xFF00) >> 8;
				msgPtr->msg[index++] = (next&0xFF);
				msgPtr->msg_len = index;
				
				osal_msg_send(s_cot_mt_task_id,(uint8_t *)msgPtr);
			}
		}
	}
}

/***************************************************************************************************
 * @fn      cot_led_on_off
 *
 * @brief   Turns specified LED ON or OFF
 *
 * @param   leds - LED bit mask
 *          mode - LED_ON,LED_OFF,
 *
 * @return  none
 ***************************************************************************************************/
void cot_led_on_off (uint8_t leds, uint8_t mode)
{
	int32_t fd = 0;
	uint8_t value = 1;
	
	if (leds & COT_MT_LED_1)
	{
		if (mode == COT_MT_LED_MODE_ON)
		{
			value = LED_ON;
			fd = open(PATH_LED1,0);
			if(fd > 0)
			{
				write(fd,&value,1);
				close(fd);
			}
		}
		else
		{
			value = LED_OFF;
			fd = open(PATH_LED1,0);
			if(fd > 0)
			{
				//ioctl(fd,REQ_SET_GPIO_LED_RELEASE,0);
				write(fd,&value,1);
				close(fd);
			}
		}
	}

	if (leds & COT_MT_LED_2)
	{
		if (mode == COT_MT_LED_MODE_ON)
		{
			value = LED_ON;
			fd = open(PATH_LED2,0);
			if(fd > 0)
			{	
				write(fd,&value,1);
				close(fd);
			}
		}
		else
		{
			value = LED_OFF;
			fd = open(PATH_LED2,0);
			if(fd > 0)
			{
				write(fd,&value,1);
				close(fd);
			}
		}
	}

	if (leds & COT_MT_LED_3)
	{
		if (mode == COT_MT_LED_MODE_ON)
		{
			value = LED_ON;
			fd = open(PATH_LED3,0);
			if(fd > 0)
			{
				write(fd,&value,1);
				close(fd);
			}
		}
		else
		{
			value = LED_OFF;
			fd = open(PATH_LED3,0);
			if(fd > 0)
			{
				write(fd,&value,1);
				close(fd);
			}
		}
	}


	/* Remember current state */
	if (mode)
	{
		cot_mt_led_state |= leds;
	}
	else
	{
		cot_mt_led_state &= (leds ^ 0xFF);
	}
}


/***************************************************************************************************
 * @fn      HalGetLedState
 *
 * @brief   Dim LED2 - Dim (set level) of LED2
 *
 * @param   none
 *
 * @return  led state
 ***************************************************************************************************/
uint8_t cot_mt_led_get_state ()
{
#if (HAL_LED == TRUE)
  return cot_mt_led_state;
#else
  return 0;
#endif
}

/***************************************************************************************************
 * @fn      cot_mt_led_enter_sleep
 *
 * @brief   Store current LEDs state before sleep
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void cot_mt_led_enter_sleep( void )
{
#ifdef BLINK_LEDS
  /* Sleep ON */
  cot_mt_led_status_control.sleep_active = TRUE;
#endif /* BLINK_LEDS */

#if (HAL_LED == TRUE)
  /* Save the state of each led */
  HalSleepLedState = 0;
  //HalSleepLedState |= HAL_STATE_LED1();
  //HalSleepLedState |= HAL_STATE_LED2() << 1;
  //HalSleepLedState |= HAL_STATE_LED3() << 2;

  /* TURN OFF all LEDs to save power */
  cot_led_on_off (COT_MT_LED_ALL, COT_MT_LED_MODE_OFF);
#endif /* HAL_LED */

}

/***************************************************************************************************
 * @fn      cot_mt_led_exit_sleep
 *
 * @brief   Restore current LEDs state after sleep
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void cot_mt_led_exit_sleep( void )
{
#if (HAL_LED == TRUE)
  /* Load back the saved state */
  cot_led_on_off(HalSleepLedState, COT_MT_LED_MODE_ON);

  /* Restart - This takes care BLINKING LEDS */
  cot_mt_led_update();
#endif /* HAL_LED */

#ifdef BLINK_LEDS
  /* Sleep OFF */
  cot_mt_led_status_control.sleep_active = FALSE;
#endif /* BLINK_LEDS */
}
