/**
* @file         sample_app.cpp
* @brief        This is a brief description.
* @details      This is the detail description.
* @author       author
* @date     	date
* @version  	A001
* @par Copyright (c):  Copyright (c) 2017 by COTiot Instruments, Inc.
* @par History:
*   version: author, date, desc\n
*/

/*********************************************************************
 * INCLUDE
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "am_hal_systick.h"
#include "am_hal_gpio.h"
#include "am_util_delay.h"

#include "error.h"
#include "osal.h"
#include "hal_timers.h"
#include "osal_time.h"
#include "osal_tasks.h"
#include "osal_pwrmgr.h"
#include "gpio.h"
#include "log.h"
#include "data_package.h"
#include "rf_param_type_define.h"
#include "cot_mac.h"
#include "RfParamConfig.h"
#include "af.h"
#include "cot_mt.h"
#include "cot_mt_task.h"
#include "sample_app.h"
#include "cot_mt_oled.h"
#include "Sample_app.h"
#include "cot_mt_key.h"
#include "cot_mt_led.h"


/*********************************************************************
 * MACROS
 */
#define MSG(FORMAT, ARG...) /* message that is destined to the user */
#define MSG_DEBUG(FLAG, fmt, ...)                                                                         \
            do  {                                                                                         \
                if (FLAG)                                                                                 \
                {\
                    log_printf("%s:%d:%s(): " ,__FILE__, __LINE__, __FUNCTION__);\
					log_printf(fmt,##__VA_ARGS__);\
                }\
            } while (0)

/*********************************************************************
 * CONSTANTS
 */
// phy layer msg  Events
#define SMAPLE_MSG_TX_EVENT                              			0x0002
#define SMAPLE_MSG_TX_TIMEOUT_EVENT                      			0x0004
#define SMAPLE_MSG_RX_TIMEOUT_EVENT                           		0x0008
#define SMAPLE_MSG_RX_ERROR_EVENT                       			0x0010
#define SMAPLE_MSG_RNG_EVENT                          				0x0020
#define SMAPLE_MSG_RANGING_DONE_EVENT                         		0x0040
#define SMAPLE_MSG_RANGING_TIMEOUT_EVENT                            0x0080
#define SMAPLE_MSG_CONFIG_EVENT                                 	0x0100
#define SMAPLE_MSG_TX_DONE_EVENT                                 	0x0200
#define SMAPLE_MSG_RX_DONE_EVENT                                 	0x0400
#define SMAPLE_MSG_INIT_EVENT                                 		0x0800	
#define SMAPLE_MSG_SLEEP_EVENT                                 		0x1000




#define SX1280_CHIP 0x00
#define SX1281_CHIP 0x01


/*!
 * \brief sample application cmd
 */
#define COT_SYSTEM_CMD_H                    0x00

#define SAMPLE_APP_CMD_HANDLE               0x01
#define SAMPLE_APP_CMD_BROADCAST            0x02
#define SAMPLE_APP_CMD_SET_MULTICAST_ID     0x03
#define SMAPLE_APP_CMD_SET_WORK_PARRAM      0x04

#define SMAPLE_APP_CMD_START_RANG           0x20
#define SMAPLE_APP_CMD_SEND_PKG             0x21
#define SMAPLE_APP_CMD_SEND_PKG_LOWPOWER    0x22

#define SMAPLE_APP_CMD_PINGPONG             0x77


/*!
 * \brief cot_phy_payload_head_t;
 * it's len is 10 Byte((4byte src_dev_id)+(4byte dst_dev_id)+(1byte commad_h)+(1byte commad_l))
 */
#define COT_PHY_PAYLOAD_HEAD    10

#define STATUS_SUCCESS          0
#define STATUS_FAIL             1

#define COMFIRM_FRAME           1
#define UNCOMFIRM_FRAME         0

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
    uint8_t                     commad_h;                   //the high byte of commad
    uint8_t                     commad_l;                   //the low byte of commad
    uint8_t                     *data;                      //the payload data field
}cot_application_msg_t;

typedef struct
{
    uint16_t TX_count;
    uint16_t RX_count;
    uint16_t RX_SUC_count;
    bool     hold; //  true  pingpong stop
}cot_sample_pingpong_t;

typedef struct
{
    uint8_t         key1;     //key1 count
    uint8_t         key2;     //key2 count
    uint8_t         key3;     //key3 count
    uint8_t         key4;     //key4 count
    uint8_t         key5;     //key5 count

    int8_t          key2subkey5;     //key2 count sub key5 count
    int8_t          key4subkey3;     //key2 count sub key5 count
}cot_button_count_t;


/*********************************************************************
 * GLOBAL VARIABLES
 */



/*********************************************************************
 * EXTERNAL VARIABLES
 */



/*********************************************************************
 * EXTERNAL FUNCTIONS
 */



/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8_t s_sample_app_id = 0; ///<  sample application id for osal task id


static cot_mac_msg_t *s_cot_sample_msg_tx;


static ConfigData_t s_config_data;

static uint8_t g_sleep_pram = 0;


static  uint8_t s_soft_version[4]=	SOFT_VERSION;

static  uint8_t s_hard_ware_version[2]= HARD_WARE_VERSION;

/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * PROFILE CALLBACKS
 */



/*********************************************************************
 * PUBLIC FUNCTIONS
 */



/**
* @name: sample_app_init
* This funtion deal with the sample app init
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event   >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-04-29
*/
void sample_app_init( uint8_t taskId )
{
    s_sample_app_id = taskId ;

    osal_set_event( s_sample_app_id, SMAPLE_MSG_INIT_EVENT );
}


uint8_t get_sample_task_id( void )
{
    return s_sample_app_id;
}



/**
 * @name: sample_app_processevent
 * This function provide simple useing of the rf lib apis.
 * @param[in]   task_id -- the osal task id.
 *				 events -- the event of the task_id.
 * @param[out]  none
 * @retval  ERR_SUCCESS  0
 * @retval  do not deal the event   >0
 * @par identifier
 *      reserve
 * @par other
 *      none
 * @par ModifyBlog
 *      create by zhangjh on 2017-04-29
 */
uint16_t sample_app_processevent(uint8_t task_id, uint16_t events)
{
    uint16_t ret = ERR_SUCCESS ;

    if ( events & SYS_EVENT_MSG )
    {
        cot_mac_msg_t *samle_msg_ptr;

        while ( (samle_msg_ptr = (cot_mac_msg_t*)osal_msg_receive( task_id )) != NULL )
        {
            switch(samle_msg_ptr->hdr.event)
            {
                case CMD_MAC_LAYER:
                    {
						//mac_layer_system_message_process((cot_mac_msg_t *)samle_msg_ptr);
                    }
                    break;
                case CMD_KEY_MSG:
					{	
						//phy_button_process_osal_msg((mt_osal_msg_data_t *)samle_msg_ptr);
					}
                break;
            }
            // Release the OSAL message
            osal_msg_deallocate( (uint8_t *)samle_msg_ptr );
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if ( events & SMAPLE_MSG_INIT_EVENT )
    {
        DataFlashInit(&s_config_data);
		s_cot_sample_msg_tx = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) +239 );
		s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
		s_config_data.RFParamSettings.Entity = RF_SLAVE;
		get_rf_param(&s_config_data);

        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) );
        if( p_msg )
        {
            PHYParamSettings_t phy_param_setting ;
            sample_rf_param_get(s_config_data, &phy_param_setting);

            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_INIT_CMD;
            p_msg->src_task_id = task_id;
            p_msg->ModulationType = phy_param_setting.RF_RadioPacketTypes;
            p_msg->data = (uint8_t*)(p_msg+1);

            memcpy((char *)p_msg->data,(char *)&phy_param_setting,sizeof(phy_param_setting));
            p_msg->data_len = sizeof(phy_param_setting);
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }

        mt_osal_msg_data_t *p_mt_msg = (mt_osal_msg_data_t*)osal_msg_allocate( sizeof(mt_osal_msg_data_t) );
        if( p_mt_msg )
        {
            p_mt_msg->hdr.event = CMD_SERIAL_MSG;
			p_mt_msg->event_cmd = SERIAL_EVENT_INIT_CMD;
            p_mt_msg->task_id = s_sample_app_id;
            p_mt_msg->msg_len = 0;
            osal_msg_send(get_mt_task_id(),(uint8_t*)p_mt_msg);
        }

        osal_set_event(task_id, SMAPLE_MSG_CONFIG_EVENT);

        return (events ^ SMAPLE_MSG_INIT_EVENT);
    }


    if ( events & SMAPLE_MSG_CONFIG_EVENT )
    {
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) + sizeof(PHYParamSettings_t));
        if( p_msg )
        {
            PHYParamSettings_t phy_param_setting ;

			/*参数配置*/
			uint8_t buffer[RF_PARAM_BUFFER_SIZE] = {0};
			DataFlashMcuReadBuffer( 0, buffer, RF_PARAM_BUFFER_SIZE );

            sample_rf_param_get(s_config_data, &phy_param_setting);

            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_CONFIG_CMD;
            p_msg->src_task_id = task_id;
			p_msg->data = (uint8_t *)(p_msg + 1);
            p_msg->ModulationType = phy_param_setting.RF_RadioPacketTypes;


            memcpy((char *)p_msg->data,(char *)&phy_param_setting,sizeof(phy_param_setting));
            p_msg->data_len = sizeof(phy_param_setting);
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);

			osal_start_timerEx(task_id,SMAPLE_MSG_SLEEP_EVENT,1000);
		}
			
        return (events ^ SMAPLE_MSG_CONFIG_EVENT);
    }


    if(events & SMAPLE_MSG_TX_EVENT) // 发送数据
    {
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t)+ 239 );
        if( p_msg )
        {
			p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_TX_CMD;
            p_msg->src_task_id = task_id;
			p_msg->pkg_hdr.Bits.frame_pending = 0;
			p_msg->dst_addr = s_cot_sample_msg_tx->dst_addr;
			p_msg->pkg_hdr.Bits.MType = UNCONFIRM;
			p_msg->pkg_hdr.Bits.ack_request = 1;
			p_msg->rssi_water_leve = -80; //侦测阈值
            p_msg->data = (uint8_t *)(p_msg+1);
			
			memcpy((char *)p_msg->data,s_cot_sample_msg_tx->data,s_cot_sample_msg_tx->data_len);
            p_msg->data_len = s_cot_sample_msg_tx->data_len;
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }

        return (events ^ SMAPLE_MSG_TX_EVENT);
    }

    if(events & SMAPLE_MSG_RX_EVENT) // 使能接收数据
    {
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) );
        if( p_msg )
        {
            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_RX_CMD;
            p_msg->src_task_id = task_id;
            p_msg->irq_type.Value = RF_IRQ_RX_DONE | RF_IRQ_CRC_ERROR | RF_IRQ_RX_TX_TIMEOUT;
            p_msg->tick_timeout.Step = RF_RADIO_TICK_SIZE_1000_US;
            p_msg->tick_timeout.NbSteps = 0xFFFF;
            p_msg->data_len = 0;
            p_msg->data = NULL;

            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }

        osal_pwrmgr_task_state(task_id, PWRMGR_HOLD);

        return (events ^ SMAPLE_MSG_RX_EVENT);
    }


    if(events & SMAPLE_MSG_RNG_EVENT) // 使能测距
    {
        PHYParamSettings_t phy_param_setting ;
        sample_rf_param_get(s_config_data, &phy_param_setting);

        cot_ranging_result_t cot_ranging_res;
        memset( (char *)&cot_ranging_res, 0x0, sizeof(cot_ranging_result_t) );
        cot_ranging(&phy_param_setting, &cot_ranging_res);
       
        return (events ^ SMAPLE_MSG_RNG_EVENT);
   }

    if ( events & SMAPLE_MSG_TX_DONE_EVENT )
	{
		
        return (events ^ SMAPLE_MSG_TX_DONE_EVENT);
    }

    if ( events & SMAPLE_MSG_TX_TIMEOUT_EVENT )
    {
        osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
        return (events ^ SMAPLE_MSG_TX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_TIMEOUT_EVENT )
    {
		osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
		return (events ^ SMAPLE_MSG_RX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_ERROR_EVENT )
    {
		
		osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
		
        return (events ^ SMAPLE_MSG_RX_ERROR_EVENT);
    }

    if ( events & SMAPLE_MSG_SLEEP_EVENT )
    {
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) + 1 );
        if( p_msg )
        {
            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_MSG_SLEEP;
            p_msg->src_task_id = task_id;
            p_msg->data_len = 1;
			p_msg->data = (uint8_t*)(p_msg+1);
            p_msg->data[0] = g_sleep_pram;
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }
        pwrmgr_attribute.pwrmgr_device = PWRMGR_BATTERY;
        return (events ^ SMAPLE_MSG_SLEEP_EVENT);
    }


    return ret;
}


