/**
* @file         cot_mt_task.c
* @brief        monitor of test.
* @details      This is the detail description.
* @author       zhangjh
* @date     	2017-06-01
* @version  	V0.1
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <string.h>
#include "osal.h"
#include "osal_time.h"
#include "osal_memory.h"
#include "uart.h"
#include "log.h"
#include "cot_mt.h"
#include "cot_mt_uart.h"
#include "cot_mt_task.h"
#include "cot_mt_key.h"
#include "cot_mt_led.h"

/***************************************************************************************************
 * CONST
 ***************************************************************************************************/
#define COT_MT_LED_EVENT			0x1000
#define COT_MT_KEY_EVENT			0x2000
#define COT_MT_RECV_UART_EVENT  	0x4000

/***************************************************************************************************
 * GLOBALS
 ***************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLE
 ***************************************************************************************************/
static uint8_t 	s_mt_task_id;
static uint8_t	s_sample_task_id;

static uint8_t 	s_keys = 0;

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/

static void cot_mt_process_incoming_command( void *msg );


/***************************************************************************************************
 * @fn      MT_ProcessIncomingCommand
 *
 * @brief
 *
 *   Process Event Messages.
 *
 * @param   *msg - pointer to event message
 *
 * @return
 ***************************************************************************************************/
static void cot_mt_process_incoming_command( void *msg )
{
	mt_osal_msg_data_t *msg_ptr = (mt_osal_msg_data_t *)msg;

    /* Use the first byte of the message as the command ID */
    switch ( msg_ptr->hdr.event )
    {
        case CMD_SERIAL_MSG:
            {
                switch( msg_ptr->event_cmd )
                {
                    case SERIAL_EVENT_INIT_CMD:
                        {
                            s_sample_task_id = msg_ptr->task_id;
                            osal_set_event(s_mt_task_id,COT_MT_RECV_UART_EVENT);
                        }
                        break;

                    case SERIAL_EVENT_RECV_CMD:
                        {
                            mt_osal_msg_data_t *msg_send_ptr = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+msg_ptr->msg_len);
                            if( msg_send_ptr )
                            {
                                msg_send_ptr->hdr.event = CMD_SERIAL_MSG;
                                msg_send_ptr->task_id = s_mt_task_id;
                                msg_send_ptr->event_cmd = SERIAL_EVENT_RECV_CMD;
                                msg_send_ptr->msg_len = msg_ptr->msg_len;
                                msg_send_ptr->msg = (uint8_t*)(msg_send_ptr+1);
                                memcpy(msg_send_ptr->msg,msg_ptr->msg,msg_ptr->msg_len);
                                osal_msg_send(s_sample_task_id,(uint8_t*)msg_send_ptr);
                            }
                        }
                        break;

                    case SERIAL_EVENT_SEND_CMD:
                        {
                            cot_mt_uart_transport_out(msg_ptr);
                        }
                        break;

                    case SERIAL_EVENT_RESPONSE_CMD:
                        {
                            cot_mt_uart_transport_out(msg_ptr);
                        }
                        break;
                    case SERIAL_EVENT_DISABLE_CMD:
                    {
                        cot_mt_uart_disable();
                    }
                    break;
                    default:
                        break;
                }
            }
			break;
		case CMD_KEY_MSG:
			{
				switch(msg_ptr->event_cmd)
				{
					case KEY_EVENT_PRESS_DEBOUNCE_CMD:
						{
							s_keys |= msg_ptr->msg[0];
							uint8_t debounce_timeout = msg_ptr->msg[1];

							osal_start_timerEx(s_mt_task_id,COT_MT_KEY_EVENT,debounce_timeout);
						}
						break;

					case KEY_EVENT_PRESSED_CMD:
						{
							uint8_t keys = 0;
							keys = msg_ptr->msg[1];
							if( keys & COT_MT_KEY_SW_1 )
							{
								//cot_mt_led_set(COT_MT_LED_ALL,COT_MT_LED_MODE_BLINK);
								cot_mt_led_blink(COT_MT_LED_1,1,50,5);
								{
                                    mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+ 3);
                            		if (g_pMsg)
                            		{
                            			/* Fill up what we can */
                            			g_pMsg->hdr.event = CMD_KEY_MSG;
                                        g_pMsg->event_cmd = KEY_EVENT_CMD_KEY1_PRESSED;
                            			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
                            			g_pMsg->msg_len = 1;
                            			g_pMsg->msg[0] = 0;
                            			osal_msg_send(s_sample_task_id,(uint8_t *)g_pMsg);
                                    }
                                }
								log_printf("KEY1 Pressed\n");
							}

							if( keys & COT_MT_KEY_SW_2 )
							{
								//cot_led_on_off(COT_MT_LED_1,COT_MT_LED_MODE_OFF);
								//cot_led_on_off(COT_MT_LED_2,COT_MT_LED_MODE_OFF);
								//cot_led_on_off(COT_MT_LED_3,COT_MT_LED_MODE_OFF);
								cot_mt_led_blink(COT_MT_LED_1,1,50,5);
								log_printf("KEY2 Pressed\n");
                                {
                                    mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+ 3);
                            		if (g_pMsg)
                            		{
                            			/* Fill up what we can */
                            			g_pMsg->hdr.event = CMD_KEY_MSG;
                                        g_pMsg->event_cmd = KEY_EVENT_CMD_KEY2_PRESSED;
                            			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
                            			g_pMsg->msg_len = 1;
                            			g_pMsg->msg[0] = 0;
                            			osal_msg_send(s_sample_task_id,(uint8_t *)g_pMsg);
                                    }
                                }
							}

							if( keys & COT_MT_KEY_SW_3 )
							{
								//cot_mt_led_set(COT_MT_LED_ALL,COT_MT_LED_MODE_FLASH);
								//cot_mt_led_blink(COT_MT_LED_ALL,5,5,100);
								cot_mt_led_blink(COT_MT_LED_1,1,50,5);
								log_printf("KEY3 Pressed\n");
                                {
                                    mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+ 3);
                            		if (g_pMsg)
                            		{
                            			/* Fill up what we can */
                            			g_pMsg->hdr.event = CMD_KEY_MSG;
                                        g_pMsg->event_cmd = KEY_EVENT_CMD_KEY3_PRESSED;
                            			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
                            			g_pMsg->msg_len = 1;
                            			g_pMsg->msg[0] = 0;
                            			osal_msg_send(s_sample_task_id,(uint8_t *)g_pMsg);
                                    }
                                }
							}

							if( keys & COT_MT_KEY_SW_4 )
							{
								//cot_mt_led_set(COT_MT_LED_ALL,COT_MT_LED_MODE_TOGGLE);
								//cot_mt_led_blink(COT_MT_LED_ALL,5,70,100);
								cot_mt_led_blink(COT_MT_LED_1,1,50,5);
								log_printf("KEY4 Pressed\n");
                                {
                                    mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+ 3);
                            		if (g_pMsg)
                            		{
                            			/* Fill up what we can */
                            			g_pMsg->hdr.event = CMD_KEY_MSG;
                                        g_pMsg->event_cmd = KEY_EVENT_CMD_KEY4_PRESSED;
                            			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
                            			g_pMsg->msg_len = 1;
                            			g_pMsg->msg[0] = 0;
                            			osal_msg_send(s_sample_task_id,(uint8_t *)g_pMsg);
                                    }
                                }
							}

							if( keys & COT_MT_KEY_SW_5 )
							{
								//cot_led_on_off(COT_MT_LED_1,COT_MT_LED_MODE_ON);
								//cot_led_on_off(COT_MT_LED_2,COT_MT_LED_MODE_ON);
								//cot_led_on_off(COT_MT_LED_3,COT_MT_LED_MODE_ON);
								cot_mt_led_blink(COT_MT_LED_1,1,50,5);
								log_printf("KEY5 Pressed\n");
                                {
                                    mt_osal_msg_data_t *g_pMsg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+ 3);
                            		if (g_pMsg)
                            		{
                            			/* Fill up what we can */
                            			g_pMsg->hdr.event = CMD_KEY_MSG;
                                        g_pMsg->event_cmd = KEY_EVENT_CMD_KEY5_PRESSED;
                            			g_pMsg->msg = (uint8_t*)(g_pMsg+1);
                            			g_pMsg->msg_len = 1;
                            			g_pMsg->msg[0] = 0;
                            			osal_msg_send(s_sample_task_id,(uint8_t *)g_pMsg);
                                    }
                                }
							}
						}
						break;

					case KEY_EVENT_DETECT_INT_ENABLE_CMD:
						{
							osal_stop_timerEx( s_mt_task_id, COT_MT_KEY_EVENT); /* Cancel polling if active */
						}
						break;

					default:
						break;
				}
			}
			break;

		case CMD_LED_MSG:
			{
				switch(msg_ptr->event_cmd)
				{
					case STOP_COT_MT_LED_BLINK_CMD:
						{
							osal_stop_timerEx(s_mt_task_id, COT_MT_LED_EVENT);
							osal_set_event (s_mt_task_id, COT_MT_LED_EVENT);
						}
						break;
					case START_COT_MT_LED_BLINK_CMD:
						{
							osal_set_event (s_mt_task_id, COT_MT_LED_EVENT);
						}
						break;
					case START_COT_MT_LED_BLINK_IN_TIMEOUT_CMD:
						{
							uint16_t next = 0;
							next = (8<<(msg_ptr->msg[0])) + msg_ptr->msg[1];
							osal_start_timerEx(s_mt_task_id, COT_MT_LED_EVENT, next);
						}
						break;

					default:
						break;
				}
			}
			break;

        default:
        break;
    }
}


/***************************************************************************************************
 * PUBLIC FUNCTIONS
 ***************************************************************************************************/


/***************************************************************************************************
 * @fn      mt_task_init
 *
 * @brief  MonitorTest Task Initialization.  This function is put into the
 *         task table.
 *
 * @param   task_id - task ID of the MT Task
 *
 * @return  void
 ***************************************************************************************************/
void mt_task_init(uint8_t task_id)
{
    s_mt_task_id = task_id;
	cot_mt_key_init(s_mt_task_id);
	cot_mt_led_init(s_mt_task_id);
    cot_mt_uart_enable();
}

uint8_t get_mt_task_id( void )
{
	return s_mt_task_id;
}

/**************************************************************************************************
 * @fn      cot_mt_process_event
 *
 * @brief   MonitorTest Task Event Processor.  This task is put into the task table.
 *
 * @param   task_id - task ID of the MT Task
 * @param   events - event(s) for the MT Task
 *
 * @return  Bit mask of the unprocessed MT Task events.
 **************************************************************************************************/
uint16_t cot_mt_process_event(uint8_t task_id, uint16_t events)
{
	if( events & COT_MT_LED_EVENT )
	{
		cot_mt_led_update();

		return (events ^ COT_MT_LED_EVENT);
	}

	if( events & COT_MT_KEY_EVENT )
	{
		/* Check for keys */
		cot_mt_key_poll();

		/* if interrupt disabled, do next polling */
		if( !get_cot_mt_key_int_enadble_status() )
		{
			osal_start_timerEx(task_id,COT_MT_KEY_EVENT,100);
		}
		return (events ^ COT_MT_KEY_EVENT);
	}

    if( events & COT_MT_RECV_UART_EVENT )
    {
        cot_mt_uart_data_process(task_id,events);

        osal_start_timerEx( task_id, COT_MT_RECV_UART_EVENT,100);

        return (events ^ COT_MT_RECV_UART_EVENT);
    }

    /* Could be multiple events, so switch won't work */
    if ( events & SYS_EVENT_MSG )
    {
        uint8_t *msg_ptr = osal_msg_receive(task_id);

        if (msg_ptr != NULL)
        {
            cot_mt_process_incoming_command((void *)msg_ptr);
            osal_msg_deallocate(msg_ptr);
        }

        /* Return unproccessed events */
        return (events ^ SYS_EVENT_MSG);
    }

	/* Discard or make more handlers */
	return 0;

} /* MT_ProcessEvent() */



