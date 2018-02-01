/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: data_package.c
*
* Author: wende.wu
*
* Version: 1.0
*
* Date: 2017-03-31
*
* Description:  
*
* Function List:
*
* History:
*--------------------------------------------------------------------------*/

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "base64.h"
#include "data_package.h"
#include "osal.h"     
     


/*********************************************************************
 * MACROS
 */
#define SOH             0x02
#define EOT             0x03
         
#define CMD_LEN         2   //����� 
#define DATALEN_LEN     4   //���ݸ�ʽ����  ��ֺ󳤶�
         
#define MIN_LEN         10   //��С����
#define MAX_LEN         128  //��󳤶�         
    


/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

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


/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/
    
/**
* @Description:   У������
* @param data      - ��Ҫ����У��ֵ���ַ���
* @param datalen   - ��Ҫ����У��ֵ���ַ�������
* @return  У��ֵ
*/
uint8_t LRCcheck(uint8_t *data,uint16_t datalen)
{
    uint8_t lrc = 0;
    
    while(datalen > 0)
    {
        datalen --;
        lrc += data[datalen];        
    }
    return (0-lrc);
}

/**
* @Description:   ������ݣ���ָ�ʽΪ��4λ�͵�4λ�ֱ��0x30
* @param hex      - ��Ҫ��ֵ�����
* @param asc      - ��ֵ�����ָ��
* @return ��
*/
static void HexToTwoAsc(uint8_t hex,uint8_t *asc)
{
    asc[0] = (hex >> 4) + 0x30;
    asc[1] = (hex & 0x0f) + 0x30;
}

/**
* @Description:   ������ֺ����ݺϲ�Ϊԭʼ����
* @param asc      - �������ݵ�ָ��
* @return �ϲ����ԭʼ����
*/
static uint8_t TwoAscToHex(uint8_t *asc)
{
    return ((asc[0]- 0x30) << 4 ) | (asc[1]- 0x30);
}

/**
* @Description:         ��װ���ݰ�
* @param send_data      - ��Ҫ��װ���ַ�������
* @param outdata        - ���յ������ݳ���
* @param outdata_len    - �����������
* @return SUCCESS : �ɹ�   FAILURE: ʧ��
*/
uint8_t makeDataPackage(packagedata_t send_data,uint8_t *outdata,uint16_t *outdata_len)
{
    uint16_t len = 0,tem = 0;

    tem =  send_data.data_len / 3 * 4;
    if(send_data.data_len % 3)
        tem += 4;
    
    outdata[len++] = SOH;
    
    HexToTwoAsc((tem + CMD_LEN)>>8,outdata+ len); // len    ���
    len += 2;
    HexToTwoAsc((tem + CMD_LEN) & 0x0ff,outdata+ len);  
    len += 2;
    
    if(send_data.cmd != 0) 
    {
        outdata[len++] = send_data.cmd >> 8;    // CMD  �����
        outdata[len++] = send_data.cmd & 0x0ff; 
    }
    
    tem = base64Encode(send_data.data,send_data.data_len,outdata+ len); // data  �����
    len += tem;

    
    tem = LRCcheck(outdata,len);  // lrc     ���
    HexToTwoAsc(tem,outdata+ len);   
    len += 2;

    outdata[len++] = EOT;
    *outdata_len = len;
    return SUCCESS;
}


/**
* @Description: �������� 
* @param redata     - ���յ����ַ�������
* @param redata_len - ���յ������ݳ���
* @param Receive_data �����������
* @return SUCCESS :�����ɹ�   FAILURE: ����ʧ��
*/
uint8_t ReceiveHandle(const uint8_t *redata,const uint16_t redata_len, packagedata_t *receive_data)
{
    uint16_t base = 0,datalen = 0;
    
    while(redata_len >= (base + MIN_LEN))
    {
        if(redata[base] == SOH)
        {
            datalen = ((uint16_t)TwoAscToHex((uint8_t*)&redata[base+1]) << 8 ) | (uint16_t)TwoAscToHex((uint8_t*)&redata[base+3]);
            if(((base + 1 + DATALEN_LEN + datalen + 2 + 1) <= redata_len)&&(redata[base + 1 + DATALEN_LEN + datalen + 2] == EOT)) /* ȷ��ͷβ */ 
            {
                if(LRCcheck((uint8_t*)&redata[base],1+DATALEN_LEN+ datalen) == \
                    TwoAscToHex((uint8_t*)&redata[base + 1 + DATALEN_LEN + datalen])) /*У��*/ 
                {
                    receive_data->cmd = ((uint16_t)redata[base + 1 + DATALEN_LEN]) << 8 | (uint16_t)redata[base + 1 + DATALEN_LEN + 1];
                    if(receive_data->data != 0)
                        receive_data->data_len = base64Decode(&redata[base + 1 + DATALEN_LEN + CMD_LEN],datalen - CMD_LEN,receive_data->data);
                    return SUCCESS;
                }
            }
        }
        base++;
    }
    return FAILURE;

}


/**
* @Description: У��������ݣ�ÿ�μ��һ����
* @param redata     - ���յ����ַ�������
* @param redata_len - ���յ������ݳ���
* @param count      - ������ݰ���ʼλ�� 
* @return  �Ѿ��������ݳ���
*/
uint16_t check_receivedata(const uint8_t *redata,const uint16_t redata_len, uint16_t *count)
{
    uint16_t base = 0,datalen = 0;
    uint16_t len = 0,start = 0;

    *count = 0;
    while(redata_len > base)
    {
        if(redata[base] == SOH)
        {  
            start = base;
            if(redata_len >= (base + MIN_LEN))
            {
                datalen = ((uint16_t)TwoAscToHex((uint8_t*)&redata[base+1]) << 8 ) | (uint16_t)TwoAscToHex((uint8_t*)&redata[base+3]);
                len = 1 + DATALEN_LEN + datalen + 2 + 1;
                if(((base + len) <= redata_len)&&(redata[base + len - 1] == EOT)) /* ȷ��ͷβ */ 
                { 
                    if(LRCcheck((uint8_t*)&redata[base],1+DATALEN_LEN+ datalen) == \
                        TwoAscToHex((uint8_t*)&redata[base + 1 + DATALEN_LEN + datalen])) /*У��*/ 
                    {
                        *count = base;
                        return len;
                    }
                }
            }
        }
        base++;
    }
    if((base - start) > MAX_LEN)  /*  ����������ݰ� ,��ȥ*/
    {
        start = base;
    }
    *count = start;
    return 0;

}





