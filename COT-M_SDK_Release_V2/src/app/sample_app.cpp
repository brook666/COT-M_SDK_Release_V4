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

//static cot_sample_msg_t s_cot_sample_msg_rx;

static cot_mac_msg_t *s_cot_sample_msg_tx;

//static af_serial_incoming_msg_packet_t af_serial_incoming_msg;


//static  uint8_t s_soft_version[4]={1,0,0,1};

//static  uint8_t s_hard_ware_version[2]={1,0};

static  uint8_t seq_no;

static ConfigData_t s_config_data;

static uint8_t g_sleep_pram = 0;

static cot_sample_pingpong_t s_cot_sample_pingpong;

static cot_button_count_t s_cot_button_count;

static FunctionalMode g_functional_mode = SLAVE_RX;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void pingpong_parameter_show(int8_t rssi,double distance,int8_t valid_count , int32_t request_count);


/**
* @name: cot_sample_msg_handle
* This
* @param[in]   sample_msg -- sample message.
* @param[out]  none.
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-18
*/
static void cot_sample_msg_handle( cot_mac_msg_t *sample_msg_packet );


static void cot_mac_layer_system_msg_process( cot_mac_msg_t *p_msg );

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
    s_cot_sample_pingpong.hold = false; //true  pingpong stop
}


uint8_t get_sample_task_id( void )
{
    return s_sample_app_id;
}


void sample_send_pingpong(char *ping,uint32_t dst_dev_id,uint16_t data_len)
{
    uint8_t index = 0 ;

	s_cot_sample_msg_tx->dst_addr = dst_dev_id;
	s_cot_sample_msg_tx->data = (uint8_t *)(s_cot_sample_msg_tx + 1);
	
	#if 0
	cot_application_msg_t *application_data = (cot_application_msg_t*)osal_msg_allocate( sizeof(cot_application_msg_t) + 239);;
	application_data->data = (uint8_t *)(application_data + 1);
	
	application_data->commad_h = COT_SYSTEM_CMD_H;
	application_data->commad_l = SMAPLE_APP_CMD_PINGPONG;
	index += 2;
	
	memcpy(application_data->data,ping,data_len);
	index += data_len;
	
	//memcpy(&application_data->data[data_len],&s_cot_sample_pingpong.TX_count,2);
	application_data->data[index++] = (s_cot_sample_pingpong.TX_count & 0xff00)>>8;
	application_data->data[index++] = (s_cot_sample_pingpong.TX_count & 0xff);
	
	memcpy((uint8_t*)s_cot_sample_msg_tx->data,(uint8_t*)application_data,index);
	#endif
	s_cot_sample_msg_tx->data[index++] = COT_SYSTEM_CMD_H;
	s_cot_sample_msg_tx->data[index++] = SMAPLE_APP_CMD_PINGPONG;
	memcpy(s_cot_sample_msg_tx->data+index,ping,data_len);
	index += data_len;
	s_cot_sample_msg_tx->data[index++] = (s_cot_sample_pingpong.TX_count & 0xff00)>>8;
	s_cot_sample_msg_tx->data[index++] = (s_cot_sample_pingpong.TX_count & 0xff);
	
	s_cot_sample_msg_tx->data_len = index;
	cot_mt_led_blink(COT_MT_LED_2,1,50,3);
	
	osal_start_timerEx(get_sample_task_id(), SMAPLE_MSG_TX_EVENT, 500); // 延时发送
}

/**
* @name: cot_sample_msg_handle
* This
* @param[in]   sample_msg -- sample message.
* @param[out]  sample_msg -- sample message.
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-18
*/
static void cot_sample_msg_handle( cot_mac_msg_t *sample_msg_packet )
{
	cot_application_msg_t *sample_msg = (cot_application_msg_t*)sample_msg_packet->data;
	uint8_t cmd_h;
	cmd_h = sample_msg->commad_h;
	switch( cmd_h )
	{
		case COT_SYSTEM_CMD_H:
			{
				uint8_t cmd_l = sample_msg->commad_l;
				switch(cmd_l)
				{
                    case SMAPLE_APP_CMD_PINGPONG:
                    {
                        if(s_cot_sample_pingpong.hold == false)
                        {
							// 计数
							s_cot_sample_pingpong.RX_count = ((uint16_t)(sample_msg_packet->data[6]) << 8)  \
															  + (sample_msg_packet->data[7]);
							s_cot_sample_pingpong.RX_SUC_count ++;
							osal_set_event(get_sample_task_id(),SMAPLE_MSG_RX_EVENT);
							pingpong_parameter_show(sample_msg_packet->rssi,0,0,0);
							osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD); // 阻止休眠
                        }
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

/**
* @name: mac_layer_system_message_process
* This
* @param[in]   inArgName input argument description.
* @param[out]  outArgName output argument description.
* @retval  ERR_SUCCESS 0
* @retval  ERR_ERROR   < 0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-04-29
*/
static void mac_layer_system_message_process( cot_mac_msg_t *p_msg )
{
	
    switch (p_msg->event_cmd)
    {
        case MAC_EVENT_RX_DONE_CMD:
        {
			if( s_config_data.RFParamSettings.Entity == RF_SLAVE )
			{
				if( g_functional_mode == SLAVE_RX )
				{
					cot_mt_led_blink(COT_MT_LED_3,1,50,5);
					cot_sample_msg_handle(p_msg);
				}
			}
		}
        break;
        case MAC_EVENT_TX_DONE_CMD:
        {
			if( s_config_data.RFParamSettings.Entity == RF_MASTER )
			{
				if(g_functional_mode == MASTER_TX)
				{
					s_cot_sample_pingpong.TX_count++;
					pingpong_parameter_show(0,0,0,0);
					sample_send_pingpong("PING",0xFFFFFFFF,4);
				}
			}
			else
			{
				osal_stop_timerEx(get_sample_task_id(), SMAPLE_MSG_TX_EVENT);
			}

        }
        break;
        case MAC_EVENT_TX_TIMEOUT_CMD:
        {
            //osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_TIMEOUT_EVENT);
			if(g_functional_mode == MASTER_TX)
			{
				osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_EVENT);
			}
			else
				osal_set_event(get_sample_task_id(),SMAPLE_MSG_CONFIG_EVENT);
        }
        break;
        case MAC_EVENT_RX_TIMEOUT_CMD:
        {
			if( s_config_data.RFParamSettings.Entity == RF_SLAVE )
			{
				if(g_functional_mode == SLAVE_ANCHOR)
				{
					osal_set_event(get_sample_task_id(),SMAPLE_MSG_CONFIG_EVENT);
				}
				else
					osal_start_timerEx(get_sample_task_id(), SMAPLE_MSG_RX_EVENT,10);
				//osal_set_event(get_sample_task_id(),SMAPLE_MSG_RX_TIMEOUT_EVENT);
			}
			else
			{
				osal_stop_timerEx(get_sample_task_id(), SMAPLE_MSG_RX_EVENT);
				if(g_functional_mode == MASTER_MOVE)
				{
					osal_set_event(get_sample_task_id(),SMAPLE_MSG_CONFIG_EVENT);
				}

			}
        }
        break;

        case MAC_EVENT_RX_ERROR_CMD:
        {
			if( g_functional_mode == SLAVE_RX )
			{
				osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_ERROR_EVENT);
			}
			else
			{
				osal_set_event(get_sample_task_id(),SMAPLE_MSG_CONFIG_EVENT);
			}

        }
        break;
		case MAC_EVENT_RNG_DONE_CMD:
        {
			

        }
        break;

        default:
            // do nothing
            break;
    }
}


/**
* @name: pingpong_param_reset
* This
* @param[in]   none.
* @param[out]  none.
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-23
*/
void pingpong_param_reset()
{
    s_cot_sample_pingpong.TX_count = 0;
    s_cot_sample_pingpong.RX_count = 0;
    s_cot_sample_pingpong.RX_SUC_count = 0;
    //pingpong_parameter_show(0);
}

/**
* @name: phy_button_process_osal_msg
* This
* @param[in]   inArgName input argument description.
* @param[out]  none.
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-23
*/
static void phy_button_process_osal_msg( mt_osal_msg_data_t *p_msg )
{
    switch( p_msg->event_cmd )
    {
        case KEY_EVENT_CMD_KEY1_PRESSED:
        {
            s_cot_button_count.key1++;
			if(s_cot_button_count.key1 >= 4)
				s_cot_button_count.key1 = 0;

		    if(s_cot_button_count.key1 == 0)
			{
				g_functional_mode = SLAVE_RX;

				pingpong_param_reset();

				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				s_config_data.RFParamSettings.Entity = RF_SLAVE;

				pingpong_parameter_show(0,0,0,0);
				osal_stop_timerEx(s_sample_app_id,SMAPLE_MSG_CONFIG_EVENT);
				osal_stop_timerEx(s_sample_app_id,SMAPLE_MSG_RNG_EVENT);
				//osal_set_event(s_sample_app_id,SMAPLE_MSG_CONFIG_EVENT);
			}
			else if(s_cot_button_count.key1 == 1)
			{
				g_functional_mode = MASTER_TX;

				pingpong_param_reset();

				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				s_config_data.RFParamSettings.Entity = RF_MASTER;

				pingpong_parameter_show(0,0,0,0);
				//osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_CONFIG_EVENT,3000);
			}
			else if(s_cot_button_count.key1 == 2)
			{
				g_functional_mode = MASTER_MOVE;
				pingpong_param_reset();

				if(s_config_data.ModulationParams.Params.LoRa.SpreadingFactor > RF_LORA_SF10)
				{
					s_config_data.ModulationParams.Params.LoRa.SpreadingFactor = RF_LORA_SF10;
					s_config_data.RFParamSettings.ModulationParam1 = RF_LORA_SF10;
					set_rf_param(&s_config_data,true);
				}
				if(s_config_data.ModulationParams.Params.LoRa.Bandwidth == RF_LORA_BW_0200)
				{
					s_config_data.ModulationParams.Params.LoRa.Bandwidth = RF_LORA_BW_0400;
					s_config_data.RFParamSettings.ModulationParam2 = RF_LORA_BW_0400;
					set_rf_param(&s_config_data,true);
				}
				s_config_data.RFParamSettings.Entity = RF_MASTER;

				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;

				pingpong_parameter_show(0,0,0,0);

				//osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_CONFIG_EVENT,3000);


			}
			else
			{
				g_functional_mode = SLAVE_ANCHOR;
				pingpong_param_reset();

				s_config_data.RFParamSettings.Entity = RF_SLAVE;
				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;

				pingpong_parameter_show(0,0,0,0);
				//osal_set_event(s_sample_app_id,SMAPLE_MSG_CONFIG_EVENT);
			}
            //osal_set_event(s_sample_app_id,SMAPLE_MSG_INIT_EVENT);

            osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD); // 阻止休眠
        }
        break;
        case KEY_EVENT_CMD_KEY2_PRESSED:
        {
			pingpong_param_reset();
            s_cot_button_count.key2++;
            s_cot_button_count.key2subkey5 ++;
            pingpong_parameter_show(0,0,0,0);
			//wait_ms(3000);
        }
        break;
        case KEY_EVENT_CMD_KEY3_PRESSED:
        {
			pingpong_param_reset();
            s_cot_button_count.key3++;
            s_cot_button_count.key4subkey3 --;
            pingpong_parameter_show(0,0,0,0);
			
        }
        break;
        case KEY_EVENT_CMD_KEY4_PRESSED:
        {
			pingpong_param_reset();
            s_cot_button_count.key4++;
            s_cot_button_count.key4subkey3 ++;
			pingpong_parameter_show(0,0,0,0);
			//wait_ms(3000);
        }
        break;
        case KEY_EVENT_CMD_KEY5_PRESSED:
        {
			pingpong_param_reset();
            s_cot_button_count.key5++;
            s_cot_button_count.key2subkey5 --;
            pingpong_parameter_show(0,0,0,0);
			//wait_ms(3000);
        }
        break;
        default:
            break;
    }
	
	if( s_config_data.RFParamSettings.Entity == RF_MASTER )
	{
		osal_stop_timerEx(get_sample_task_id(), SMAPLE_MSG_TX_EVENT);
		osal_start_timerEx(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT,3000);
		//wait_ms(3000);
	}
	else
	{
		osal_stop_timerEx(get_sample_task_id(), SMAPLE_MSG_RX_EVENT);
		osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT);
	}
}


/**
* @name: pingpong_sample_rf_param_set
* This funtion is to initialization rf param
* @param[in]   task_id -- phy task id in osal.
* @param[out]  none
* @retval  ERR_SUCCESS  0
* @retval  do not deal the event >0
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-05-10
*/
static int32_t pingpong_sample_rf_param_set(ConfigData_t *ConfigData)
{
    if(s_cot_button_count.key4subkey3 != 0) // sf
    {
        if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA || ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_RANGING) //
        {

			ConfigData->ModulationParams.Params.LoRa.SpreadingFactor = (RF_RadioLoRaSpreadingFactors_t)((uint8_t)ConfigData->ModulationParams.Params.LoRa.SpreadingFactor \
                                                                        + s_cot_button_count.key4subkey3 * 0x10);
			ConfigData->ModulationParams.Params.Rng.SpreadingFactor = ConfigData->ModulationParams.Params.LoRa.SpreadingFactor;
			ConfigData->RFParamSettings.ModulationParam1 = ConfigData->ModulationParams.Params.LoRa.SpreadingFactor;


			set_rf_param(ConfigData,true);
			//设置LORA和RNG参数同步
			if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA)
			{
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
				set_rf_param(ConfigData,true);
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
			}
			else
			{
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				set_rf_param(ConfigData,true);
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
			}
		}
    }

    if(s_cot_button_count.key2subkey5 != 0) // bw
    {
        if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA || ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_RANGING) //
        {
            ConfigData->ModulationParams.Params.LoRa.Bandwidth = (RF_RadioLoRaBandwidths_t)((uint8_t)ConfigData->ModulationParams.Params.LoRa.Bandwidth
                                                                -s_cot_button_count.key2subkey5 *0xE);
			ConfigData->ModulationParams.Params.Rng.Bandwidth = ConfigData->ModulationParams.Params.LoRa.Bandwidth;
			ConfigData->RFParamSettings.ModulationParam2 = ConfigData->ModulationParams.Params.LoRa.Bandwidth;
			set_rf_param(ConfigData,true);

			if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA)
			{
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
				set_rf_param(ConfigData,true);
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
			}
			else
			{
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				set_rf_param(ConfigData,true);
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
			}
		}
        else if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_GFSK) //
        {

        }
        else if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_FLRC) //
        {

        }
        else if(ConfigData->RFParamSettings.ModulationType == RF_PACKET_TYPE_BLE) //
        {
            ;
        }
    }
    s_cot_button_count.key4subkey3 = 0;
    s_cot_button_count.key2subkey5 = 0;

	#if 1
	if(g_functional_mode == MASTER_MOVE ||g_functional_mode == SLAVE_ANCHOR)
	{
		if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF5)
		{
			if(ConfigData->ModulationParams.Params.LoRa.Bandwidth == RF_LORA_BW_1600)
			{
				ConfigData->ModulationParams.Params.LoRa.Bandwidth = RF_LORA_BW_0800;
				ConfigData->RFParamSettings.ModulationParam2 = (uint8_t)ConfigData->ModulationParams.Params.LoRa.Bandwidth;
				set_rf_param(ConfigData,true);

				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				set_rf_param(ConfigData,true);
				ConfigData->RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
			}
		}

	}

#endif
	//if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF5)
		ConfigData->ModulationParams.Params.LoRa.CodingRate = RF_LORA_CR_4_5;
	#if 0
	else if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF6)
		ConfigData->ModulationParams.Params.LoRa.CodingRate = RF_LORA_CR_4_6;
	else if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF7)
		ConfigData->ModulationParams.Params.LoRa.CodingRate = RF_LORA_CR_4_7;
	else if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF8)
		ConfigData->ModulationParams.Params.LoRa.CodingRate = RF_LORA_CR_4_8;
	else if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF9)
		ConfigData->ModulationParams.Params.LoRa.CodingRate = RF_LORA_CR_4_5;
	else if(ConfigData->ModulationParams.Params.LoRa.SpreadingFactor == RF_LORA_SF10)
		ConfigData->ModulationParams.Params.LoRa.CodingRate = RF_LORA_CR_4_6;
	#endif
	ConfigData->RFParamSettings.ModulationParam3 = (uint8_t)ConfigData->ModulationParams.Params.LoRa.CodingRate;

	//ConfigData->RFParamSettings.DataSetType = RADIO_LORA_PARAMS;
	set_rf_param(ConfigData,true);

    return ERR_SUCCESS;
}


/**
* @name: pingpong_parameter_show
* This
* @param[in]   inArgName input argument description.
* @param[out]  none.
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by wende.wu on 2017-06-23
*/
static void pingpong_parameter_show(int8_t rssi,double distance,int8_t valid_count , int32_t request_count)
{
    static RF_RadioPacketTypes_t s_mode = RF_PACKET_TYPE_NONE;
    static uint8_t s_bw = 0;
    static uint8_t s_sf = 0;
    static uint8_t s_cr = 0;  // CodingRate
    static int8_t s_txpower = 99;
    static uint32_t s_frequency = 0;  // Frequency
    static DemoMode s_rfmode;
    uint8_t showstr[25] = {0};
    uint8_t i = 0;
    ConfigData_t *rf_config_param = &s_config_data;

    if((rf_config_param->RFParamSettings.ModulationType != s_mode)//  First line
        ||(rf_config_param->RFParamSettings.Entity != s_rfmode))
    {
        s_bw = 0;
        s_sf = 0;
        s_cr = 0;
        s_rfmode = (DemoMode)rf_config_param->RFParamSettings.Entity;
        #if 1
        if(rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA) //
        {
            memcpy(showstr,"LoRa",4);
        }
        else if(rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_GFSK) //
        {
            memcpy(showstr,"GFSK",4);
        }
        else if(rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_FLRC) //
        {
            memcpy(showstr,"FLRC",4);
        }
        else if(rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_BLE) //
        {
            memcpy(showstr,"BLE",3);showstr[4] = ' ';
        }
		else if(rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_RANGING) //
        {
            memcpy(showstr,"Rng",3);showstr[3] = ' ';
        }
		if( g_functional_mode == MASTER_TX)
			showstr[4] = 'T';
		else if( g_functional_mode == SLAVE_RX )
			showstr[4] = 'R';
		else
			showstr[4] = ' ';
        //if(s_rfmode == RF_MASTER) showstr[4] = 'T';
        //else  showstr[4] = 'R';
        showstr[5] = 0;
        OLED_ShowStr8x16(OLED_SHOWDARK,0,0,showstr);
        #endif
    }

    if( rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA || rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_RANGING )
    {

        if((rf_config_param->RFParamSettings.ModulationParam2 != s_bw)
            ||(s_sf != rf_config_param->RFParamSettings.ModulationParam1)
            ||(s_cot_button_count.key2subkey5 != 0)
            ||(s_cot_button_count.key4subkey3 != 0))
        {
            memset(showstr,0,21);
			if(g_functional_mode == MASTER_TX || g_functional_mode ==SLAVE_RX )
			{
				switch( rf_config_param->RFParamSettings.ModulationParam2 )
				{
					case 0x34:
						if(s_cot_button_count.key2subkey5 > 3) s_cot_button_count.key2subkey5 = 3;
						if(s_cot_button_count.key2subkey5 < 0) s_cot_button_count.key2subkey5 = 0;
					break;
					case 0x26:
						if(s_cot_button_count.key2subkey5 > 2) s_cot_button_count.key2subkey5 = 2;
						if(s_cot_button_count.key2subkey5 < -1) s_cot_button_count.key2subkey5 = -1;
					break;
					case 0x18:
						if(s_cot_button_count.key2subkey5 > 1) s_cot_button_count.key2subkey5 = 1;
						if(s_cot_button_count.key2subkey5 < -2) s_cot_button_count.key2subkey5 = -2;
					break;
					case 0x0A:
						if(s_cot_button_count.key2subkey5 > 0) s_cot_button_count.key2subkey5 = 0;
						if(s_cot_button_count.key2subkey5 < -3) s_cot_button_count.key2subkey5 = -3;
					break;
					default:
						s_cot_button_count.key2subkey5 = 0;
						break;
				}
			}
			if(g_functional_mode == MASTER_MOVE || g_functional_mode == SLAVE_ANCHOR)
			{
				switch( rf_config_param->RFParamSettings.ModulationParam2 )
				{
					case 0x26:
						if(s_cot_button_count.key2subkey5 > 2) s_cot_button_count.key2subkey5 = 2;
						if(s_cot_button_count.key2subkey5 < 0) s_cot_button_count.key2subkey5 = 0;
					break;
					case 0x18:
						if(s_cot_button_count.key2subkey5 > 1) s_cot_button_count.key2subkey5 = 1;
						if(s_cot_button_count.key2subkey5 < -1) s_cot_button_count.key2subkey5 = -1;
					break;
					case 0x0A:
						if(s_cot_button_count.key2subkey5 > 0) s_cot_button_count.key2subkey5 = 0;
						if(s_cot_button_count.key2subkey5 < -2) s_cot_button_count.key2subkey5 = -2;
					break;
					default:
						s_cot_button_count.key2subkey5 = 0;
						break;
				}
			}
			
            s_bw = rf_config_param->RFParamSettings.ModulationParam2 - s_cot_button_count.key2subkey5 *0xE;
            switch( s_bw )
            {
                case 0x34:
                    memcpy(showstr,"BW:0200",strlen("BW:0200"));
                break;
                case 0x26:
                    memcpy(showstr,"BW:0400",strlen("BW:0400"));
                break;
                case 0x18:
                    memcpy(showstr,"BW:0800",strlen("BW:0800"));
                break;
                case 0x0A:
						memcpy(showstr,"BW:1600",strlen("BW:1600"));
                break;
                default:
                    memcpy(showstr,"BW:    ",strlen("BW:    "));
                    break;
            }
			
            if(g_functional_mode == MASTER_TX || g_functional_mode ==SLAVE_RX )
			{
				switch( rf_config_param->RFParamSettings.ModulationParam1 )
				{
					case 0x50:
						if(s_cot_button_count.key4subkey3 > 7) s_cot_button_count.key4subkey3 = 7;
						if(s_cot_button_count.key4subkey3 < 0) s_cot_button_count.key4subkey3 = 0;
					break;
					case 0x60:
						if(s_cot_button_count.key4subkey3 > 6) s_cot_button_count.key4subkey3 = 6;
						if(s_cot_button_count.key4subkey3 < -1) s_cot_button_count.key4subkey3 = -1;
					break;
					case 0x70:
						if(s_cot_button_count.key4subkey3 > 5) s_cot_button_count.key4subkey3 = 5;
						if(s_cot_button_count.key4subkey3 < -2) s_cot_button_count.key4subkey3 = -2;
					break;
					case 0x80:
						if(s_cot_button_count.key4subkey3 > 4) s_cot_button_count.key4subkey3 = 4;
						if(s_cot_button_count.key4subkey3 < -3) s_cot_button_count.key4subkey3 = -3;
					break;
					case 0x90:
						if(s_cot_button_count.key4subkey3 > 3) s_cot_button_count.key4subkey3 = 3;
						if(s_cot_button_count.key4subkey3 < -4) s_cot_button_count.key4subkey3 = -4;
					break;
					case 0xA0:
						if(s_cot_button_count.key4subkey3 > 2) s_cot_button_count.key4subkey3 = 2;
						if(s_cot_button_count.key4subkey3 < -5) s_cot_button_count.key4subkey3 = -5;
					break;
					case 0xB0:
						if(s_cot_button_count.key4subkey3 > 1) s_cot_button_count.key4subkey3 = 1;
						if(s_cot_button_count.key4subkey3 < -6) s_cot_button_count.key4subkey3 = -6;
					break;
					case 0xC0:
						if(s_cot_button_count.key4subkey3 > 0) s_cot_button_count.key4subkey3 = 0;
						if(s_cot_button_count.key4subkey3 < -7) s_cot_button_count.key4subkey3 = -7;
					break;
					default:
						s_cot_button_count.key4subkey3 = 0;
						break;
				}
			}
			if(g_functional_mode == MASTER_MOVE || g_functional_mode == SLAVE_ANCHOR)
			{
				switch( rf_config_param->RFParamSettings.ModulationParam1 )
				{
					case 0x50:
						if(s_cot_button_count.key4subkey3 > 5) s_cot_button_count.key4subkey3 = 5;
						if(s_cot_button_count.key4subkey3 < 0) s_cot_button_count.key4subkey3 = 0;
					break;
					case 0x60:
						if(s_cot_button_count.key4subkey3 > 4) s_cot_button_count.key4subkey3 = 4;
						if(s_cot_button_count.key4subkey3 < -1) s_cot_button_count.key4subkey3 = -1;
					break;
					case 0x70:
						if(s_cot_button_count.key4subkey3 > 3) s_cot_button_count.key4subkey3 = 3;
						if(s_cot_button_count.key4subkey3 < -2) s_cot_button_count.key4subkey3 = -2;
					break;
					case 0x80:
						if(s_cot_button_count.key4subkey3 > 2) s_cot_button_count.key4subkey3 = 2;
						if(s_cot_button_count.key4subkey3 < -3) s_cot_button_count.key4subkey3 = -3;
					break;
					case 0x90:
						if(s_cot_button_count.key4subkey3 > 1) s_cot_button_count.key4subkey3 = 1;
						if(s_cot_button_count.key4subkey3 < -4) s_cot_button_count.key4subkey3 = -4;
					break;
					case 0xA0:
						if(s_cot_button_count.key4subkey3 > 0) s_cot_button_count.key4subkey3 = 0;
						if(s_cot_button_count.key4subkey3 < -5) s_cot_button_count.key4subkey3 = -5;
					break;
					default:
						s_cot_button_count.key4subkey3 = 0;
						break;
				}
			}
            s_sf = rf_config_param->RFParamSettings.ModulationParam1 + s_cot_button_count.key4subkey3 *0x10;
            switch( s_sf )
            {
                case 0x50:
                    memcpy(showstr+ strlen((char*)showstr),"  SF5 ",strlen("  SF5 "));
                break;
                case 0x60:
                    memcpy(showstr+ strlen((char*)showstr),"  SF6 ",strlen("  SF6 "));
                break;
                case 0x70:
                    memcpy(showstr+ strlen((char*)showstr),"  SF7 ",strlen("  SF7 "));
                break;
                case 0x80:
                    memcpy(showstr+ strlen((char*)showstr),"  SF8 ",strlen("  SF8 "));
                break;
                case 0x90:
                    memcpy(showstr+ strlen((char*)showstr),"  SF9 ",strlen("  SF9 "));
                break;
                case 0xA0:
                    memcpy(showstr+ strlen((char*)showstr),"  SF10",strlen("  SF10"));
                break;
                case 0xB0:
                    memcpy(showstr+ strlen((char*)showstr),"  SF11",strlen("  SF11"));
                break;
                case 0xC0:
                    memcpy(showstr+ strlen((char*)showstr),"  SF12",strlen("  SF12"));
                break;
                default:
                    memcpy(showstr+ strlen((char*)showstr),"  SF  ",strlen("  SF  "));
                    break;
            }
            OLED_ShowStr6x8(OLED_SHOWDARK,48,0,showstr);
            memset(showstr,0,21);
        }


        if( s_cr != rf_config_param->RFParamSettings.ModulationParam3 )
        {
            memset(showstr,0,21);
            s_cr = rf_config_param->RFParamSettings.ModulationParam3;
            switch( s_cr )
            {
                case 0x01:
                    memcpy(showstr,"CR:4/5",strlen("CR:4/5"));
                break;
                case 0x02:
                    memcpy(showstr,"CR:4/6",strlen("CR:4/6"));
                break;
                case 0x03:
                    memcpy(showstr,"CR:4/7",strlen("CR:4/7"));
                break;
                case 0x04:
                    memcpy(showstr,"CR:4/8",strlen("CR:4/8"));
                break;
                case 0x05:
                    memcpy(showstr,"CR:4/5",strlen("CR:4/5")); // RF_LORA_CR_LI_4_5
                break;
                case 0x06:
                    memcpy(showstr,"CR:4/6",strlen("CR:4/6")); // RF_LORA_CR_LI_4_6
                break;
                case 0x07:
                    memcpy(showstr,"CR:4/7",strlen("CR:4/7")); // RF_LORA_CR_LI_4_7
                break;
                default:
                    memcpy(showstr,"CR: / ",strlen("CR: / "));
                    break;
            }
            OLED_ShowStr6x8(OLED_SHOWDARK,48,1,showstr);
            memset(showstr,0,21);
        }

        }
    else if((rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_GFSK)
        &&(rf_config_param->ModulationParams.Params.Gfsk.BitrateBandwidth != s_bw)) //
    {
        s_bw = rf_config_param->ModulationParams.Params.Gfsk.BitrateBandwidth + s_cot_button_count.key2subkey5;
        switch( s_bw )
        {
            case 0x04:
            case 0x28:
            case 0x4C:
            case 0x70:
                memcpy(showstr,"BW:2400",strlen("BW:2400"));
                break;
            case 0x45:
            case 0x69:
            case 0x8D:
            case 0xB1:
                memcpy(showstr,"BW:1200",strlen("BW:1200"));
                break;
            case 0x86:
            case 0xAA:
            case 0xCE:
                memcpy(showstr,"BW:0600",strlen("BW:0600"));
                break;
            case 0xC7:
            case 0xEF:
                memcpy(showstr,"BW:0300",strlen("BW:0300"));
                break;
            default:
                memcpy(showstr,"BW:    ",strlen("BW:    "));
                break;
        }
        showstr[7] = 0;
        OLED_ShowStr6x8(OLED_SHOWDARK,48,0,showstr);
    }
    else if((rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_FLRC)
        &&(rf_config_param->ModulationParams.Params.Flrc.BitrateBandwidth != s_bw)) //
    {
        s_bw = rf_config_param->ModulationParams.Params.Flrc.BitrateBandwidth;
        switch( s_bw )
        {
            case 0x45:
            case 0x69:
                memcpy(showstr,"BW:1200",strlen("BW:1200"));
                break;
            case 0x86:
            case 0xAA:
                memcpy(showstr,"BW:0600",strlen("BW:0600"));
                break;
            case 0xC7:
            case 0xEB:
                memcpy(showstr,"BW:0300",strlen("BW:0300"));
                break;
            default:
                memcpy(showstr,"BW:    ",strlen("BW:    "));
                break;
        }
        showstr[7] = 0;
        OLED_ShowStr6x8(OLED_SHOWDARK,48,0,showstr);
    }
    else if((rf_config_param->RFParamSettings.ModulationType == RF_PACKET_TYPE_BLE)
        &&(rf_config_param->ModulationParams.Params.Ble.BitrateBandwidth != s_bw)) //
    {
        s_bw = rf_config_param->ModulationParams.Params.Ble.BitrateBandwidth;
        switch( s_bw )
        {
            case 0x04:
            case 0x28:
            case 0x4C:
            case 0x70:
                memcpy(showstr,"BW:2400",strlen("BW:2400"));
                break;
            case 0x45:
            case 0x69:
            case 0x8D:
            case 0xB1:
                memcpy(showstr,"BW:1200",strlen("BW:1200"));
                break;
            case 0x86:
            case 0xAA:
            case 0xCE:
                memcpy(showstr,"BW:0600",strlen("BW:0600"));
                break;
            case 0xC7:
            case 0xEF:
                memcpy(showstr,"BW:0300",strlen("BW:0300"));
                break;
            default:
                memcpy(showstr,"BW:    ",strlen("BW:    "));
                break;
        }
        showstr[7] = 0;
        OLED_ShowStr6x8(OLED_SHOWDARK,48,0,showstr);
    }

    //2402000000u
    if(s_frequency != rf_config_param->RFParamSettings.Frequency)
    {
        s_frequency = rf_config_param->RFParamSettings.Frequency;
        memset(showstr,0,21);
        sprintf((char*)showstr,"%d",s_frequency / 1000000);
        OLED_ShowStr6x8(OLED_SHOWDARK,102,1,showstr);
    }

	if( rf_config_param->RFParamSettings.Entity == RF_MASTER && g_functional_mode == MASTER_TX )
	{
		// TX  =============================
		//if(s_txpower != rf_config_param->RFParamSettings.TxPower)
		{
			s_txpower = rf_config_param->RFParamSettings.TxPower;
			memset(showstr,0,21);
			memcpy(showstr,"TX:     Power:",strlen("TX:     Power:"));
			sprintf((char*)showstr + strlen((char*)showstr),"%d",s_txpower);
			memcpy((char*)showstr+ strlen((char*)showstr),"dB",strlen("dB"));
			OLED_ShowStr6x8(OLED_SHOWDARK,0,2,showstr);
		}
		memset(showstr,0,21);
		memcpy(showstr,"number:",strlen("number:"));
		sprintf((char*)showstr + strlen((char*)showstr),"%05d",s_cot_sample_pingpong.TX_count);
		OLED_ShowStr6x8(OLED_SHOWDARK,48,3,showstr);

		oled_line_cls(4);
		oled_line_cls(5);
		oled_line_cls(6);
		oled_line_cls(7);

	}

	if( rf_config_param->RFParamSettings.Entity == RF_SLAVE && g_functional_mode == SLAVE_RX )
    {
		// RX  =============================
		oled_line_cls(2);
		oled_line_cls(3);
		oled_line_cls(4);
        memset(showstr,0,21);
        memcpy(showstr,"RX:     Rssi:",strlen("RX:     Rssi:"));
        if(rssi != 0)
        {
            sprintf((char*)showstr+ strlen((char*)showstr),"%d",rssi);
            if(rssi > -50) i = 5;
            else if(rssi > -70) i = 4;
            else if(rssi > -90) i = 3;
            else if(rssi > -110) i = 2;
            else  i = 1;
            showstr[17] = 'z' + i*2;        // 两个字节 表示 RSSI
            showstr[18] = 'z' + i*2 + 1;
        }
        for(i = 0;i <= 18;i++)
        {
            if(showstr[i] == 0)
                showstr[i] = ' '; // 清空
        }
        OLED_ShowStr6x8(OLED_SHOWDARK,0,5,showstr);

		memset(showstr,0,21);
		memcpy(showstr,"count:",strlen("count:"));
		#if 1
		if( s_cot_sample_pingpong.RX_SUC_count >= 65535 )
		{
			s_cot_sample_pingpong.RX_SUC_count = 0;
		}

		sprintf((char*)showstr+ strlen((char*)showstr),"%05d",s_cot_sample_pingpong.RX_SUC_count);
		#endif
		OLED_ShowStr6x8(OLED_SHOWDARK,48,6,showstr);
		//showstr[strlen((char*)showstr)] = '/';

		memset(showstr,0,21);
		memcpy(showstr,"number:",strlen("number:"));
		if( s_cot_sample_pingpong.RX_count >= 65535 )
		{
			s_cot_sample_pingpong.RX_count = 0;
		}
		sprintf((char*)showstr+ strlen((char*)showstr),"%05d",s_cot_sample_pingpong.RX_count);
		OLED_ShowStr6x8(OLED_SHOWDARK,48,7,showstr);
    }
	if(rf_config_param->RFParamSettings.Entity == RF_MASTER && g_functional_mode == MASTER_MOVE)
	{
		memset(showstr,0,21);
		memcpy(showstr,"Move",4);
		showstr[5] = 0;
		OLED_ShowStr8x16(OLED_SHOWDARK,0,2,showstr);

		memset(showstr,0,21);
		for(i = 0;i <= 9;i++)
                showstr[i] = ' '; // 清空
		OLED_ShowStr6x8(OLED_SHOWDARK,48,2,showstr);

		s_txpower = rf_config_param->RFParamSettings.TxPower;
        memset(showstr,0,21);
        memcpy(showstr," TxPower:",strlen(" TxPower:"));
        sprintf((char*)showstr + strlen((char*)showstr),"%d",s_txpower);
        memcpy((char*)showstr+ strlen((char*)showstr),"dB",strlen("dB"));
        OLED_ShowStr6x8(OLED_SHOWDARK,48,3,showstr);

		memset(showstr,0,21);
		memcpy(showstr,"Res:",4);
		showstr[5] = 0;
		OLED_ShowStr8x16(OLED_SHOWDARK,0,4,showstr);

		memset(showstr,0,21);
		memcpy(showstr,"Status: on",strlen("Status: on"));
		OLED_ShowStr6x8(OLED_SHOWDARK,36,5,showstr);

		memset(showstr,0,21);
		memcpy(showstr,"Count:",strlen("Count:"));
		sprintf((char*)showstr + strlen((char*)showstr),"%02d/%02d",valid_count,request_count);
		OLED_ShowStr6x8(OLED_SHOWDARK,36,6,showstr);

		//distance = 1.5;
		memset(showstr,0,21);
		memcpy(showstr,"distance:",strlen("distance:"));
		sprintf((char*)showstr + strlen((char*)showstr),"%5.1fm",distance);
		OLED_ShowStr6x8(OLED_SHOWDARK,36,7,showstr);

	}
	if(rf_config_param->RFParamSettings.Entity == RF_SLAVE && g_functional_mode == SLAVE_ANCHOR)
	{
		memset(showstr,0,21);
		memcpy(showstr,"Anchor",6);
		showstr[7] = 0;
		OLED_ShowStr8x16(OLED_SHOWDARK,0,2,showstr);

		s_txpower = rf_config_param->RFParamSettings.TxPower;
        memset(showstr,0,21);
        memcpy(showstr," TxPower:",strlen(" TxPower:"));
        sprintf((char*)showstr + strlen((char*)showstr),"%d",s_txpower);
        memcpy((char*)showstr+ strlen((char*)showstr),"dB",strlen("dB"));
        OLED_ShowStr6x8(OLED_SHOWDARK,48,3,showstr);

		memset(showstr,0,21);
		memcpy(showstr,"Res:",4);
		showstr[5] = 0;
		OLED_ShowStr8x16(OLED_SHOWDARK,0,4,showstr);

		memset(showstr,0,21);
		memcpy(showstr,"Status: on",strlen("Status: on"));
		OLED_ShowStr6x8(OLED_SHOWDARK,36,5,showstr);
		oled_line_cls(6);
		oled_line_cls(7);
	}

}

void get_rf_param_flash(ConfigData_t *ConfigData)
{
	uint8_t buffer[RF_PARAM_BUFFER_SIZE] = {0};

    DataFlashMcuReadBuffer( 0, buffer, RF_PARAM_BUFFER_SIZE );
	DataFlashLoadGeneralSettings(buffer,ConfigData);
    DataFlashLoadSettings(buffer,ConfigData);
	get_rf_param(&s_config_data);
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
						mac_layer_system_message_process((cot_mac_msg_t *)samle_msg_ptr);
                    }
                    break;
                case CMD_KEY_MSG:
					{	
						phy_button_process_osal_msg((mt_osal_msg_data_t *)samle_msg_ptr);
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
		s_config_data.RFParamSettings.TxPower               = 13;
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

		#if 1
        mt_osal_msg_data_t *p_mt_msg = (mt_osal_msg_data_t*)osal_msg_allocate( sizeof(mt_osal_msg_data_t) );
        if( p_mt_msg )
        {
            p_mt_msg->hdr.event = CMD_SERIAL_MSG;
			p_mt_msg->event_cmd = SERIAL_EVENT_INIT_CMD;
            p_mt_msg->task_id = s_sample_app_id;
            p_mt_msg->msg_len = 0;
            osal_msg_send(get_mt_task_id(),(uint8_t*)p_mt_msg);
        }
        #endif

        osal_set_event(task_id, SMAPLE_MSG_CONFIG_EVENT);

        return (events ^ SMAPLE_MSG_INIT_EVENT);
    }


    if ( events & SMAPLE_MSG_CONFIG_EVENT )
    {
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) + sizeof(PHYParamSettings_t));
        if( p_msg )
        {
            PHYParamSettings_t phy_param_setting ;

            pingpong_sample_rf_param_set(&s_config_data);

			/*参数配置*/
			uint8_t buffer[RF_PARAM_BUFFER_SIZE] = {0};
			DataFlashMcuReadBuffer( 0, buffer, RF_PARAM_BUFFER_SIZE );

			if( g_functional_mode == MASTER_TX )
			{
				s_config_data.RFParamSettings.Entity = RF_MASTER;
				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				DataFlashLoadSettings(buffer,&s_config_data);
				get_rf_param(&s_config_data);
				s_config_data.RFParamSettings.PacketParam3 = 0xFF;
			}
			else if( g_functional_mode == SLAVE_ANCHOR )
			{
				s_config_data.RFParamSettings.Entity = RF_SLAVE;
				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
				DataFlashLoadSettings(buffer,&s_config_data);
				get_rf_param(&s_config_data);
				s_config_data.RFParamSettings.PacketParam3 = 13;
				s_config_data.RFParamSettings.RngRequestCount = 20;
				s_config_data.RFParamSettings.RngFullScale = 30;
				s_config_data.RFParamSettings.RngAddress = 0x00000001;
				s_config_data.RFParamSettings.RngAntenna = DEMO_RNG_ANT_1;
				s_config_data.RFParamSettings.RngUnit = DEMO_RNG_UNIT_SEL_M;
			}
			else if( g_functional_mode == MASTER_MOVE )
			{
				s_config_data.RFParamSettings.Entity = RF_MASTER;
				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
				DataFlashLoadSettings(buffer,&s_config_data);
				get_rf_param(&s_config_data);
				s_config_data.RFParamSettings.PacketParam3 = 13;
				s_config_data.RFParamSettings.RngRequestCount = 20;
				s_config_data.RFParamSettings.RngFullScale = 30;
				s_config_data.RFParamSettings.RngAddress = 0x00000001;
				 s_config_data.RFParamSettings.RngAntenna = DEMO_RNG_ANT_1;
				s_config_data.RFParamSettings.RngUnit = DEMO_RNG_UNIT_SEL_M;
			}
			else //SLAVE_RX
			{
				s_config_data.RFParamSettings.Entity = RF_SLAVE;
				s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
				DataFlashLoadSettings(buffer,&s_config_data);
				get_rf_param(&s_config_data);
				s_config_data.RFParamSettings.PacketParam3 = 0xFF;
			}

            set_rf_param(&s_config_data,true);  // save to flash
            sample_rf_param_get(s_config_data, &phy_param_setting);

            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_CONFIG_CMD;
            p_msg->src_task_id = task_id;
			p_msg->data = (uint8_t *)(p_msg + 1);
            p_msg->ModulationType = phy_param_setting.RF_RadioPacketTypes;


            memcpy((char *)p_msg->data,(char *)&phy_param_setting,sizeof(phy_param_setting));
            p_msg->data_len = sizeof(phy_param_setting);
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);

			
            osal_pwrmgr_task_state( task_id, PWRMGR_HOLD);
            //osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
			if( s_config_data.RFParamSettings.Entity == RF_MASTER )
			{
				if( g_functional_mode == MASTER_TX )
				{
					pingpong_param_reset();
					s_cot_sample_pingpong.TX_count++;
					sample_send_pingpong("PING",0xFFFFFFFF,4);
				}
				else
				{
					pingpong_param_reset();
					osal_set_event(s_sample_app_id,SMAPLE_MSG_RNG_EVENT);
				}
			}
			else
			{
				if( g_functional_mode == SLAVE_RX )
				{
					pingpong_param_reset();
					pingpong_parameter_show(0,0,0,0);
					osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_EVENT);
				}
				else
				{
					pingpong_param_reset();
					osal_set_event(s_sample_app_id,SMAPLE_MSG_RNG_EVENT);
				}
			}
            
			
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
		//log_printf("start rng\n");
        PHYParamSettings_t phy_param_setting ;
        sample_rf_param_get(s_config_data, &phy_param_setting);
		phy_param_setting.cot_dev_id = 0x00000001;
        cot_ranging_result_t cot_ranging_res;
        memset( (char *)&cot_ranging_res, 0x0, sizeof(cot_ranging_result_t) );
        cot_ranging(&phy_param_setting, &cot_ranging_res);
		
		
        if( phy_param_setting.Entity == RF_MASTER )
        {
			pingpong_parameter_show(0,cot_ranging_res.distance,cot_ranging_res.cnt_packet_rx_ok,s_config_data.RFParamSettings.RngRequestCount);
        }
       
		osal_start_timerEx(task_id,SMAPLE_MSG_CONFIG_EVENT,1000);
        return (events ^ SMAPLE_MSG_RNG_EVENT);
   }

    if ( events & SMAPLE_MSG_TX_DONE_EVENT )
	{
		
        return (events ^ SMAPLE_MSG_TX_DONE_EVENT);
    }

    if ( events & SMAPLE_MSG_TX_TIMEOUT_EVENT )
    {
        MSG_DEBUG(1,"SMAPLE_MSG_TX_TIMEOUT_EVENT\n");
        osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
        return (events ^ SMAPLE_MSG_TX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_TIMEOUT_EVENT )
    {
		if(g_functional_mode == SLAVE_RX)
		{
			 //osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
			osal_start_timerEx(task_id, SMAPLE_MSG_RX_EVENT,10);
			osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
		}
		else if(g_functional_mode == SLAVE_ANCHOR)
		{
			//osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
			osal_set_event( task_id, SMAPLE_MSG_RNG_EVENT );
		}

		return (events ^ SMAPLE_MSG_RX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_ERROR_EVENT )
    {
		if(g_functional_mode == SLAVE_RX)
		{
			//MSG_DEBUG(1,"MSG_RX_ERROR_EVENT\n");
			osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
			osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
		}
		else if(g_functional_mode == SLAVE_ANCHOR)
		{
			//osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
			osal_set_event( task_id, SMAPLE_MSG_RNG_EVENT );
		}
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


