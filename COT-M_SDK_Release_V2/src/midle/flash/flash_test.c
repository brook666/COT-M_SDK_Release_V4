/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: flash_test.c
*
* Author: wuwd
*
* Version: 1.0
*
* Date: 2017-03-20
*
* Description: the functions of serial virtual file system
*
* Function List:
*
* History:
*--------------------------------------------------------------------------*/


#define FLASHTEST_DEBUG  


#ifdef FLASHTEST_DEBUG
/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include "am_bsp.h"
#include "am_util.h"
#include <stdint.h>
#include <string.h> 
#include "vfs.h"
#include "flash.h"







/*-----------------------------------------------------------------------------
* Function Name  : flashTest
* Description    : 功能验证测试flashTest
* Input          : - None.
* Output         : - None.
* Return         : 
* Notes          : 






                   20170320 wuwd.
-------------------------------------------------------------------------------*/
int flashTest(uint32_t  uart1_fd)
{

    uint8_t g_recv_buf[128];
    
    uint32_t flash_fd; /*!< flash file descriptor */
    uint32_t i = 0 ;
    volatile int32_t  i32ReturnCode = 0;            /* flash status */
    
    FLASH_InitModule();
    flash_fd = open( PATH_FLASH, NULL );  /* open file */

    lseek( flash_fd, 0x40000 , SEEK_SET);

    read( flash_fd , g_recv_buf , 0x10);/* read  flash */
    write(uart1_fd,g_recv_buf,0x10);
    
    for(i = 0;i< 0x10 ;i++) 
        g_recv_buf[i] = i*i + 1;

    
    
    lseek( flash_fd, 0x40000 , SEEK_SET);
    i32ReturnCode = write(flash_fd,g_recv_buf,0x10); /* write flash */


    lseek( flash_fd, 0x40000 , SEEK_SET);
    read( flash_fd , g_recv_buf , 0x10);/* read  flash */
    write(uart1_fd,g_recv_buf,0x10);

    close(flash_fd);


    return 0;
}


#endif


