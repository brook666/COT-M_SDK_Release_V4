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
         
#define CMD_LEN         2   //命令长度 
#define DATALEN_LEN     4   //数据格式长度  拆分后长度
         
#define MIN_LEN         10   //最小长度
#define MAX_LEN         128  //最大长度         
    


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
* @Description:   校验数据
* @param data      - 需要计算校验值的字符串
* @param datalen   - 需要计算校验值的字符串长度
* @return  校验值
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
* @Description:   拆分数据，拆分格式为高4位和低4位分别加0x30
* @param hex      - 需要拆分的数据
* @param asc      - 拆分的数据指针
* @return 无
*/
static void HexToTwoAsc(uint8_t hex,uint8_t *asc)
{
    asc[0] = (hex >> 4) + 0x30;
    asc[1] = (hex & 0x0f) + 0x30;
}

/**
* @Description:   两个拆分后数据合并为原始数据
* @param asc      - 两个数据的指针
* @return 合并后的原始数据
*/
static uint8_t TwoAscToHex(uint8_t *asc)
{
    return ((asc[0]- 0x30) << 4 ) | (asc[1]- 0x30);
}

/**
* @Description:         封装数据包
* @param send_data      - 需要封装的字符串数据
* @param outdata        - 接收到的数据长度
* @param outdata_len    - 输出解析数据
* @return SUCCESS : 成功   FAILURE: 失败
*/
uint8_t makeDataPackage(packagedata_t send_data,uint8_t *outdata,uint16_t *outdata_len)
{
    uint16_t len = 0,tem = 0;

    tem =  send_data.data_len / 3 * 4;
    if(send_data.data_len % 3)
        tem += 4;
    
    outdata[len++] = SOH;
    
    HexToTwoAsc((tem + CMD_LEN)>>8,outdata+ len); // len    拆分
    len += 2;
    HexToTwoAsc((tem + CMD_LEN) & 0x0ff,outdata+ len);  
    len += 2;
    
    if(send_data.cmd != 0) 
    {
        outdata[len++] = send_data.cmd >> 8;    // CMD  不拆分
        outdata[len++] = send_data.cmd & 0x0ff; 
    }
    
    tem = base64Encode(send_data.data,send_data.data_len,outdata+ len); // data  不拆分
    len += tem;

    
    tem = LRCcheck(outdata,len);  // lrc     拆分
    HexToTwoAsc(tem,outdata+ len);   
    len += 2;

    outdata[len++] = EOT;
    *outdata_len = len;
    return SUCCESS;
}


/**
* @Description: 处理数据 
* @param redata     - 接收到的字符串数据
* @param redata_len - 接收到的数据长度
* @param Receive_data 输出解析数据
* @return SUCCESS :解析成功   FAILURE: 解析失败
*/
uint8_t ReceiveHandle(const uint8_t *redata,const uint16_t redata_len, packagedata_t *receive_data)
{
    uint16_t base = 0,datalen = 0;
    
    while(redata_len >= (base + MIN_LEN))
    {
        if(redata[base] == SOH)
        {
            datalen = ((uint16_t)TwoAscToHex((uint8_t*)&redata[base+1]) << 8 ) | (uint16_t)TwoAscToHex((uint8_t*)&redata[base+3]);
            if(((base + 1 + DATALEN_LEN + datalen + 2 + 1) <= redata_len)&&(redata[base + 1 + DATALEN_LEN + datalen + 2] == EOT)) /* 确认头尾 */ 
            {
                if(LRCcheck((uint8_t*)&redata[base],1+DATALEN_LEN+ datalen) == \
                    TwoAscToHex((uint8_t*)&redata[base + 1 + DATALEN_LEN + datalen])) /*校验*/ 
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
* @Description: 校验接收数据，每次检查一个包
* @param redata     - 接收到的字符串数据
* @param redata_len - 接收到的数据长度
* @param count      - 输出数据包开始位置 
* @return  已经检查的数据长度
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
                if(((base + len) <= redata_len)&&(redata[base + len - 1] == EOT)) /* 确认头尾 */ 
                { 
                    if(LRCcheck((uint8_t*)&redata[base],1+DATALEN_LEN+ datalen) == \
                        TwoAscToHex((uint8_t*)&redata[base + 1 + DATALEN_LEN + datalen])) /*校验*/ 
                    {
                        *count = base;
                        return len;
                    }
                }
            }
        }
        base++;
    }
    if((base - start) > MAX_LEN)  /*  大于最大数据包 ,舍去*/
    {
        start = base;
    }
    *count = start;
    return 0;

}





