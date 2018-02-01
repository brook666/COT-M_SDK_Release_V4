
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


			
#define SX1280_CHIP 0x00
#define SX1281_CHIP 0x01

//#define MASTER_ROLE
#define SLAVE_ROLE

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
//#define COT_PHY_PAYLOAD_HEAD    10

#define STATUS_SUCCESS          0
#define STATUS_FAIL             1

#define COMFIRM_FRAME           1
#define UNCOMFIRM_FRAME         0
/*********************************************************************
 * TYPEDEFS
 */


typedef struct
{
	osal_event_hdr_t            hdr;                    //osal event
    uint32_t                    dst_dev_id;              //the device id of receiver device id
	int8_t                      rssi;                   //only effect when recevie
	uint16_t                    arq_num;                //the num of retransmission
	uint16_t                    cot_phy_payload_len;
    uint8_t                     data[245]; 
}cot_application_msg_t;


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

static cot_application_msg_t s_cot_sample_msg_rx;

static cot_application_msg_t s_cot_sample_msg_tx;

static  uint8_t s_soft_version[4]=	SOFT_VERSION;

static  uint8_t s_hard_ware_version[2]= HARD_WARE_VERSION;

static uint8_t rngrequestcount;

static uint8_t seq_no;

static ConfigData_t s_config_data;
/*********************************************************************
 * LOCAL FUNCTIONS
 */



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
static void cot_sample_msg_handle( cot_application_msg_t *sample_msg );


static void  mac_layer_system_message_process( cot_mac_msg_t *p_msg );

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


static void Value_to_String(uint8_t *Value,uint8_t Value_size,uint8_t *String)
{
    uint8_t i= 0;
    for(i= 0;i < Value_size;i++)
    {
        String[i] = Value[Value_size-1-i];
    }
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
static void cot_sample_msg_handle( cot_application_msg_t *sample_msg )
{
	uint8_t index=0;	
	uint8_t cmd_h = sample_msg->data[index++];
	switch( cmd_h )
	{
		case COT_SYSTEM_CMD_H:
			{
				uint8_t cmd_l = sample_msg->data[index++];
				switch(cmd_l)
				{
					case SAMPLE_APP_CMD_BROADCAST:
						{
                            #ifdef MASTER_ROLE

                            mt_osal_msg_data_t *p_msg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+sample_msg->cot_phy_payload_len + 1);
                            if (p_msg)
                            {
                                /* Fill up what we can */
                                p_msg->hdr.event = CMD_SERIAL_MSG;
                                p_msg->event_cmd = SERIAL_EVENT_SEND_CMD;
                                p_msg->msg = (uint8_t*)(p_msg+1);
                                p_msg->msg[0] = sample_msg->rssi;
                                p_msg->msg_len = sample_msg->cot_phy_payload_len+1;////8 represents 4byte src_dev_id and 4byte dst_dev_id, 1 represents 1byte rssi
                                memcpy(&p_msg->msg[1],(char *)&sample_msg->data[0],sample_msg->cot_phy_payload_len);
                                osal_msg_send(get_mt_task_id(),(uint8_t *)p_msg);
								osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_RX_EVENT,50);
                            }

                            #else

							#define BROADCAST_APPLY_DEDUCT_DATA_LEN  	5

							uint8_t status = STATUS_FAIL;
							uint16_t ack_num = (uint16_t)sample_msg->data[2]<<8 | sample_msg->data[3];//
							uint8_t data_len = sample_msg->data[4];
							uint16_t pkg_len = sample_msg->cot_phy_payload_len;
							uint32_t dst_dev_id = s_cot_sample_msg_tx.dst_dev_id;
							uint32_t cot_dev_id = s_config_data.RFParamSettings.cot_dev_id;
							uint8_t index = 0 ;

							if( ( pkg_len - BROADCAST_APPLY_DEDUCT_DATA_LEN ) != data_len )
							{
								s_cot_sample_msg_tx.arq_num = 1;
								data_len = 0;
							}
							else
							{
								s_cot_sample_msg_tx.arq_num = ack_num;
								status = STATUS_SUCCESS;
							}
							
                          
                            //s_cot_sample_msg_tx.arq_num = ack_num;
							
                            s_cot_sample_msg_tx.data[index++] = cmd_h;
                            s_cot_sample_msg_tx.data[index++] = cmd_l;
                            s_cot_sample_msg_tx.data[index++] = STATUS_SUCCESS;
	                  
							dst_dev_id=(((dst_dev_id&0xff)<<24)|
								((dst_dev_id&0xff00)<<8)|
								((dst_dev_id&0xff0000)>>8)|
								((dst_dev_id&0xff000000)>>24));
                            memcpy((char *)&s_cot_sample_msg_tx.data[index],(char *)&dst_dev_id,4);
                            index += 4;
							
							uint8_t multicast_id[4];
							multicast_id[3] = (s_config_data.RFParamSettings.cot_multicast_id&0xff);
							multicast_id[2] = (s_config_data.RFParamSettings.cot_multicast_id&0xff00)>>8;
							multicast_id[1] = (s_config_data.RFParamSettings.cot_multicast_id&0xff0000)>>16;
							multicast_id[0] = (s_config_data.RFParamSettings.cot_multicast_id&0xff000000)>>24;
                            memcpy(&s_cot_sample_msg_tx.data[index],multicast_id,4);
                            index += 4;
							
                            memcpy(&s_cot_sample_msg_tx.data[index],s_soft_version,4);
                            index += 4;
                            memcpy(&s_cot_sample_msg_tx.data[index],s_hard_ware_version,2);
                            index += 2;
                            s_cot_sample_msg_tx.data[index++] = SX1280_CHIP;
							
							cot_dev_id=(((cot_dev_id&0xff)<<24)|
								((cot_dev_id&0xff00)<<8)|
								((cot_dev_id&0xff0000)>>8)|
								((cot_dev_id&0xff000000)>>24));
                            memcpy(&s_cot_sample_msg_tx.data[index],(char *)&cot_dev_id,4);
                            index += 4;
							
                            s_cot_sample_msg_tx.data[index++] = sample_msg->rssi;
                            s_cot_sample_msg_tx.data[index++] = data_len;
							if( data_len > 0 )
							{
								memcpy(&s_cot_sample_msg_tx.data[index],&sample_msg->data[5],data_len);
								index += data_len;
							}
                            s_cot_sample_msg_tx.cot_phy_payload_len = index ;

                            osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD);
							if(s_cot_sample_msg_tx.arq_num > 0)
							{
								s_cot_sample_msg_tx.arq_num--;
								osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_TX_EVENT,100);
								//osal_set_event( s_sample_app_id, SMAPLE_MSG_TX_EVENT);
							}
							else
							{
								//osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_TX_EVENT,100);
								osal_set_event( s_sample_app_id, SMAPLE_MSG_RX_EVENT);
							}
                            #endif

						}
						break;
                   
                
                    
                    case SMAPLE_APP_CMD_SEND_PKG:
                         {
							#ifdef MASTER_ROLE
							uint8_t index = 0;
							uint32_t cot_dev_id = s_config_data.RFParamSettings.cot_dev_id;
							 
                            mt_osal_msg_data_t *p_msg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+sample_msg->cot_phy_payload_len + 4);
                            if (p_msg)
                            {
                                /* Fill up what we can */
                                p_msg->hdr.event = CMD_SERIAL_MSG;
                                p_msg->event_cmd = SERIAL_EVENT_SEND_CMD;
                                p_msg->msg = (uint8_t*)(p_msg+1);
                                
								memcpy((char *)&p_msg->msg[index],(char *)&sample_msg->data[0],2);
								index += 2;																
								cot_dev_id=(((s_config_data.RFParamSettings.cot_dev_id&0xff)<<24)|
								((cot_dev_id&0xff00)<<8)|
								((cot_dev_id&0xff0000)>>8)|
								((cot_dev_id&0xff000000)>>24));
								memcpy(&p_msg->msg[index],(char *)&cot_dev_id,4);
								index += 4;								
								memcpy(&p_msg->msg[index],&sample_msg->data[2],sample_msg->cot_phy_payload_len-2);//2 represents commad_h and commad_l
								index += (sample_msg->cot_phy_payload_len-2);
								
								p_msg->msg_len = index;
                                osal_msg_send(get_mt_task_id(),(uint8_t *)p_msg);
								osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_RX_EVENT,50);
                            }

							#else
							
							#define SEND_PKG_APPLY_DEDUCT_DATA_LEN  	5
							
							seq_no = 0;
							uint8_t status = STATUS_FAIL;
							uint16_t pkg_len = sample_msg->cot_phy_payload_len;
                            uint8_t frame_format = sample_msg->data[2];
                            uint8_t ack_num = (uint8_t)sample_msg->data[3];
							uint8_t data_len = sample_msg->data[4];
							if( ( pkg_len - SEND_PKG_APPLY_DEDUCT_DATA_LEN ) != data_len )
						     
							{
								s_cot_sample_msg_tx.arq_num = 0;
								data_len = 0;
							}
							else
							{
								s_cot_sample_msg_tx.arq_num = ack_num;
								status = STATUS_SUCCESS;
							}
							
                            uint8_t index = 0 ;                          				                         
                            s_cot_sample_msg_tx.data[index++] = cmd_h;
                            s_cot_sample_msg_tx.data[index++] = cmd_l;
                            s_cot_sample_msg_tx.data[index++] = status;
                            s_cot_sample_msg_tx.data[index++] = frame_format;
                            s_cot_sample_msg_tx.data[index++] = 0x00; //pkg number
                            s_cot_sample_msg_tx.data[index++] = data_len;
							if( data_len > 0 )
							{
								memcpy(&s_cot_sample_msg_tx.data[index],&sample_msg->data[5],data_len);
								index += data_len;
							}
                            s_cot_sample_msg_tx.cot_phy_payload_len = index ;
                                                     
							osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_TX_EVENT,100);
							osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD);  
							#endif
                        }
                        break;
						case SMAPLE_APP_CMD_START_RANG :
					         {
						
						         s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_RANGING;
		                         s_config_data.RFParamSettings.ModulationParam1 = RF_LORA_SF8;  //SF5-->SF10,扩频因子递增,速率递减
		                         s_config_data.RFParamSettings.ModulationParam2 = RF_LORA_BW_1600; //200K--->1.6M 带宽递增,速率递增
		                         s_config_data.RFParamSettings.ModulationParam3 = RF_LORA_CR_4_5;

		                         s_config_data.RFParamSettings.PacketParam1 = 12 ; // PreambleLength
		                         s_config_data.RFParamSettings.PacketParam2 = RF_LORA_PACKET_VARIABLE_LENGTH;
		                         s_config_data.RFParamSettings.PacketParam3 = 13;
		                         s_config_data.RFParamSettings.PacketParam4 = RF_LORA_CRC_ON;
		                         s_config_data.RFParamSettings.PacketParam5 = RF_LORA_IQ_NORMAL;
                        
						         s_config_data.RFParamSettings.RngRequestCount = rngrequestcount;
                                 s_config_data.RFParamSettings.RngFullScale = 30;
                                 
                                 s_config_data.RFParamSettings.RngAntenna = DEMO_RNG_ANT_1;
                                 s_config_data.RFParamSettings.RngUnit = DEMO_RNG_UNIT_SEL_M;
                        
						
											
						         #ifdef MASTER_ROLE	
								 s_config_data.RFParamSettings.RngAddress =s_cot_sample_msg_tx.dst_dev_id ;
								 osal_set_event(s_sample_app_id,SMAPLE_MSG_CONFIG_EVENT);
						         #endif
						
						         #ifdef SLAVE_ROLE
						         uint8_t index = 0;
								 s_config_data.RFParamSettings.RngAddress =s_config_data.RFParamSettings.cot_dev_id ;								 
						         s_cot_sample_msg_tx.arq_num = 0;	
								 
						         s_cot_sample_msg_tx.data[index++] = cmd_h;
                                 s_cot_sample_msg_tx.data[index++] = cmd_l;
                                 s_cot_sample_msg_tx.data[index++] = 0x00;
						         s_cot_sample_msg_tx.cot_phy_payload_len = index;
																		
						         //osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_EVENT);
								 osal_start_timerEx(s_sample_app_id,SMAPLE_MSG_TX_EVENT,100);
												
						         #endif
					        }
					
					
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
static void  mac_layer_system_message_process( cot_mac_msg_t *p_msg )
{
    switch (p_msg->event_cmd)
    {
        case MAC_EVENT_RX_DONE_CMD:
        {
            s_cot_sample_msg_rx.rssi = p_msg->rssi;
            s_cot_sample_msg_rx.cot_phy_payload_len = p_msg->data_len ;
            s_cot_sample_msg_tx.dst_dev_id = p_msg->src_addr;
            memcpy(&s_cot_sample_msg_rx.data[0],p_msg->data,p_msg->data_len);
			cot_sample_msg_handle(&s_cot_sample_msg_rx);
			
			
		}
        break;

        case MAC_EVENT_TX_DONE_CMD:
        {
            osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_DONE_EVENT);
        }
        break;

        case MAC_EVENT_TX_TIMEOUT_CMD:
        {         
                osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_TIMEOUT_EVENT);  
        }
        break;

        case MAC_EVENT_RX_TIMEOUT_CMD:
        {
                osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_TIMEOUT_EVENT);
        }
        break;
		
        case MAC_EVENT_RX_ERROR_CMD:
        {
                osal_set_event(s_sample_app_id,SMAPLE_MSG_RX_TIMEOUT_EVENT);
        }
        break;
		

     
       

        default:
            // do nothing
            break;
    }
}


/**
* @name: cot_mt_serial_process_event
* This
* @param[in]   inArgName input argument description.
* @param[out]  outArgName output argument description.
* @retval  none
* @retval  none
* @par identifier
*      reserve
* @par other
*      none
* @par ModifyBlog
*      create by zhangjh on 2017-04-29
*/
static void cot_mt_serial_process_event( mt_osal_msg_data_t *p_msg )
{
    uint16_t index = 0;

    switch( p_msg->event_cmd )
    {
        case SERIAL_EVENT_RECV_CMD:
            {
                uint8_t rf_hdr =  p_msg->msg[index++];
                uint8_t cmd_h = p_msg->msg[index++];
                uint8_t cmd_l = p_msg->msg[index++];

                switch( cmd_h )
                {
                    case COT_SYSTEM_CMD_H:
                        {
                            switch( cmd_l )
                            {
                                case SAMPLE_APP_CMD_HANDLE:
                                    {

                                    }
                                    break;

                                case SAMPLE_APP_CMD_BROADCAST:
                                    {
										#if defined(MASTER_ROLE)
										
										#define BROADCAST_LORA_DATA_LEN 	214
										#define BROADCAST_GFSK_DATA_LEN 	214
										#define BROADCAST_FLRC_DATA_LEN 	84
										
										s_cot_sample_msg_tx.arq_num = 0;
                                      	s_cot_sample_msg_tx.dst_dev_id = s_config_data.RFParamSettings.cot_broadcast_id;
										
										uint8_t data_len = p_msg->msg[5];///5--represent the index of data length in broadcast cmd
										uint8_t tmp_count = 0;
										if( s_config_data.RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA )
										{
											if( data_len > BROADCAST_LORA_DATA_LEN)
											{
												//uint8_t tmp_count = 0;
												tmp_count = data_len - BROADCAST_LORA_DATA_LEN; // extra length
												p_msg->msg[5] -= tmp_count;
												//s_cot_sample_msg_tx.cot_phy_payload_len = tmp_count;
												s_cot_sample_msg_tx.cot_phy_payload_len = p_msg->msg[5];
											}
										}
										else if( s_config_data.RFParamSettings.ModulationType == RF_PACKET_TYPE_GFSK )
										{
											if( data_len > BROADCAST_GFSK_DATA_LEN )
											{
												//uint8_t tmp_count = 0;
												tmp_count = data_len - BROADCAST_GFSK_DATA_LEN; // extra length
												p_msg->msg[5] -= tmp_count;
												s_cot_sample_msg_tx.cot_phy_payload_len = tmp_count;
												//s_cot_sample_msg_tx.cot_phy_payload_len = p_msg->msg[5];
											}
										}
										else if( s_config_data.RFParamSettings.ModulationType == RF_PACKET_TYPE_FLRC )
										{
											if( data_len > BROADCAST_FLRC_DATA_LEN )
											{
												
												tmp_count = data_len - BROADCAST_FLRC_DATA_LEN; // extra length
												p_msg->msg[5] -= tmp_count;
												//s_cot_sample_msg_tx.cot_phy_payload_len = tmp_count;
												s_cot_sample_msg_tx.cot_phy_payload_len = p_msg->msg[5];
											}
										}
										
										s_cot_sample_msg_tx.cot_phy_payload_len = p_msg->msg_len-3 - tmp_count;////2byte cmd + 1byte rf_hdr + 多余的数据
										
										s_cot_sample_msg_tx.data[0] = cmd_h;
                                        s_cot_sample_msg_tx.data[1] = cmd_l;
                                        s_cot_sample_msg_tx.cot_phy_payload_len += 2;
										
                                        memcpy(&s_cot_sample_msg_tx.data[2],(uint8_t*)&p_msg->msg[index],s_cot_sample_msg_tx.cot_phy_payload_len-2);//2byte cmd_h cmd_l
										
                                        osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD);
                                        osal_set_event( s_sample_app_id, SMAPLE_MSG_TX_EVENT);

                                        #endif

                                    }
                                    break;
									
                              
                                case SMAPLE_APP_CMD_SEND_PKG:
                                    {
										#define SEND_DATA_LEN 232
										
                                       
                                        memcpy((char *)&s_cot_sample_msg_tx.dst_dev_id,&p_msg->msg[index],4); 
										s_cot_sample_msg_tx.dst_dev_id=(((s_cot_sample_msg_tx.dst_dev_id&0xff)<<24)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff00)<<8)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff0000)>>8)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff000000)>>24));
                                        s_cot_sample_msg_tx.cot_phy_payload_len = p_msg->msg_len-7;//2byte cmd + 1byte rf_hdr+4byte dst_id 
										s_cot_sample_msg_tx.arq_num =0;

										s_cot_sample_msg_tx.data[0]=cmd_h;
										s_cot_sample_msg_tx.data[1]=cmd_l;
										s_cot_sample_msg_tx.cot_phy_payload_len += 2;
										#if 1
										if( s_config_data.RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA )
										{
											uint8_t data_len = p_msg->msg[9];///9--represent the index of data length in send pkg cmd
											if( data_len > SEND_DATA_LEN)
											{
												uint8_t tmp_count = 0;
												tmp_count = data_len - SEND_DATA_LEN; // extra length
												p_msg->msg[9] -= tmp_count;
												s_cot_sample_msg_tx.cot_phy_payload_len -= tmp_count;
											}
										}
										#endif								
                                        memcpy(&s_cot_sample_msg_tx.data[2],(uint8_t*)&p_msg->msg[index+4],s_cot_sample_msg_tx.cot_phy_payload_len-2);//4byte dst_id 2byte cmd_h cmd_l
                                     
                                        osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD);
                                        osal_set_event( s_sample_app_id, SMAPLE_MSG_TX_EVENT);
										#if 1
                                        mt_osal_msg_data_t *p_mt_msg = (mt_osal_msg_data_t *)osal_msg_allocate(sizeof(mt_osal_msg_data_t)+12);
                                        if (p_mt_msg)
                                        {
                                            /* Fill up what we can */
                                            p_mt_msg->hdr.event = CMD_SERIAL_MSG;
                                            p_mt_msg->event_cmd = SERIAL_EVENT_RESPONSE_CMD;
                                            p_mt_msg->msg = (uint8_t*)(p_mt_msg+1);
                                            p_mt_msg->msg[0] = 0;
                                            p_mt_msg->msg[1] = cmd_h;
                                            p_mt_msg->msg[2] = cmd_l;
                                            p_mt_msg->msg[3] = SUCCESS;
                                            p_mt_msg->msg_len = 4;
                                            osal_msg_send(get_mt_task_id(),(uint8_t *)p_mt_msg);
                                        }
										#endif
                                    }
                                    break;
								case SMAPLE_APP_CMD_START_RANG:
								{
									#ifdef MASTER_ROLE
									uint8_t index = 0;
									s_cot_sample_msg_tx.arq_num = 0;
									rngrequestcount = p_msg->msg[12];									
									memcpy((char *)&s_cot_sample_msg_tx.dst_dev_id,&p_msg->msg[3],4); 
									s_cot_sample_msg_tx.dst_dev_id=(((s_cot_sample_msg_tx.dst_dev_id&0xff)<<24)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff00)<<8)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff0000)>>8)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff000000)>>24));
									
									
									s_cot_sample_msg_tx.data[index++] = cmd_h;
									s_cot_sample_msg_tx.data[index++] = cmd_l;
									s_cot_sample_msg_tx.data[index++] = 0x00;
									memcpy(&s_cot_sample_msg_tx.data[index],(uint8_t*)&p_msg->msg[7],5);
									index +=5;
									s_cot_sample_msg_tx.data[index++] = p_msg->msg[12];
									s_cot_sample_msg_tx.cot_phy_payload_len = index;
									
									
									osal_pwrmgr_task_state(s_sample_app_id, PWRMGR_HOLD);
									osal_set_event(s_sample_app_id,SMAPLE_MSG_TX_EVENT);
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
            break;

        default:
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
uint16_t sample_app_processevent(uint8_t task_id, uint16_t events)
{
    uint16_t ret = ERR_SUCCESS ;

    if ( events & SYS_EVENT_MSG )
    {
       cot_application_msg_t *samle_msg_ptr;

        while ( (samle_msg_ptr = (cot_application_msg_t*)osal_msg_receive( task_id )) != NULL )
        {
            switch(samle_msg_ptr->hdr.event)
            {
                case CMD_SERIAL_MSG:
                    {
                        cot_mt_serial_process_event((mt_osal_msg_data_t *)samle_msg_ptr);
                    }
                    break;
                case CMD_MAC_LAYER:
                    {
                        mac_layer_system_message_process((cot_mac_msg_t *)samle_msg_ptr);
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
        s_config_data.RFParamSettings.ModulationType = RF_PACKET_TYPE_LORA;
		s_config_data.RFParamSettings.ModulationParam1 = RF_LORA_SF8;
		s_config_data.RFParamSettings.ModulationParam2 = RF_LORA_BW_1600;
		s_config_data.RFParamSettings.ModulationParam3 = RF_LORA_CR_4_5;

		s_config_data.RFParamSettings.PacketParam1 = 12 ; // PreambleLength
		s_config_data.RFParamSettings.PacketParam2 = RF_LORA_PACKET_VARIABLE_LENGTH;
		s_config_data.RFParamSettings.PacketParam3 = DEMO_GFS_LORA_MAX_PAYLOAD;
		s_config_data.RFParamSettings.PacketParam4 = RF_LORA_CRC_ON;
		s_config_data.RFParamSettings.PacketParam5 = RF_LORA_IQ_NORMAL;
		set_rf_param(&s_config_data,true);
		
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

			osal_set_event( task_id,SMAPLE_MSG_CONFIG_EVENT );
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
         cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t)+sizeof(PHYParamSettings_t) );
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

            osal_pwrmgr_task_state( task_id, PWRMGR_HOLD);
            
			if( s_config_data.RFParamSettings.ModulationType == RF_PACKET_TYPE_LORA)
			{
			   #ifdef SLAVE_ROLE												
			        //osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
				    osal_start_timerEx(task_id,SMAPLE_MSG_RX_EVENT,10);
			   #endif
			}
			else
			{
				#ifdef MASTER_ROLE
					osal_start_timerEx(task_id,SMAPLE_MSG_RNG_EVENT ,2);		
				#endif
				#ifdef SLAVE_ROLE
				    osal_set_event( task_id,SMAPLE_MSG_RNG_EVENT );
				#endif
			}

        }

        return (events ^ SMAPLE_MSG_CONFIG_EVENT);
    }


    if(events & SMAPLE_MSG_TX_EVENT) // 发送数据
    {
        cot_mac_msg_t *p_msg = (cot_mac_msg_t*)osal_msg_allocate( sizeof(cot_mac_msg_t)+sizeof(cot_application_msg_t) );
        if( p_msg )
        {
            p_msg->hdr.event = CMD_MAC_LAYER;
            p_msg->event_cmd = MAC_EVENT_TX_CMD;
            p_msg->src_task_id = task_id;           
			p_msg->pkg_hdr.Bits.frame_pending=0;
			p_msg->pkg_hdr.Bits.MType=UNCONFIRM;
			p_msg->pkg_hdr.Bits.ack_request=0;
			p_msg->dst_addr = s_cot_sample_msg_tx.dst_dev_id;
			p_msg->src_addr = s_config_data.RFParamSettings.cot_dev_id;
			p_msg->irq_type.Value = RF_IRQ_TX_DONE | RF_IRQ_RX_TX_TIMEOUT;
			p_msg->rssi_water_leve = -80; //侦测阈值
			p_msg->data=(uint8_t *)(p_msg+1);
						
			memcpy(p_msg->data,s_cot_sample_msg_tx.data,s_cot_sample_msg_tx.cot_phy_payload_len);
            p_msg->data_len = s_cot_sample_msg_tx.cot_phy_payload_len;
			
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
			p_msg->src_addr=s_config_data.RFParamSettings.cot_dev_id;	
            osal_msg_send(get_mac_layer_task_id(),(uint8_t*)p_msg);
        }

        osal_pwrmgr_task_state(task_id, PWRMGR_HOLD);

        return (events ^ SMAPLE_MSG_RX_EVENT);
    }


    if(events & SMAPLE_MSG_RNG_EVENT) // 使能测距
    {
		
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
		
        if( phy_param_setting.Entity == RF_MASTER )
        {			
            uint8_t printf_buff[128];
            uint8_t index = 0;
            memset( printf_buff, 0x0, sizeof(printf_buff) );
			printf_buff[index++] = 0x00;
			printf_buff[index++] = 0x20;
			s_cot_sample_msg_tx.dst_dev_id=(((s_cot_sample_msg_tx.dst_dev_id&0xff)<<24)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff00)<<8)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff0000)>>8)|
								            ((s_cot_sample_msg_tx.dst_dev_id&0xff000000)>>24));
			memcpy( &printf_buff[index],(char *)&s_cot_sample_msg_tx.dst_dev_id,4 );
			index +=4;
			Value_to_String((uint8_t *)&cot_ranging_res.moy,8,&printf_buff[index]);
			index +=8;
			Value_to_String((uint8_t *)&cot_ranging_res.squre_devi,8,&printf_buff[index]);
			index +=8;
			memcpy( &printf_buff[index++],(char *)&cot_ranging_res.rssi,1 );
			memcpy( &printf_buff[index++],(char *)&cot_ranging_res.zn,1 );
			Value_to_String((uint8_t *)&cot_ranging_res.zmoy,8,&printf_buff[index]);
			index +=8;
			Value_to_String((uint8_t *)&cot_ranging_res.fei,8,&printf_buff[index]);
			index +=8;
			Value_to_String((uint8_t *)&cot_ranging_res.distance,8,&printf_buff[index]);
			index +=8;
			mt_osal_msg_data_t *p_msg = (mt_osal_msg_data_t*)osal_msg_allocate( sizeof(mt_osal_msg_data_t)+sizeof(printf_buff) );
            if( p_msg )
			{
				p_msg->hdr.event = CMD_SERIAL_MSG;
				p_msg->event_cmd = SERIAL_EVENT_SEND_CMD;
				p_msg->task_id = s_sample_app_id;
				p_msg->msg_len = index;
				p_msg->msg = (uint8_t*)(p_msg+1);
				memcpy(p_msg->msg,printf_buff,p_msg->msg_len);
				osal_msg_send(get_mt_task_id(),(uint8_t*)p_msg);
			}   			
        }
        osal_set_event(task_id, SMAPLE_MSG_INIT_EVENT);
        return (events ^ SMAPLE_MSG_RNG_EVENT);
   }

    if ( events & SMAPLE_MSG_TX_DONE_EVENT )
	{

      if(s_cot_sample_msg_tx.arq_num >0 )
        {
            s_cot_sample_msg_tx.arq_num--;
            seq_no++;
            switch( s_cot_sample_msg_tx.data[0] )
            {
                case COT_SYSTEM_CMD_H:
                    {
                        switch( s_cot_sample_msg_tx.data[1] )
                        {
							case SAMPLE_APP_CMD_BROADCAST:
                                {
									osal_start_timerEx(task_id,SMAPLE_MSG_TX_EVENT,150);
                                }
                                break;
                            case SMAPLE_APP_CMD_SEND_PKG:
                                {
									s_cot_sample_msg_tx.data[4] = seq_no;
                                    osal_start_timerEx(task_id,SMAPLE_MSG_TX_EVENT,150);
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
        else
        {
			if( s_config_data.RFParamSettings.ModulationType == RF_PACKET_TYPE_RANGING )
			{
				osal_set_event( task_id,SMAPLE_MSG_CONFIG_EVENT );
			}
            else
			    //osal_set_event(task_id, SMAPLE_MSG_RX_EVENT);
			    osal_start_timerEx(task_id,SMAPLE_MSG_RX_EVENT,20);
			
			
            osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
            
        }

        return (events ^ SMAPLE_MSG_TX_DONE_EVENT);
    }

    if ( events & SMAPLE_MSG_TX_TIMEOUT_EVENT )
    {
        osal_start_timerEx(task_id,SMAPLE_MSG_TX_EVENT,100);
        osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
        return (events ^ SMAPLE_MSG_TX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_TIMEOUT_EVENT )
    {
		osal_start_timerEx(task_id,SMAPLE_MSG_RX_EVENT,100);      
        osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);		
        return (events ^ SMAPLE_MSG_RX_TIMEOUT_EVENT);
    }

    if ( events & SMAPLE_MSG_RX_ERROR_EVENT )
    {
		osal_set_event(task_id,SMAPLE_MSG_RX_EVENT);
        osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE);
        return (events ^ SMAPLE_MSG_RX_ERROR_EVENT);
    }
	
    return ret ;
}

