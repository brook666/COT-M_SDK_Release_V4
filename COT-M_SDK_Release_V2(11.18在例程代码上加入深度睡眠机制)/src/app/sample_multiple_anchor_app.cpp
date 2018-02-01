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
#include "cot_ranging.h"
#include "sample_app.h"
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

#define MASTER_ROLE
//#define SLAVE_ROLE

/*********************************************************************
 * CONSTANTS
 */
// sample msg  Events
#define SMAPLE_MSG_TX_EVENT                              			0x0002
#define SMAPLE_MSG_TX_TIMEOUT_EVENT                      			0x0004
#define SMAPLE_MSG_RX_TIMEOUT_EVENT                           		0x0008
#define SMAPLE_MSG_RX_ERROR_EVENT                       			0x0010
#define SMAPLE_MSG_RNG_EVENT                          				0x0020
//#define SMAPLE_MSG_RANGING_DONE_EVENT                         		0x0040
//#define SMAPLE_MSG_RANGING_TIMEOUT_EVENT                            0x0080
#define SMAPLE_MSG_CONFIG_EVENT                                 	0x0100
#define SMAPLE_MSG_TX_DONE_EVENT                                 	0x0200
#define SMAPLE_MSG_RX_DONE_EVENT                                 	0x0400
#define SMAPLE_MSG_INIT_EVENT                                 		0x0800

#define SMAPLE_MSG_MASTER_DECE_TIMER_EVENT                          0x1000
#define SMAPLE_MSG_START_RNG_EVENT									0x2000
#define SMAPLE_MSG_HANDSHAKE_EVENT									0x4000


#define SMAPLE_MSG_SLEEP_EVENT        0x0040
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


/*!
 * \brief cot_phy_payload_head_t;
 * it's len is 10 Byte((4byte src_dev_id)+(4byte dst_dev_id)+(1byte commad_h)+(1byte commad_l))
 */
#define COT_PHY_PAYLOAD_HEAD    10

#define STATUS_SUCCESS          0
#define STATUS_FAIL             1

#define COMFIRM_FRAME           1
#define UNCOMFIRM_FRAME         0

#define MASTER_REVE_ANCHOR_COUNT		5
#define MASTER_RNG_DEVICE_COUNT			3
/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    //uint8_t                     src_dev_id[4];              //the device id of sender device id
    //uint8_t                     dst_dev_id[4];              //the device id of receiver device id
    uint8_t                     commad_h;                   //the high byte of commad
    uint8_t                     commad_l;                   //the low byte of commad
    uint8_t                     *data;                      //the payload data field
}cot_application_msg_t;

typedef struct
{
    osal_event_hdr_t            hdr;                    //osal event
    uint8_t  					src_task_id;			//the osal task id of the sendder
	RF_RadioPacketTypes_t       ModulationType;      	//modulation type
	int8_t                      rssi;                   //only effect when recevie
	cot_rf_hdr_t                rf_hdr;                 //only effect when send
    uint16_t                    arq_num;                //the num of retransmission
    uint8_t                     tx_window_time;         //tx window time
    uint16_t                    cot_application_msg_len;
    cot_application_msg_t       cot_application_msg;
}cot_sample_msg_t;

typedef struct device_node {
	uint32_t device_id;
	int8_t rssi; 
}BroadCast_Data;

enum LocationRunStates
{
	LOCATION_NONE = 0,
	LOCATION_ONE,
	LOCATION_TWO,
	LOCATION_THREE
};
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
 
static uint8_t g_sleep_pram = 0;

static uint8_t s_sample_app_id = 0; ///<  sample application id for osal task id

static  uint8_t s_soft_version[4]=	SOFT_VERSION;

static  uint8_t s_hard_ware_version[2]= HARD_WARE_VERSION;

static ConfigData_t s_config_data;

//static bool g_start_rng_flag_master = false;
static bool g_start_rng_flag_slave = false;

static uint16_t g_start_rng_count = 0;

static uint16_t g_device_id_count = 0;

BroadCast_Data g_device_id[5];

static cot_mac_msg_t *s_cot_sample_msg_tx;
bool g_flag_send_frist = false;

pkg_type_t g_pkg_type = UNCONFIRM;
static uint16_t g_NbSteps = 0xFFFF;
bool g_start_rng_flag = false;
static uint8_t g_send_count_t = 0;

static LocationRunStates g_LocationRunStates = LOCATION_NONE;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
/**
* @name: sample_rf_param_set
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
//static int32_t sample_rf_param_get(ConfigData_t ConfigData, PHYParamSettings_t *phy_param_setting );



static void mac_layer_system_message_process( cot_mac_msg_t *p_msg );

/*********************************************************************
 * PROFILE CALLBACKS
 */



/*********************************************************************
 * PUBLIC FUNCTIONS
 */


void master_broadcast_data()
{
	g_LocationRunStates = LOCATION_ONE;
	s_cot_sample_msg_tx->dst_addr = 0xFFFFFFFF;
	s_cot_sample_msg_tx->data = (uint8_t*)(s_cot_sample_msg_tx + 1);
	s_cot_sample_msg_tx->data[0] = 0x00;
	s_cot_sample_msg_tx->data[1] = 0x02;
	s_cot_sample_msg_tx->data[2] = 0x01;
	uint8_t temp[64];
	memset(temp,0x5a,64);
	
	//memcpy(&s_cot_sample_msg_tx->data[2],temp,64);
	s_cot_sample_msg_tx->data_len = 3;
}

void slave_broadcast_data(uint32_t device_id)
{
	
	s_cot_sample_msg_tx->dst_addr = device_id;
	s_cot_sample_msg_tx->data = (uint8_t*)(s_cot_sample_msg_tx + 1);
	s_cot_sample_msg_tx->data[0] = 0x00;
	s_cot_sample_msg_tx->data[1] = 0x02;
	s_cot_sample_msg_tx->data[2] = 0x00;
	uint8_t temp[64];
	memset(temp,0x5a,64);
	
	//memcpy(&s_cot_sample_msg_tx->data[2],temp,64);
	s_cot_sample_msg_tx->data_len = 3;
	
}
void master_handshake_data(uint8_t count)
{
	g_LocationRunStates = LOCATION_TWO;
	log_printf("\n%x\n",g_device_id[count].device_id);
	s_cot_sample_msg_tx->dst_addr = g_device_id[count].device_id;
	s_cot_sample_msg_tx->data = (uint8_t*)(s_cot_sample_msg_tx + 1);
	s_cot_sample_msg_tx->data[0] = 0x00;
	s_cot_sample_msg_tx->data[1] = 0x20;
	uint8_t temp[64];
	memset(temp,0x5a,64);
	
	memcpy(&s_cot_sample_msg_tx->data[2],temp,2);
	s_cot_sample_msg_tx->data_len = 4;
}
void slave_handshake_data(uint32_t device_id)
{
	s_cot_sample_msg_tx->dst_addr = device_id;
	s_cot_sample_msg_tx->data = (uint8_t*)(s_cot_sample_msg_tx + 1);
	s_cot_sample_msg_tx->data[0] = 0x00;
	s_cot_sample_msg_tx->data[1] = 0x20;
	
	uint8_t temp[64];
	memset(temp,0x5a,64);
	
	memcpy(&s_cot_sample_msg_tx->data[2],temp,2);
	s_cot_sample_msg_tx->data_len = 4;	
}

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
	cot_application_msg_t *sample_msg;
	sample_msg = (cot_application_msg_t *)sample_msg_packet->data;
	sample_msg->data = &sample_msg_packet->data[2];
	uint8_t cmd_h;
	cmd_h = sample_msg->commad_h;
	switch( cmd_h )
	{
		case COT_SYSTEM_CMD_H:
			{
				uint8_t cmd_l = sample_msg->commad_l;
				switch(cmd_l)
				{
					case SAMPLE_APP_CMD_BROADCAST:
						{
                            #ifdef MASTER_ROLE
								//log_printf("D=%x",sample_msg->data[0]);
								if(sample_msg->data[0] != 0x01)//多移动点广播标志位
								{
									g_device_id[g_device_id_count].device_id = sample_msg_packet->src_addr;
									g_device_id[g_device_id_count].rssi = sample_msg_packet->rssi;
									g_device_id_count++;
								
									if(g_device_id_count >= MASTER_RNG_DEVICE_COUNT)
									{
										osal_stop_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT);
										osal_set_event(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT);
									}
									else
									{
										g_NbSteps = 0xFFFF;
										osal_set_event(get_sample_task_id(),SMAPLE_MSG_RX_EVENT);
									}
								}
								else
								{
									g_NbSteps = 0xFFFF;
									osal_set_event(get_sample_task_id(),SMAPLE_MSG_RX_EVENT);
								}
							#else
								
								uint32_t dev_id = s_config_data.RFParamSettings.cot_dev_id;//get_dev_id();
								srand(dev_id);
								int16_t delayms = rand()%10;
								slave_broadcast_data(sample_msg_packet->src_addr);
								osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_TX_EVENT,delayms*7);
								log_printf("delayms = %d\n",delayms*2);
							#endif
						}
						break;
                    case SMAPLE_APP_CMD_START_RANG:
                        {
							#ifdef MASTER_ROLE
							
								//osal_set_event(s_sample_app_id,SMAPLE_MSG_START_RNG_EVENT);
								//osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_START_RNG_EVENT,7);
							#else
								
								//g_start_rng_flag_slave = true;
								//slave_handshake_data(sample_msg_packet->src_addr);
								osal_set_event(s_sample_app_id,SMAPLE_MSG_START_RNG_EVENT);
							#endif
                            
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


static void phy_button_process_osal_msg( mt_osal_msg_data_t *p_msg )
{
    switch( p_msg->event_cmd )
    {
        case KEY_EVENT_CMD_KEY1_PRESSED:
        {   
       #ifdef MASTER_ROLE
			g_start_rng_count = 0;
			g_device_id_count = 0;
			g_start_rng_flag = false;
			g_pkg_type = UNCONFIRM;
			
			memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
			master_broadcast_data();
			osal_start_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
			osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
			#endif

        }
        break;
        case KEY_EVENT_CMD_KEY2_PRESSED:
        {   
        }
        break;
        case KEY_EVENT_CMD_KEY3_PRESSED:
        {
		
        }
        break;
        case KEY_EVENT_CMD_KEY4_PRESSED:
        {
			
        }
        break;
        case KEY_EVENT_CMD_KEY5_PRESSED:
        {
        }
        break;
        default:
            break;
    }
}
/**
* @name: phy_layer_process_osal_msg
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
			cot_sample_msg_handle(p_msg);
		}
        break;

        case MAC_EVENT_TX_DONE_CMD:
        {
            osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_DONE_EVENT);
        }
        break;

        case MAC_EVENT_TX_TIMEOUT_CMD:
        {
            osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_TIMEOUT_EVENT);
        }
        break;

        case MAC_EVENT_RX_TIMEOUT_CMD:
        {
            osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_TIMEOUT_EVENT);
        }
        break;

		case MAC_EVENT_RX_ERROR_CMD:
        {
            osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_ERROR_EVENT);
        }
        break;

        default:
            // do nothing
            break;
    }
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
#include "hal_timers.h"
uint16_t sample_app_processevent(uint8_t task_id, uint16_t events)
{
    uint16_t ret = ERR_SUCCESS ;
	
	
	if ( events & SMAPLE_MSG_HANDSHAKE_EVENT )
	{
		
		master_handshake_data(g_start_rng_count);
		g_pkg_type = CONFIRM;
		g_start_rng_flag = true;
		osal_set_event(task_id,SMAPLE_MSG_CONFIG_EVENT|SMAPLE_MSG_TX_EVENT);
		return (events ^ SMAPLE_MSG_HANDSHAKE_EVENT);
	}
    if ( events & SYS_EVENT_MSG )
    {
        cot_sample_msg_t *samle_msg_ptr;

        while ( (samle_msg_ptr = (cot_sample_msg_t*)osal_msg_receive( task_id )) != NULL )
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
		
		s_cot_sample_msg_tx = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t)+ 239 );

        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t)+sizeof(PHYParamSettings_t) );
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
        
		
		osal_start_timerEx(task_id,SMAPLE_MSG_CONFIG_EVENT,2);
		return (events ^ SMAPLE_MSG_INIT_EVENT);
	}

    if ( events & SMAPLE_MSG_CONFIG_EVENT )
    {
		
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t)+sizeof(PHYParamSettings_t) );
		
		s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
		s_config_data.RFParamSettings.ModulationParam1 = RF_LORA_SF7;
		s_config_data.RFParamSettings.ModulationParam2 = RF_LORA_BW_1600;
		s_config_data.RFParamSettings.ModulationParam3 = RF_LORA_CR_4_5;

		s_config_data.RFParamSettings.PacketParam1 = 12 ; // PreambleLength
		s_config_data.RFParamSettings.PacketParam2 = RF_LORA_PACKET_VARIABLE_LENGTH;
		s_config_data.RFParamSettings.PacketParam3 = DEMO_GFS_LORA_MAX_PAYLOAD;
		s_config_data.RFParamSettings.PacketParam4 = RF_LORA_CRC_ON;
		s_config_data.RFParamSettings.PacketParam5 = RF_LORA_IQ_NORMAL;
		s_config_data.RFParamSettings.Frequency = DEMO_CENTRAL_FREQ_PRESET1;
        if( p_msg )
        {
            PHYParamSettings_t phy_param_setting ;
            sample_rf_param_get(s_config_data, &phy_param_setting);

            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_CONFIG_CMD;
            p_msg->src_task_id = task_id;
            p_msg->ModulationType = phy_param_setting.RF_RadioPacketTypes;
            p_msg->data = (uint8_t*)(p_msg+1);

            memcpy((char *)p_msg->data,(char *)&phy_param_setting,sizeof(phy_param_setting));
            p_msg->data_len = sizeof(phy_param_setting);
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
             //osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
            osal_pwrmgr_task_state( task_id, PWRMGR_HOLD);
					 // osal_set_event(task_id,SMAPLE_MSG_SLEEP_EVENT);
        }
		
		#ifdef SLAVE_ROLE
			g_start_rng_flag_slave = false;
			g_NbSteps = 0xFFFF;
			osal_set_event(task_id, SMAPLE_MSG_RX_EVENT);
		#endif
		
        return (events ^ SMAPLE_MSG_CONFIG_EVENT);
    }

	
	if ( events & SMAPLE_MSG_START_RNG_EVENT )
	{

		s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
		s_config_data.RFParamSettings.ModulationParam1 = RF_LORA_SF7;
		s_config_data.RFParamSettings.ModulationParam2 = RF_LORA_BW_1600;
		s_config_data.RFParamSettings.ModulationParam3 = RF_LORA_CR_4_5;

		s_config_data.RFParamSettings.PacketParam1 = 12 ; // PreambleLength
		s_config_data.RFParamSettings.PacketParam2 = RF_LORA_PACKET_VARIABLE_LENGTH;
		s_config_data.RFParamSettings.PacketParam3 = 13;
		s_config_data.RFParamSettings.PacketParam4 = RF_LORA_CRC_ON;
		s_config_data.RFParamSettings.PacketParam5 = RF_LORA_IQ_NORMAL;
        s_config_data.RFParamSettings.RngRequestCount = 20;
        s_config_data.RFParamSettings.RngFullScale = 30;
		s_config_data.RFParamSettings.Frequency = DEMO_CENTRAL_FREQ_PRESET1;
		s_config_data.RFParamSettings.ChannelsCount = 0xFFFFFFFFFF;
		#ifdef MASTER_ROLE
        s_config_data.RFParamSettings.RngAddress = g_device_id[g_start_rng_count].device_id;
		#else
		s_config_data.RFParamSettings.RngAddress = get_dev_id();
		#endif
        s_config_data.RFParamSettings.RngAntenna = DEMO_RNG_ANT_1;
        s_config_data.RFParamSettings.RngUnit = DEMO_RNG_UNIT_SEL_M;

		osal_set_event(task_id,SMAPLE_MSG_RNG_EVENT);
		return (events ^ SMAPLE_MSG_START_RNG_EVENT);
	}
	
	if( events & SMAPLE_MSG_RNG_EVENT )
    {
		g_LocationRunStates = LOCATION_THREE;
        PHYParamSettings_t phy_param_setting ;
        sample_rf_param_get(s_config_data, &phy_param_setting);

        #ifdef MASTER_ROLE
        phy_param_setting.Entity = RF_MASTER;
        #endif

        #ifdef SLAVE_ROLE
        phy_param_setting.Entity = RF_SLAVE;
        #endif

        cot_ranging_result_t cot_ranging_res;
        memset( (char *)&cot_ranging_res, 0x0, sizeof(cot_ranging_result_t) );
        cot_ranging(&phy_param_setting, &cot_ranging_res);
		g_start_rng_flag = false;
		
        if( phy_param_setting.Entity == RF_MASTER )
        {
			//gpio_config(12,0);
            uint8_t printf_buff[128];
            uint8_t index = 0;
			g_start_rng_count++;
            memset(printf_buff, 0x0, sizeof(printf_buff) );
            sprintf( (char *)printf_buff, "Range: %5.2f mi", cot_ranging_res.distance );
            index = strlen((char *)printf_buff);
            sprintf( (char *)printf_buff+index, ",cnt_pkg_rx_ok=: %d",cot_ranging_res.cnt_packet_rx_ok );
            log_printf("%s\n",printf_buff);
			if(g_device_id_count > MASTER_RNG_DEVICE_COUNT)
			{
				if(g_start_rng_count < MASTER_RNG_DEVICE_COUNT)
					osal_set_event(task_id,SMAPLE_MSG_HANDSHAKE_EVENT);
				else
				{
					//gpio_config(15,0);
					//gpio_config(16,0);
					wait_ms(2000);
					g_pkg_type = UNCONFIRM;
					g_start_rng_count = 0;
					g_device_id_count = 0;
					memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
					master_broadcast_data();
					//g_flag_send_frist = true;
					//osal_set_event(get_sample_task_id(), SMAPLE_MSG_SLEEP_EVENT );
					osal_start_timerEx(task_id,SMAPLE_MSG_SLEEP_EVENT,1000);
					//osal_start_timerEx(task_id,SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
					//osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
					
				}
			}
			else
			{
				if(g_start_rng_count < g_device_id_count)
					osal_set_event(task_id,SMAPLE_MSG_HANDSHAKE_EVENT);
				else
				{
					//gpio_config(15,0);
					//gpio_config(16,0);
					wait_ms(2000);
					g_pkg_type = UNCONFIRM;
					g_start_rng_count = 0;
					g_device_id_count = 0;
					memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
					master_broadcast_data();
					//g_flag_send_frist = true;
					//osal_set_event(get_sample_task_id(), SMAPLE_MSG_SLEEP_EVENT );
					osal_start_timerEx(task_id,SMAPLE_MSG_SLEEP_EVENT,1000);
					
					//osal_start_timerEx(task_id,SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
					//osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
					
				}
			}
        }
        else
        {
            uint8_t printf_buff[128];
            uint8_t index = 0;

            memset(printf_buff, 0x0, sizeof(printf_buff) );
            sprintf( (char *)printf_buff+index, "cnt_pkg_rx_ok=: %d",cot_ranging_res.cnt_packet_rx_ok );
            log_printf("%s\n",printf_buff);
			osal_set_event(task_id,SMAPLE_MSG_CONFIG_EVENT);
        }

		//osal_set_event(task_id,SMAPLE_MSG_RNG_EVENT);

        return (events ^ SMAPLE_MSG_RNG_EVENT);
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
			p_msg->pkg_hdr.Bits.MType = g_pkg_type;
			p_msg->pkg_hdr.Bits.ack_request = 1;
			p_msg->rssi_water_leve = -80;
            p_msg->data = (uint8_t *)(p_msg+1);

            memcpy((char *)p_msg->data,s_cot_sample_msg_tx->data,s_cot_sample_msg_tx->data_len);
            p_msg->data_len = s_cot_sample_msg_tx->data_len;
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }
		else
		{
			log_printf("error\n");
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
            p_msg->irq_type.Value = RF_IRQ_RX_DONE | RF_IRQ_CRC_ERROR|RF_IRQ_RX_TX_TIMEOUT;
            p_msg->tick_timeout.Step = RF_RADIO_TICK_SIZE_1000_US;
            p_msg->tick_timeout.NbSteps = g_NbSteps;
            p_msg->data_len = 0;
            p_msg->data = NULL;

            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }

        osal_pwrmgr_task_state(task_id, PWRMGR_HOLD);
			//	osal_pwrmgr_task_state(task_id, PWRMGR_CONSERVE);

        return (events ^ SMAPLE_MSG_RX_EVENT);
    }

	if( ( events & SMAPLE_MSG_MASTER_DECE_TIMER_EVENT ) )
	{
		/*设置mac层空闲状态*/
		cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t) );
        if( p_msg )
        {
            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_FREE_CMD;
            p_msg->src_task_id = task_id;
            p_msg->data_len = 0;
            p_msg->data = NULL;
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }
		
		if( g_device_id_count > 0 )
		{
			
			//gpio_config(13,0);
			uint8_t i = 0,j = 0;
			
			if(g_device_id_count > 4)
			{
				//根据信号强度大到小排序		
				BroadCast_Data temp_data;
				for(i = 0; i < g_device_id_count; i++)
				{
					for(j = i+1; j< g_device_id_count; j++)
					{
						if( g_device_id[i].rssi < g_device_id[j].rssi)
						{
							temp_data = g_device_id[i];
							g_device_id[i] = g_device_id[j];
							g_device_id[j] = temp_data;
						}
					}
				}
			}
			for(int i = 0;i< g_device_id_count;i++)
			{
				log_printf("device_id = %x\n",g_device_id[i].device_id);
			}
			log_printf("\n");
			//gpio_config(12,1);
			//gpio_config(16,1);
			osal_set_event(task_id,SMAPLE_MSG_HANDSHAKE_EVENT);
		}
		else
		{
			g_start_rng_count = 0;
			g_device_id_count = 0;
			//g_flag_send_frist = true;
			g_start_rng_flag = false;
			g_pkg_type = UNCONFIRM;
			memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
			master_broadcast_data();
			osal_start_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
			osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
		}
		return (events ^ SMAPLE_MSG_MASTER_DECE_TIMER_EVENT);
	}
	
    if ( events & SMAPLE_MSG_TX_DONE_EVENT )
	{
        log_printf("SMAPLE_MSG_TX_DONE_EVENT\n");
		#ifdef MASTER_ROLE
			if(g_start_rng_flag)//启动测距标志
				osal_set_event(task_id,SMAPLE_MSG_START_RNG_EVENT);
			else
			{
				g_NbSteps = 0xFFFF;
				osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
			}
			
			//if(g_flag_send_frist)//新一轮开始标志
			{
				//g_flag_send_frist = false;
			}
		#else
			//if(g_start_rng_flag_slave)
				//osal_set_event(task_id,SMAPLE_MSG_START_RNG_EVENT);
			//else
			{
				g_NbSteps = 0xFFFF;
				osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
			}
		#endif
		return (events ^ SMAPLE_MSG_TX_DONE_EVENT);
    }

    if ( events & SMAPLE_MSG_TX_TIMEOUT_EVENT )
    {
		
		g_send_count_t++;
		if(g_send_count_t < 3)
		{
			osal_start_timerEx(task_id, SMAPLE_MSG_TX_EVENT,10);
		}
		else
		{
			g_send_count_t = 0;
			g_start_rng_count++;
			if(g_start_rng_count < g_device_id_count)
				osal_set_event(task_id,SMAPLE_MSG_HANDSHAKE_EVENT);
			else
			{
//				g_flag_send_frist = false;
				
				g_start_rng_count = 0;
				g_device_id_count = 0;
				g_pkg_type = UNCONFIRM;
				memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
				master_broadcast_data();
				osal_start_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
				osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
			}
		}
		log_printf("SMAPLE_MSG_TX_TIMEOUT_EVENT\n");
		return (events ^ SMAPLE_MSG_TX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_TIMEOUT_EVENT )
    {
		#ifdef MASTER_ROLE
		if(g_LocationRunStates == LOCATION_TWO)
		{
			g_start_rng_count++;
			if(g_start_rng_count < g_device_id_count)
				osal_set_event(task_id,SMAPLE_MSG_HANDSHAKE_EVENT);
			else
			{
//				g_flag_send_frist = false;
				
				g_start_rng_count = 0;
				g_device_id_count = 0;
				g_pkg_type = UNCONFIRM;
				memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
				master_broadcast_data();
				osal_start_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
				osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
			}
		}
		else
		{
			g_NbSteps = 0xFFFF;
			osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
			osal_start_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
		}
		#else
		g_NbSteps = 0xFFFF;
		osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
		#endif
		//osal_start_timerEx(task_id,SMAPLE_MSG_RX_EVENT,2);
		log_printf("SMAPLE_MSG_RX_TIMEOUT_EVENT\n");
		
		
		return (events ^ SMAPLE_MSG_RX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_ERROR_EVENT )
    {
		log_printf("SMAPLE_MSG_RX_ERROR_EVENT %d\n",g_LocationRunStates);
		#ifdef MASTER_ROLE
		if(g_LocationRunStates == LOCATION_TWO)
		{
			g_start_rng_count++;
			if(g_start_rng_count < g_device_id_count)
				osal_set_event(task_id,SMAPLE_MSG_HANDSHAKE_EVENT);
			else
			{
//				g_flag_send_frist = false;
				
				g_start_rng_count = 0;
				g_device_id_count = 0;
				g_pkg_type = UNCONFIRM;
				memset(g_device_id,0x0,sizeof(g_device_id)/sizeof(BroadCast_Data));
				master_broadcast_data();
				osal_start_timerEx(get_sample_task_id(),SMAPLE_MSG_MASTER_DECE_TIMER_EVENT,1000);
				osal_set_event(get_sample_task_id(), SMAPLE_MSG_CONFIG_EVENT | SMAPLE_MSG_TX_EVENT);
			}
		}
		else
		{
			g_NbSteps = 0xFFFF;
			osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
		}
		#else
		g_NbSteps = 0xFFFF;
		osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
		#endif
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
				osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
        return (events ^ SMAPLE_MSG_SLEEP_EVENT);
    }

    return ret ;
}

