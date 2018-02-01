
#ifndef FLASH_H
#define FLASH_H

/*********************************************************************
    Filename:       flash.h
    Revised:        $Date: 2017-03-14 14:57:55 -0700 $
    Revision:       $Revision: 1 $

    Description:

       This file contains the flash virtual file system Management API.

    Notes:

    Copyright (c) 2017 by COTiot Instruments, Inc.
    All Rights Reserved.  Permission to use, reproduce, copy, prepare
    derivative works, modify, distribute, perform, display or sell this
    software and/or its documentation for any purpose is prohibited
    without the express written consent of COTiot, Inc.
*********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
 #include <stdint.h>

/*********************************************************************
 * MACROS
 */
/* visual file list */
#define PATH_FLASH      ("flash")   /*!< internal flash */

/*flash devie module */
#define FLASH_PAGE_SIZE                 (0x800)                 /* flash page size */
#define FLASH_PAGE_SIZE_MASK            ((uint32_t)0x7FF)       /* flash page size */
#define FLASH_SIZE                      (0x80000)               /* flash size */

#define FLASH_OTA_INFO_STARTADDR        (0x40000)               /* OTA_INFO start addr */
#define FLASH_OTA_INFO_SIZE             (0x100)                 /* OTA_INFO size */
#define FLASH_OTA_DATA_STARTADDR        (0x40800)               /* OTA_DATA start addr */
#define FLASH_OTA_DATA_SIZE             (0x3C000)               /* OTA_DATA size */

#define FLASH_OFFSET_VERSION            (72)                    /* 软件版本号偏移量 */ 
#define FLASH_VERSION_SIZE              ( 6)                    /* 软件版本号字节数 */ 

#define FLASH_OFFSET_ENCFILESIZE        (84)                    /* 密文文件大小偏移量  */
#define FLASH_ENCFILESIZE_SIZE          ( 4)                    /* 密文文件大小字节数  */

#define FLASH_OFFSET_FILESIZE           (88)                    /* 明文文件大小偏移量  */
#define FLASH_FILESIZE_SIZE             ( 4)                    /* 明文文件大小字节数  */

#define FLASH_OFFSET_HASH               (92)                    /* sha256值偏移量  */
#define FLASH_HASH_SIZE                 (32)                    /* sha256值字节数  */



/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */

int32_t FLASH_InitModule(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif




#endif


