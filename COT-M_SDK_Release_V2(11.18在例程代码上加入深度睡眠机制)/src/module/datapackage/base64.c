/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: base64.c
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
#include <string.h>

#include "base64.h"



/*********************************************************************
 * MACROS
 */


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
/*base64加密算法字符表*/
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        


/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/


/*******************************************************************************
* 函 数 名: base64Encode
* 功能描述: base64加密接口
* 输入参数: bindata：待加密的数据
*           base64：加密后的数据缓存指针
*           binlength：待加密的数据长度
* 输出参数：base64：加密后的数据
* 返 回 值: 加密后的数据长度
********************************************************************************/
uint16_t base64Encode( const uint8_t *bindata,uint16_t binlength,uint8_t *base64 )
{
    uint16_t i = 0, j = 0 ; 
    uint8_t current = 0;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;

        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return (j);
}

/*******************************************************************************
* 函 数 名: base64Decode
* 功能描述: base64解密接口
* 输入参数: base64：待解密的数据
*           bindata：解密后的数据缓存指针
*           base64len:待解密数据长度
* 输出参数：bindata：解密后的数据
* 返 回 值: 解密后的数据长度
********************************************************************************/
uint16_t base64Decode( const uint8_t * base64,uint16_t base64len, uint8_t *bindata)
{
    uint16_t i = 0, j = 0;
    uint8_t k = 0;
    uint8_t temp[4] = {0};
    for ( i = 0, j = 0; i < base64len ; i += 4 )
    {
        memset( temp, 0xFF, 4);
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}



